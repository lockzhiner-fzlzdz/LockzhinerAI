#include <lockzhiner_vision_module/edit/edit.h>

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

int main(int argc, char *argv[]) {
  int width = 640;
  int height = 480;
  if (argc == 3) {
    width = atoi(argv[1]);
    height = atoi(argv[2]);
  }

  lockzhiner_vision_module::edit::Edit edit;
  // 打开服务器并阻塞接收客户端连接
  if (!edit.StartAndAcceptConnection()) {
    std::cout << "Failed to start and accept connection" << std::endl;
    return 1;
  }
  std::cout << "Succeed to connect device" << std::endl;

  cv::VideoCapture cap;
  // 设置摄像头长宽
  cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
  cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
  // 打开摄像头
  cap.open(0);

  const int w = cap.get(cv::CAP_PROP_FRAME_WIDTH);
  const int h = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
  cv::Mat temp_mat;
  while (true) {
    cap >> temp_mat;
    // 判断获取的图片是否为空
    if (temp_mat.empty()) {
      continue;
    }
    // 向客户端输出图片
    if (!edit.Print(temp_mat)) {
      std::cout << "Failed to print to edit" << std::endl;
      break;
    }
  }
  return 0;
}