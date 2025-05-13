#include <lockzhiner_vision_module/edit/edit.h>
#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char *argv[])
{
  // 检查命令行参数数量是否正确
  if (argc != 3)
  {
    std::cerr << "Usage: " << argv[0] << " <image1_path> <image2_path>" << std::endl;
    return -1;
  }

  // 从命令行参数中读取图像路径
  std::string image1Path = argv[1];
  std::string image2Path = argv[2];

  // 加载图像
  cv::Mat image1 = cv::imread(image1Path);
  cv::Mat image2 = cv::imread(image2Path);

  // 检查图像是否加载成功
  if (image1.empty() || image2.empty())
  {
    std::cerr << "Error: Could not load images!" << std::endl;
    return -1;
  }

  // 获取图像尺寸
  int width = image1.cols;
  int height = image1.rows;
  std::cout << "Image size: " << width << "x" << height << std::endl;

  // 从(10,10)开始，宽40，高40，设置为蓝色
  cv::Rect roi(10, 10, 40, 40);
  image1(roi).setTo(cv::Scalar(255, 0, 0));

  // 图像取反
  cv::Mat invertedImage;
  cv::bitwise_not(image1, invertedImage);

  // 图像差分
  cv::Mat diffImage;
  cv::absdiff(image1, image2, diffImage);

  // 显示结果
  cv::imshow("Original Image", image1);
  cv::imshow("Inverted Image", invertedImage);
  cv::imshow("Difference Image", diffImage);

  cv::waitKey(0);
  return 0;
}