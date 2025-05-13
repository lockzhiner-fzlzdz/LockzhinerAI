#include <lockzhiner_vision_module/vision/deep_learning/classification/paddle_clas.h>
#include <lockzhiner_vision_module/vision/utils/visualize.h>
#include <lockzhiner_vision_module/edit/edit.h>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std::chrono;

lockzhiner_vision_module::vision::PaddleClas model;

int TestCapture()
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
  // 设置摄像头长宽
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
      // 获取当前时间点作为开始时间
      high_resolution_clock::time_point start_time =
          high_resolution_clock::now();
      cap >> input_mat;
      if (input_mat.empty())
      {
        continue;
      }
      // 使用 model 对象的 Predict 方法对输入图像进行预测
      auto result = model.Predict(input_mat);
      // 获取当前时间点作为结束时间
      high_resolution_clock::time_point end_time = high_resolution_clock::now();
      auto time_span = duration_cast<milliseconds>(end_time - start_time);
      time_ms += time_span.count();
      read_index += 1;

      std::cout << "score is " << result.score << ";label_id is "
                << result.label_id << std::endl;

      cv::Mat output_image;
      lockzhiner_vision_module::vision::Visualize(input_mat, output_image,
                                                  result);
      // 使用 edit 模块处理帧
      edit.Print(output_image);
    }
    std::cout << "Frames per second: " << 1000.0 / time_ms * read_index
              << std::endl;
  }
  cap.release();
  return 0;
}

int TestImage(const std::string &image_path)
{
  cv::Mat input_image = cv::imread(image_path);
  auto result = model.Predict(input_image);
  std::cout << "score is " << result.score << ";label_id is " << result.label_id
            << std::endl;
  cv::Mat output_image;
  lockzhiner_vision_module::vision::Visualize(input_image, output_image,
                                              result);
  cv::imwrite("cls_result.png", output_image);
  return 0;
}

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    std::cerr << "Usage: Test-PaddleClas model_path <Capture|image_path>"
              << std::endl;
    return 1;
  }

  if (!model.Initialize(argv[1]))
  {
    std::cout << "Failed to initialize model." << std::endl;
    return 1;
  }

  std::string argument(argv[2]);
  if (argument == "Capture")
  {
    return TestCapture();
  }
  else
  {
    return TestImage(argument);
  }
  return 0;
}