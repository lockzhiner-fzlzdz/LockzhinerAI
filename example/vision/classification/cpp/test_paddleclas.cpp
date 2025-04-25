#include <lockzhiner_vision_module/vision/deep_learning/classification/paddle_clas.h>
#include <lockzhiner_vision_module/vision/utils/visualize.h>

#include <chrono>
#include <cstdlib>  // for std::rand and std::srand
#include <ctime>    // for std::time
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std::chrono;

lockzhiner_vision_module::vision::PaddleClas model;

int TestFPS() {
  // 创建一个空的3通道Mat，宽640，高480，类型为CV_8UC3
  std::srand(static_cast<unsigned int>(std::time(nullptr)));
  cv::Mat input_mat(480, 640, CV_8UC3);
  for (int i = 0; i < input_mat.rows; ++i) {
    for (int j = 0; j < input_mat.cols; ++j) {
      input_mat.at<cv::Vec3b>(i, j)[0] = std::rand() % 256;  // B通道
      input_mat.at<cv::Vec3b>(i, j)[1] = std::rand() % 256;  // G通道
      input_mat.at<cv::Vec3b>(i, j)[2] = std::rand() % 256;  // R通道
    }
  }

  constexpr int run_index = 1000;
  int time_ms = 0;
  for (int i = 0; i < run_index; i++) {
    high_resolution_clock::time_point start_time = high_resolution_clock::now();
    auto result = model.Predict(input_mat);
    high_resolution_clock::time_point end_time = high_resolution_clock::now();
    auto time_span = duration_cast<milliseconds>(end_time - start_time);
    time_ms += time_span.count();
  }
  std::cout << "Frames per second: " << 1000.0 / time_ms * run_index
            << std::endl;
  return 0;
}

int TestCapture() {
  cv::VideoCapture cap;  // 设置摄像头长宽
  cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
  cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
  // 打开摄像头
  cap.open(0);

  cv::Mat input_mat;
  while (true) {
    int read_index = 0;
    int time_ms = 0;
    for (int i = 0; i < 30; i++) {
      high_resolution_clock::time_point start_time =
          high_resolution_clock::now();
      cap >> input_mat;
      // 判断获取的图片是否为空
      if (input_mat.empty()) {
        continue;
      }
      auto result = model.Predict(input_mat);
      high_resolution_clock::time_point end_time = high_resolution_clock::now();
      auto time_span = duration_cast<milliseconds>(end_time - start_time);
      time_ms += time_span.count();
      read_index += 1;
      std::cout << "score is " << result.score << ";label_id is "
                << result.label_id << std::endl;
    }
    std::cout << "Frames per second: " << 1000.0 / time_ms * read_index
              << std::endl;
  }
  return 0;
}

int TestImage(const std::string& image_path) {
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

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: Test-PaddleClas model_path <Capture|FPS|image_path>"
              << std::endl;
    return 1;
  }

  if (!model.Initialize(argv[1])) {
    std::cout << "Failed to initialize model." << std::endl;
    return 1;
  }

  std::string argument(argv[2]);
  if (argument == "Capture") {
    return TestCapture();
  } else if (argument == "FPS") {
    return TestFPS();
  } else {
    return TestImage(argument);
  }
  return 0;
}