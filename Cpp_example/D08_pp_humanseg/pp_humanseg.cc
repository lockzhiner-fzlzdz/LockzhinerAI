#include <iostream>
#include <cmath>
#include <opencv2/opencv.hpp>
#include "rknpu2_backend/rknpu2_backend.h"
#include <chrono>
#include <cstdlib> 
#include <ctime> 

using namespace std::chrono;
// 输入和输出量化的scale和zeropoint
const float INPUT_SCALE = 1.0f / 255.0f; 
const int INPUT_ZP = -128;
const float OUTPUT_SCALE = 0.034558f;
const int OUTPUT_ZP = -128;

// 预处理函数
cv::Mat preprocess(const cv::Mat& image, const std::vector<size_t>& input_dims) {
    // 确保输入维度为NHWC [1, H, W, 3]
    if (input_dims.size() != 4 || input_dims[0] != 1 || input_dims[3] != 3) {
        std::cerr << "Invalid input dimensions" << std::endl;
        return cv::Mat();
    }

    const size_t input_h = input_dims[1];
    const size_t input_w = input_dims[2];

    // Resize并转换颜色空间
    cv::Mat resized, rgb;
    cv::resize(image, resized, cv::Size(input_w, input_h));
    cv::cvtColor(resized, rgb, cv::COLOR_BGR2RGB);

    // 量化到INT8
    cv::Mat quantized;
    rgb.convertTo(quantized, CV_8S, 255.0 * INPUT_SCALE, INPUT_ZP);

    return quantized;
}

// 后处理函数
cv::Mat postprocess(const rknn_tensor_mem* output_mem, 
                   const std::vector<size_t>& output_dims,
                   const cv::Size& target_size) {
    // 验证输出维度为NCHW [1, 2, H, W]
    if (output_dims.size() != 4 || output_dims[0] != 1 || output_dims[1] != 2) {
        std::cerr << "Invalid output dimensions" << std::endl;
        return cv::Mat();
    }

    const int8_t* data = static_cast<const int8_t*>(output_mem->virt_addr);
    const int h = output_dims[2];
    const int w = output_dims[3];

    // ================= 1. 概率图生成优化 =================
    cv::Mat prob_map(h, w, CV_32FC1);
    // 基于192x192模型的缩放补偿
    float spatial_weight = 1.0f - (h * w) / (192.0f * 192.0f); 
    
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            const int bg_idx = 0 * h * w + y * w + x;
            const int fg_idx = 1 * h * w + y * w + x;
            // 带饱和保护的反量化
            float bg_logit = std::clamp((data[bg_idx] - OUTPUT_ZP) * OUTPUT_SCALE, -10.0f, 10.0f);
            float fg_logit = std::clamp((data[fg_idx] - OUTPUT_ZP) * OUTPUT_SCALE, -10.0f, 10.0f);

            // 空间注意力加权（中心区域增强）
            float center_weight = 1.0f - (std::abs(x - w/2.0f)/(w/2.0f) + std::abs(y - h/2.0f)/(h/2.0f))/2.0f;
            fg_logit *= (1.2f + 0.3f * center_weight * spatial_weight);

            // 稳健的Softmax计算
            float max_logit = std::max(bg_logit, fg_logit);
            float exp_sum = expf(bg_logit - max_logit) + expf(fg_logit - max_logit);
            prob_map.at<float>(y, x) = expf(fg_logit - max_logit) / (exp_sum + 1e-8f);
        }
    }

    // ================= 2. 自适应阈值优化 =================
    cv::Mat binary_mask;
    // 重点区域检测
    cv::Mat prob_roi = prob_map(cv::Rect(w/4, h/4, w/2, h/2)); 
    float center_mean = cv::mean(prob_roi)[0];
    float dynamic_thresh = std::clamp(0.45f - (center_mean - 0.5f) * 0.3f, 0.25f, 0.65f);
    
    cv::threshold(prob_map, binary_mask, dynamic_thresh, 255, cv::THRESH_BINARY);
    binary_mask.convertTo(binary_mask, CV_8U);

    // ================= 3. 多尺度形态学处理 =================
    std::vector<cv::Mat> mask_pyramid;
    // 构建金字塔
    cv::buildPyramid(binary_mask, mask_pyramid, 2); 
    
    // 小尺度去噪
    cv::Mat kernel1 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3,3));
    cv::morphologyEx(mask_pyramid[1], mask_pyramid[1], cv::MORPH_OPEN, kernel1);
    
    // 中尺度填充
    cv::Mat kernel2 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5,5));
    cv::morphologyEx(mask_pyramid[0], mask_pyramid[0], cv::MORPH_CLOSE, kernel2);
    
    // 金字塔重建
    cv::Mat refined_mask;
    cv::pyrUp(mask_pyramid[1], refined_mask, mask_pyramid[0].size());
    cv::bitwise_and(refined_mask, mask_pyramid[0], refined_mask);

    // ================= 4. 智能边缘优化 =================
    cv::Mat edge_weights;
    cv::distanceTransform(refined_mask, edge_weights, cv::DIST_L2, 3);
    cv::normalize(edge_weights, edge_weights, 0, 1.0, cv::NORM_MINMAX);
    
    cv::Mat probabilistic_edges;
    cv::Canny(refined_mask, probabilistic_edges, 50, 150);
    probabilistic_edges.convertTo(probabilistic_edges, CV_32F, 1.0/255.0);
    
    cv::Mat final_edges;
    cv::multiply(probabilistic_edges, edge_weights, final_edges);
    final_edges.convertTo(final_edges, CV_8U, 255.0);

    // ================= 5. 多模态融合输出 =================
    cv::Mat resized_mask;
    cv::resize(refined_mask, resized_mask, target_size, 0, 0, cv::INTER_LANCZOS4);
    
    cv::Mat final_mask;
    cv::bilateralFilter(resized_mask, final_mask, 5, 15, 15);
    
    // 最终保边处理
    cv::Mat edge_mask_hr;
    cv::resize(final_edges, edge_mask_hr, target_size, 0, 0, cv::INTER_NEAREST);
    final_mask.setTo(255, edge_mask_hr > 128);

    return final_mask;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <model_path> <image_path>" << std::endl;
        return 1;
    }

    const std::string model_path = argv[1];
    const std::string image_path = argv[2];

    // 初始化RKNN后端
    lockzhiner_vision_module::vision::RKNPU2Backend backend;
    if (!backend.Initialize(model_path)) {
        std::cerr << "Failed to initialize RKNN backend" << std::endl;
        return -1;
    }

    // 加载图像
    cv::Mat image = cv::imread(image_path);
    if (image.empty()) {
        std::cerr << "Failed to read image: " << image_path << std::endl;
        return -1;
    }

    // 获取输入属性
    const auto& input_attrs = backend.GetInputAttrs();
    if (input_attrs.empty()) {
        std::cerr << "No input attributes found" << std::endl;
        return -1;
    }
    const auto& input_attr = input_attrs[0];
    std::vector<size_t> input_dims(input_attr.dims, input_attr.dims + input_attr.n_dims);

    // 预处理
    cv::Mat preprocessed = preprocess(image, input_dims);
    if (preprocessed.empty()) {
        std::cerr << "Preprocessing failed" << std::endl;
        return -1;
    }

    // 验证输入数据尺寸
    const size_t expected_input_size = input_attr.size_with_stride;
    const size_t actual_input_size = preprocessed.total() * preprocessed.elemSize();
    if (expected_input_size != actual_input_size) {
        std::cerr << "Input size mismatch! Expected: " << expected_input_size
                  << ", Actual: " << actual_input_size << std::endl;
        return -1;
    }

    // 拷贝输入数据
    const auto& input_memories = backend.GetInputMemories();
    if (input_memories.empty() || !input_memories[0]) {
        std::cerr << "Invalid input memory" << std::endl;
        return -1;
    }
    memcpy(input_memories[0]->virt_addr, preprocessed.data, actual_input_size);

    // 执行推理
    high_resolution_clock::time_point start_time =
          high_resolution_clock::now();
    if (!backend.Run()) {
        std::cerr << "Inference failed" << std::endl;
        return -1;
    }
    
    // 获取输出
    const auto& output_attrs = backend.GetOutputAttrs();
    if (output_attrs.empty()) {
        std::cerr << "No output attributes found" << std::endl;
        return -1;
    }
    const auto& output_memories = backend.GetOutputMemories();
    if (output_memories.empty() || !output_memories[0]) {
        std::cerr << "Invalid output memory" << std::endl;
        return -1;
    }

    // 后处理
    const auto& output_attr = output_attrs[0];
    std::vector<size_t> output_dims(output_attr.dims, output_attr.dims + output_attr.n_dims);
    cv::Mat mask = postprocess(output_memories[0], output_dims, image.size());
    high_resolution_clock::time_point end_time = high_resolution_clock::now();
    auto time_span = duration_cast<milliseconds>(end_time - start_time);
    std::cout << "单张图片推理时间(ms): " << time_span.count() << std::endl;
    // 生成结果
    cv::Mat result;
    cv::bitwise_and(image, image, result, mask);

    // 保存结果
    const std::string output_path = "result.jpg";
    cv::imwrite(output_path, result);
    std::cout << "Result saved to: " << output_path << std::endl;

    // 显示调试视图
    cv::imshow("Original", image);
    cv::imshow("Mask", mask);
    cv::imshow("Result", result);
    cv::waitKey(0);

    return 0;
}
