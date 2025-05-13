#include <lockzhiner_vision_module/vision/code/code_detector.h>
#include <lockzhiner_vision_module/vision/utils/visualize.h>
#include <lockzhiner_vision_module/edit/edit.h>

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std::chrono;

lockzhiner_vision_module::vision::QRCodeDetector model;

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

  cv::VideoCapture cap;
  // 设置摄像头获取帧的宽高
  cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
  cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
  cap.open(0);

  if (!cap.isOpened())
  {
    std::cerr << "Error: Could not open camera." << std::endl;
    return EXIT_FAILURE;
  }

  cv::Mat input_mat;

  while (true)
  {
    int read_index = 0;
    int time_ms = 0;

    for (int i = 0; i < 30; i++)
    {
      high_resolution_clock::time_point start_time = high_resolution_clock::now();
      cap >> input_mat;
      if (input_mat.empty())
      {
        continue;
      }
      // 使用 model 对象的 Predict 方法对输入图像进行预测，获取二维码检测结果
      auto results = model.Predict(input_mat);

      high_resolution_clock::time_point end_time = high_resolution_clock::now();
      auto time_span = duration_cast<milliseconds>(end_time - start_time);
      time_ms += time_span.count();
      read_index += 1;

      cv::Mat output_image;
      // 调用 Visualize 函数对原始图像和检测结果进行可视化处理，并将结果存储在 output_image 中
      lockzhiner_vision_module::vision::Visualize(input_mat, output_image, results);
      edit.Print(output_image);
    }

    std::cout << "Frames per second: " << 1000.0 / time_ms * read_index << std::endl;
  }

  cap.release();
  return 0;
}