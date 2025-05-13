#include <lockzhiner_vision_module/edit/edit.h>
#include <iostream>
#include <opencv2/opencv.hpp>

int main()
{
  // 初始化 edit 模块
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

  // 打开摄像头设备
  cap.open(0); // 参数 0 表示默认摄像头设备
  if (!cap.isOpened())
  {
    std::cerr << "Error: Could not open camera." << std::endl;
    return EXIT_FAILURE;
  }

  // 主循环：读取摄像头帧并传递给 edit 模块
  while (true)
  {
    cv::Mat frame; // 存储每一帧图像
    cap >> frame;  // 获取新的一帧

    // 检查是否成功读取帧
    if (frame.empty())
    {
      std::cerr << "Warning: Couldn't read a frame from the camera."
                << std::endl;
      continue;
    }

    // 使用 edit 模块处理帧
    edit.Print(frame);
  }

  // 释放摄像头资源
  cap.release();
  return 0;
}