#include <lockzhiner_vision_module/edit/edit.h>
#include <iostream>
#include <opencv2/opencv.hpp>

int main()
{
  lockzhiner_vision_module::edit::Edit edit;
  if (!edit.StartAndAcceptConnection())
  {
    std::cerr << "Error: Failed to start and accept connection." << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "Device connected successfully." << std::endl;
  // 初始化摄像头
  cv::VideoCapture cap;
  int width = 640;  // 设置摄像头分辨率宽度
  int height = 480; // 设置摄像头分辨率高度
  cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
  cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);

  cap.open(0); // 参数 0 表示默认摄像头设备
  if (!cap.isOpened())
  {
    std::cerr << "Error: Could not open camera." << std::endl;
    return EXIT_FAILURE;
  }

  while (true)
  {
    // 读取输入图像
    cv::Mat src;
    cap >> src; // 获取新的一帧
    if (src.empty())
    {
      std::cerr << "Warning: Couldn't read a frame from the camera."
                << std::endl;
      continue;
    }

    // 转换为灰度图像
    cv::Mat gray;
    cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);

    // 高斯模糊降噪
    cv::GaussianBlur(gray, gray, cv::Size(5, 5), 0);

    // 边缘检测（Canny）
    cv::Mat edges;
    cv::Canny(gray, edges, 50, 150);

    // 查找轮廓
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(edges, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    // 多边形拟合
    cv::Mat polygonImage = src.clone();
    for (size_t i = 0; i < contours.size(); i++)
    {
      std::vector<cv::Point> approx;
      cv::approxPolyDP(contours[i], approx,
                       cv::arcLength(contours[i], true) * 0.02, true);
      cv::drawContours(polygonImage,
                       std::vector<std::vector<cv::Point>>{approx}, -1,
                       cv::Scalar(0, 0, 255), 2);
    }

    edit.Print(polygonImage);
  }

  cap.release();
  return 0;
}