#include <unistd.h>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <ncnn/net.h>
#include <chrono>
#include <cstdlib>
#include <ctime>

using namespace cv;
using namespace std::chrono;


// 定义必要的参数
const float meanValues[3] = {0.485f * 255, 0.456f * 255, 0.406f * 255};
const float normValues[3] = {1.0f / 0.229f / 255.0f, 1.0f / 0.224f / 255.0f, 1.0f / 0.225f / 255.0f};

// 图像缩放参数定义
struct ScaleParam {
    int srcWidth;
    int srcHeight;
    int dstWidth;
    int dstHeight;
    float ratioWidth;
    float ratioHeight;
};

// 文本框结构体的定义
struct TextBox {
    std::vector<cv::Point> boxPoint;
    float score;
};

// 计算图像缩放比例以适应目标尺寸
ScaleParam getScaleParam(const cv::Mat &src, const int targetSize) {
    int imgHeight = src.rows;
    int imgWidth = src.cols;
    float ratio = std::min(static_cast<float>(targetSize) / imgHeight, static_cast<float>(targetSize) / imgWidth);
    ScaleParam scaleParam;
    scaleParam.srcHeight = imgHeight;
    scaleParam.srcWidth = imgWidth;
    scaleParam.dstHeight = static_cast<int>(imgHeight * ratio);
    scaleParam.dstWidth = static_cast<int>(imgWidth * ratio);
    scaleParam.ratioHeight = ratio;
    scaleParam.ratioWidth = ratio;
    return scaleParam;
}

// 向量归一化函数
cv::Point2f normalize(const cv::Point2f& v) {
    float len = std::sqrt(v.x * v.x + v.y * v.y);
    // 防止除以零
    if (len == 0) return cv::Point2f(0, 0); 
    return cv::Point2f(v.x / len, v.y / len);
}

// 多边形扩张函数：用于扩大文本框边界
std::vector<cv::Point> expandPolygon(const std::vector<cv::Point>& inBox, float distance) {
    std::vector<cv::Point> outBox;
    int n = inBox.size();
    // 确保输入至少是一个三角形
    if (n < 3) return outBox; 

    for (int i = 0; i < n; ++i) {
        cv::Point2f prev = inBox[(i + n - 1) % n];
        cv::Point2f curr = inBox[i];
        cv::Point2f next = inBox[(i + 1) % n];

        cv::Point2f v1 = cv::Point2f(curr.x - prev.x, curr.y - prev.y);
        cv::Point2f v2 = cv::Point2f(next.x - curr.x, next.y - curr.y);

        cv::Point2f normal1(-v1.y, v1.x);
        cv::Point2f normal2(-v2.y, v2.x);

        normal1 = normalize(normal1);
        normal2 = normalize(normal2);

        cv::Point2f bisectorNormal = normal1 + normal2;
        bisectorNormal = normalize(bisectorNormal);

        cv::Point2f newPoint = curr + bisectorNormal * distance;

        outBox.push_back(cv::Point(newPoint.x, newPoint.y));
    }

    return outBox;
}

// 查找并处理文本框区域
std::vector<TextBox> findRsBoxes(const cv::Mat &fMapMat, const cv::Mat &norfMapMat, ScaleParam &s,
                                 const float boxScoreThresh, const float unClipRatio) {
    float minArea = 3;
    std::vector<TextBox> rsBoxes;
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(norfMapMat, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    for (size_t i = 0; i < contours.size(); ++i) {
        double perimeter = cv::arcLength(contours[i], true);
        cv::RotatedRect minRect = cv::minAreaRect(contours[i]);
        float minSideLen = std::min(minRect.size.width, minRect.size.height);

        if (minSideLen < minArea)
            continue;

        // 创建一个掩码图像
        cv::Mat mask = cv::Mat::zeros(fMapMat.size(), CV_8UC1);
        cv::drawContours(mask, contours, static_cast<int>(i), cv::Scalar(255), cv::FILLED);

        // 计算掩码内部的平均得分
        cv::Scalar meanScore = cv::mean(fMapMat, mask);
        float score = static_cast<float>(meanScore[0]);

        if (score < boxScoreThresh)
            continue;

        // 使用expandPolygon实现多边形扩张
        double area = cv::contourArea(contours[i]);
        float distance = unClipRatio * area / static_cast<float>(perimeter);
        std::vector<cv::Point> clipBox = expandPolygon(contours[i], distance);

        if (minSideLen < minArea + 2)
            continue;

        for (auto &point : clipBox) {
            point.x = std::max(0, std::min(static_cast<int>(point.x / s.ratioWidth), s.srcWidth - 1));
            point.y = std::max(0, std::min(static_cast<int>(point.y / s.ratioHeight), s.srcHeight - 1));
        }

        rsBoxes.emplace_back(TextBox{clipBox, score});
    }
    return rsBoxes;
}

// 绘制文本框到图像上
void drawTextBox(cv::Mat &boxImg, const std::vector<cv::Point> &box, int thickness) {
    for (size_t i = 0; i < box.size(); ++i) {
        cv::line(boxImg, box[i], box[(i + 1) % box.size()], cv::Scalar(0, 255, 0), thickness);
    }
}

int main(int argc, char** argv) {
    if(argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <image_path> <param_path> <bin_path>" << std::endl;
        return -1;
    }
    // 读取输入图像
    cv::Mat src = cv::imread(argv[1], cv::IMREAD_COLOR); 
    if(src.empty()) {
        std::cerr << "Failed to read image: " << argv[1] << std::endl;
        return -1;
    }

    ncnn::Net net;
    // 禁用一些可能不被支持的功能
    net.opt.use_vulkan_compute = false; 
    net.opt.use_bf16_storage = false;
    net.opt.use_fp16_packed = false;
    net.opt.use_fp16_storage = false;
    net.opt.use_fp16_arithmetic = false;

    // 加载模型参数和权重文件
    if (net.load_param(argv[2]) != 0 || net.load_model(argv[3]) != 0) {
        std::cerr << "Failed to load model from " << argv[2] << " and " << argv[3] << std::endl;
        return -1;
    }
    std::cout << "Model loaded successfully." << std::endl;
    // 736是自定义的参数，可以自己设置
    ScaleParam scaleParam = getScaleParam(src, 736);

    std::cout << " scaleParam successfully." << std::endl;
    float boxScoreThresh = 0.5f;
    float boxThresh = 0.3f;
    float unClipRatio = 3.5f;

    cv::Mat srcResize;
    resize(src, srcResize, cv::Size(scaleParam.dstWidth, scaleParam.dstHeight));
    std::cout << " resize successfully." << std::endl;

    // 准备输入数据，并进行均值标准化
    ncnn::Mat input = ncnn::Mat::from_pixels(srcResize.data, ncnn::Mat::PIXEL_BGR, srcResize.cols, srcResize.rows);
    input.substract_mean_normalize(meanValues, normValues);

    // 创建推理器并执行推理
    high_resolution_clock::time_point start_time =
          high_resolution_clock::now();
    ncnn::Extractor extractor = net.create_extractor();
    extractor.input("input0", input);
    ncnn::Mat out;
    extractor.extract("out1", out);
    high_resolution_clock::time_point end_time = high_resolution_clock::now();
    // 计算推理时间
    auto time_span = duration_cast<milliseconds>(end_time - start_time);
    std::cout << "单张图片推理时间(ms): " << time_span.count() << std::endl;

    // 创建一个单通道的 cv::Mat 来存储第一个通道的数据
    cv::Mat fMapMat(srcResize.rows, srcResize.cols, CV_32FC1);

    memcpy(fMapMat.data, (float *) out.data, srcResize.rows * srcResize.cols * sizeof(float));

    cv::Mat norfMapMat;
    norfMapMat = fMapMat > boxThresh;
    
    // 查找文本框并绘制在原图上
    std::vector<TextBox> textBoxes = findRsBoxes(fMapMat, norfMapMat, scaleParam, boxScoreThresh, unClipRatio);
    
    for (const auto &textBox : textBoxes) {
      drawTextBox(src, textBox.boxPoint, 1);
    }
    cv::imshow("Detected Text Boxes", src);
    cv::waitKey(0);

    return 0;
}