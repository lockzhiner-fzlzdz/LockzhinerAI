#include <lockzhiner_vision_module/edit/edit.h>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

int main(int argc, char *argv[]) {
    // 设置默认摄像头分辨率
    int width = 640;
    int height = 480;

    // 如果命令行参数提供了宽度和高度，则使用它们
    if (argc == 3) {
        width = std::stoi(argv[1]);
        height = std::stoi(argv[2]);
    }

    // 初始化编辑模块并尝试连接设备
    lockzhiner_vision_module::edit::Edit edit;
    if (!edit.StartAndAcceptConnection()) {
        std::cerr << "Error: Failed to start and accept connection." << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Device connected successfully." << std::endl;

    cv::VideoCapture cap;
    cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
    cap.open(0);

    // 获取实际的帧尺寸
    double frameWidth = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    double frameHeight = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    std::cout << "Frame size: " << frameWidth << "x" << frameHeight << std::endl;

    // 定义ROI区域
    cv::Rect roi_rect(40, 300, 560, 180);

    while (true) {
        cv::Mat temp_mat;
        cap >> temp_mat; // 获取新的一帧
        if (temp_mat.empty()) {
            std::cerr << "Warning: Couldn't read a frame from the camera." << std::endl;
            continue;
        }

        // 提取ROI区域，并转换到HSV色彩空间
        cv::Mat roi_image = temp_mat(roi_rect);
        cv::cvtColor(roi_image, roi_image, cv::COLOR_BGR2HSV); // 注意原代码中是RGB2HSV，应改为BGR2HSV

        // 创建黑白掩膜
        cv::Scalar lower_black(0, 0, 0);
        cv::Scalar upper_black(180, 100, 60);
        cv::Mat mask;
        cv::inRange(roi_image, lower_black, upper_black, mask);

        // 应用掩膜
        cv::Mat res;
        cv::bitwise_and(roi_image, roi_image, res, mask);

        // 计算图像矩并找到质心
        cv::Moments m = cv::moments(mask, false);
        double cx = m.m10 / (m.m00 + 1e-6); // 防止除以零
        double cy = m.m01 / (m.m00 + 1e-6);

        // 在原始图像上绘制ROI边框和质心位置
        cv::rectangle(temp_mat, roi_rect, cv::Scalar(255, 0, 0), 2); // 绘制ROI边框
        cv::circle(temp_mat, cv::Point(static_cast<int>(cx + roi_rect.x), static_cast<int>(cy + roi_rect.y)), 5, cv::Scalar(0, 255, 0), -1); // 绿色圆点表示质心位置

        // 输出质心的X坐标
        std::cout << "Centroid X: " << cx + roi_rect.x << std::endl;

        // 将当前帧发送到编辑模块打印
        if (!edit.Print(temp_mat)) {
            std::cerr << "Error: Failed to print to edit module." << std::endl;
            break;
        }

    }

    return EXIT_SUCCESS;
}