#include <iostream>
#include <opencv2/opencv.hpp>
#include <ncnn/net.h>
#include <chrono>
#include <vector>
#include <fstream>
#include <numeric>
#include <cmath>

using namespace cv;
using namespace std::chrono;

// 定义必要的参数
const float dbnet_meanValues[3] = {0.485f * 255, 0.456f * 255, 0.406f * 255};
const float dbnet_normValues[3] = {1.0f / 0.229f / 255.0f, 1.0f / 0.224f / 255.0f, 1.0f / 0.225f / 255.0f};

// 文本检测结构体
struct TextBox {
    std::vector<cv::Point> boxPoint;
    float score;
    std::string text;
};

// 图像缩放参数定义
struct ScaleParam {
    int srcWidth;
    int srcHeight;
    int dstWidth;
    int dstHeight;
    float ratioWidth;
    float ratioHeight;
};

// 文本检测相关函数
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

cv::Point2f normalize(const cv::Point2f& v) {
    float len = std::sqrt(v.x * v.x + v.y * v.y);
    return (len == 0) ? cv::Point2f(0, 0) : cv::Point2f(v.x / len, v.y / len);
}

// 
std::vector<cv::Point> expandPolygon(const std::vector<cv::Point>& inBox, float distance) {
    std::vector<cv::Point> outBox;
    int n = inBox.size();
    if (n < 3) return outBox; 

    for (int i = 0; i < n; ++i) {
        cv::Point2f prev = inBox[(i + n - 1) % n];
        cv::Point2f curr = inBox[i];
        cv::Point2f next = inBox[(i + 1) % n];

        cv::Point2f v1 = curr - prev;
        cv::Point2f v2 = next - curr;

        cv::Point2f normal1(-v1.y, v1.x);
        cv::Point2f normal2(-v2.y, v2.x);

        normal1 = normalize(normal1);
        normal2 = normalize(normal2);

        cv::Point2f bisectorNormal = normalize(normal1 + normal2);
        cv::Point2f newPoint = curr + bisectorNormal * distance;

        outBox.push_back(cv::Point(newPoint.x, newPoint.y));
    }
    return outBox;
}

// 提取检测到文本的检测框
std::vector<TextBox> findRsBoxes(const cv::Mat &fMapMat, const cv::Mat &norfMapMat, ScaleParam &s,
                                 const float boxScoreThresh, const float unClipRatio) {
    float minArea = 3;
    std::vector<TextBox> rsBoxes;
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(norfMapMat, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    for (const auto& contour : contours) {
        double perimeter = cv::arcLength(contour, true);
        cv::RotatedRect minRect = cv::minAreaRect(contour);
        float minSideLen = std::min(minRect.size.width, minRect.size.height);
        if (minSideLen < minArea) continue;
        cv::Mat mask = cv::Mat::zeros(fMapMat.size(), CV_8UC1);
        for (int contour_idx = 0; contour_idx < contours.size(); contour_idx++) {
            cv::drawContours(mask, contours, contour_idx, cv::Scalar(255), cv::FILLED);
        }

        cv::Scalar meanScore = cv::mean(fMapMat, mask);
        float score = static_cast<float>(meanScore[0]);

        if (score < boxScoreThresh) continue;

        double area = cv::contourArea(contour);
        float distance = unClipRatio * static_cast<float>(area) / static_cast<float>(perimeter);
        std::vector<cv::Point> clipBox = expandPolygon(contour, distance);

        for (auto &point : clipBox) {
            point.x = std::max(0, std::min(static_cast<int>(point.x / s.ratioWidth), s.srcWidth - 1));
            point.y = std::max(0, std::min(static_cast<int>(point.y / s.ratioHeight), s.srcHeight - 1));
        }

        rsBoxes.emplace_back(TextBox{clipBox, score});
    }
    return rsBoxes;
}

void drawTextBox(cv::Mat &boxImg, const std::vector<cv::Point> &box, int thickness) {
    for (size_t i = 0; i < box.size(); ++i) {
        cv::line(boxImg, box[i], box[(i + 1) % box.size()], cv::Scalar(0, 255, 0), thickness);
    }
}

// 辅助函数
template <class ForwardIterator>
inline static size_t argmax(ForwardIterator first, ForwardIterator last) {
    return std::distance(first, std::max_element(first, last));
}

// 文本识别相关函数
void processFrame(Mat &src, ncnn::Net &ocr_net, const std::vector<std::string> &keys, 
                 const float ocr_mean[], const float ocr_norm[], int dstHeight, std::string &outputText) {
    float scale = (float)dstHeight / (float)src.rows;
    int dstWidth = int((float)src.cols * scale);
    Mat resized_patch;
    cv::resize(src, resized_patch, cv::Size(dstWidth, dstHeight));
    ncnn::Mat input = ncnn::Mat::from_pixels(resized_patch.data, ncnn::Mat::PIXEL_BGR, 
                                            resized_patch.cols, resized_patch.rows);
    input.substract_mean_normalize(ocr_mean, ocr_norm);
    ncnn::Extractor ocr_extractor = ocr_net.create_extractor();
    ocr_extractor.input("input", input);
    ncnn::Mat ocr_output;
    ocr_extractor.extract("out", ocr_output);
    float *floatArray = (float *)ocr_output.data;
    std::vector<float> outputData(floatArray, floatArray + ocr_output.h * ocr_output.w);
    int keySize = keys.size();
    std::string strRes;
    std::vector<float> scores;
    int lastIndex = 0;
    int maxIndex;
    float maxValue;
    for (int i = 0; i < ocr_output.h; i++) {
        int maxIndex = 0;
        float maxValue = -1000.f;
        // Softmax 计算
        std::vector<float> exps(ocr_output.w);
        for (int j = 0; j < ocr_output.w; j++) {
          float expSingle = exp(outputData[i * ocr_output.w + j]);
          exps.at(j) = expSingle;
        }
        // 行总和
        float partition = accumulate(exps.begin(), exps.end(), 0.0);  
        // 找到最大值及其索引
        auto maxElementIt = std::max_element(exps.begin(), exps.end());
        maxIndex = std::distance(exps.begin(), maxElementIt);
        maxValue = *maxElementIt / partition;
        // 检测到有效字符
        if (maxIndex > 0 && maxIndex < keySize &&
            (!(i > 0 && maxIndex == lastIndex))) {
          scores.emplace_back(maxValue);
          // 将字符追加到结果字符串中
          strRes.append(keys[maxIndex - 1]);  
        }
    
        lastIndex = maxIndex;
    }
    outputText = strRes;
}

int main(int argc, char *argv[]) {
    if (argc != 7) {
        std::cerr << "Usage: " << argv[0] 
                  << " <image_path> <dbnet_param> <dbnet_bin> <ocr_param> <ocr_bin> <keys.txt>" << std::endl;
        return -1;
    }
    
    // 读取输入图像
    Mat src = imread(argv[1], IMREAD_COLOR);
    if (src.empty()) {
        std::cerr << "Failed to read image." << std::endl;
        return -1;
    }

    // 加载OCR字符表
    std::vector<std::string> keys;
    std::ifstream in(argv[6]);
    std::string line;
    if (in) {
        while (getline(in, line)) {
            keys.push_back(line);
        }
    } else {
        std::cerr << "The keys.txt file was not found." << std::endl;
        return -1;
    }
    if (keys.size() != 5531) {
        std::cerr << "Invalid keys.txt format." << std::endl;
        return -1;
    }

    // 初始化DBNet检测模型
    ncnn::Net dbnet;
    dbnet.opt.use_vulkan_compute = false;
    if (dbnet.load_param(argv[2]) != 0 || dbnet.load_model(argv[3]) != 0) {
        std::cerr << "Failed to load DBNet model." << std::endl;
        return -1;
    }

    // 初始化OCR识别模型
    ncnn::Net ocr_net;
    ocr_net.opt.use_vulkan_compute = false;
    if (ocr_net.load_param(argv[4]) != 0 || ocr_net.load_model(argv[5]) != 0) {
        std::cerr << "Failed to load OCR model." << std::endl;
        return -1;
    }

    // 执行文本检测
    ScaleParam scaleParam = getScaleParam(src, 736);
    Mat srcResize;
    resize(src, srcResize, cv::Size(scaleParam.dstWidth, scaleParam.dstHeight));

    ncnn::Mat dbnet_input = ncnn::Mat::from_pixels(srcResize.data, ncnn::Mat::PIXEL_BGR, 
                                                  srcResize.cols, srcResize.rows);
    dbnet_input.substract_mean_normalize(dbnet_meanValues, dbnet_normValues);

    ncnn::Extractor dbnet_extractor = dbnet.create_extractor();
    dbnet_extractor.input("input0", dbnet_input);
    ncnn::Mat dbnet_output;
    dbnet_extractor.extract("out1", dbnet_output);

    // 解析检测输出
    Mat fMapMat(srcResize.rows, srcResize.cols, CV_32FC1);
    memcpy(fMapMat.data, (float *)dbnet_output.data, 
           srcResize.rows * srcResize.cols * sizeof(float));
    Mat norfMapMat = fMapMat > 0.3f;

    std::vector<TextBox> text_boxes = findRsBoxes(fMapMat, norfMapMat, scaleParam, 0.5f, 4.0f);
    
    // 初始化计数器
    int save_count = 0;
    // 执行文本识别
    for (auto& box : text_boxes) {
        // 裁剪文本区域
        Rect rect = cv::boundingRect(box.boxPoint);
        Mat text_patch = src(rect).clone(); 

        // 保存裁剪后的原始区域
        std::string filename = "cropped_" + std::to_string(save_count) + ".png";
        // 保存为PNG格式
        imwrite(filename, text_patch);
        save_count++;

        // 执行OCR识别
        const float ocr_mean[3] = {127.5f, 127.5f, 127.5f};
        const float ocr_norm[3] = {1.0f / 127.5f, 1.0f / 127.5f, 1.0f / 127.5f};
        std::string textResult;
        processFrame(text_patch, ocr_net, keys, ocr_mean, ocr_norm, 32, textResult);

        box.text = textResult;
    }

    // 对文本框进行排序
    std::sort(text_boxes.begin(), text_boxes.end(), [](const TextBox &a, const TextBox &b) {
        cv::Rect rectA = cv::boundingRect(a.boxPoint);
        cv::Rect rectB = cv::boundingRect(b.boxPoint);
        
        int centerYA = rectA.y + rectA.height / 2;
        int centerYB = rectB.y + rectB.height / 2;

        if (centerYA != centerYB) {
            return centerYA < centerYB;
        }
        return (rectA.x + rectA.width / 2) < (rectB.x + rectB.width / 2);
    });
    // 可视化结果
    for (const auto& box : text_boxes) {
        drawTextBox(src, box.boxPoint, 1);
        std::cout << box.text << std::endl;
        // 由于opencv并不支持绘制中文，所以一下代码可以注释掉，感兴趣的可以自己尝试一下。
        // putText(src, box.text, box.boxPoint[0], 
        //         FONT_HERSHEY_SIMPLEX, 0.8, 
        //         Scalar(0, 0, 255), 1);
    }

    // 显示结果
    imshow("Text_result", src);
    waitKey(0);
    return 0;
}