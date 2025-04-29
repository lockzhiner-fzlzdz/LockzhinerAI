#include <lockzhiner_vision_module/edit/edit.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

std::vector<std::vector<cv::Point>> find_blobs(
    const cv::Mat &image,
    const cv::Scalar &lower_bound,
    const cv::Scalar &upper_bound,
    int min_area = 100,
    int kernel_size = 5)
{

    // 转换为 HSV 颜色空间
    cv::Mat hsv_image;
    cv::cvtColor(image, hsv_image, cv::COLOR_BGR2HSV);

    // 创建二值掩码
    cv::Mat mask;
    cv::inRange(hsv_image, lower_bound, upper_bound, mask);

    // 形态学操作：清除噪声
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernel_size, kernel_size));
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);

    // 查找轮廓
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // 筛选符合条件的色块
    std::vector<std::vector<cv::Point>> filtered_contours;
    for (const auto &contour : contours)
    {
        cv::Rect bounding_rect = cv::boundingRect(contour);
        if (bounding_rect.area() >= min_area)
        {
            filtered_contours.push_back(contour);
        }
    }
    return filtered_contours;
}

int main()
{
    lockzhiner_vision_module::edit::Edit edit;
    if (!edit.StartAndAcceptConnection())
    {
        std::cerr << "Error: Failed to start and accept connection." << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Device connected successfully." << std::endl;

    cv::VideoCapture cap;
    int width = 640;  // 设置摄像头分辨率宽度
    int height = 480; // 设置摄像头分辨率高度
    cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);

    // 打开摄像头设备
    cap.open(0); // 参数 0 表示默认摄像头设备
    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open camera." << std::endl;
        return EXIT_FAILURE;
    }
    while (true)
    {
        cv::Mat image; // 存储每一帧图像
        cap >> image;  // 获取新的一帧

        // 定义颜色阈值（例如红色）
        cv::Scalar lower_red(170, 100, 100); // 红色下界
        cv::Scalar upper_red(179, 255, 255); // 红色上界

        // 调用 find_blobs 函数
        int min_area = 100;  // 最小面积阈值
        int kernel_size = 1; // 形态学操作核大小
        std::vector<std::vector<cv::Point>> blobs = find_blobs(image, lower_red, upper_red, min_area, kernel_size);

        // 绘制和打印检测到的色块
        for (const auto &contour : blobs)
        {
            // 计算外接矩形框
            cv::Rect bounding_rect = cv::boundingRect(contour);

            // 绘制矩形框
            cv::rectangle(image, bounding_rect, cv::Scalar(0, 255, 0), 2);

            // 计算中心点
            cv::Moments moments = cv::moments(contour);
            int cx = moments.m10 / moments.m00;
            int cy = moments.m01 / moments.m00;

            // 绘制中心点
            cv::circle(image, cv::Point(cx, cy), 5, cv::Scalar(0, 0, 255), -1);

            // 打印信息
            std::cout << "Blob detected at (" << cx << ", " << cy << ") with area " << bounding_rect.area() << std::endl;
        }
        edit.Print(image);
    }
    return 0;
}