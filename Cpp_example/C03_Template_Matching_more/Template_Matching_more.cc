#include <lockzhiner_vision_module/edit/edit.h>

#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

using namespace cv;
using namespace std;

// 多模板匹配函数（支持彩色或灰度图像，仅绘制匹配度最高的框）
void multiTemplateMatching(const Mat &img, const vector<string> &templatePaths,
                           double threshold = 0.7, bool isGrayscale = false)
{
  // 初始化最高匹配度和对应的模板路径、位置
  double bestMatchValue = 0.0;
  string bestMatchTemplatePath = "";
  Rect bestMatchRect;

  // 遍历每个模板路径
  for (const auto &templatePath : templatePaths)
  {
    // 加载模板图像（根据 isGrayscale 决定是灰度还是彩色）
    Mat templ = imread(templatePath, isGrayscale ? IMREAD_GRAYSCALE : IMREAD_COLOR);
    if (templ.empty())
    {
      cerr << "无法加载模板图像: " << templatePath << endl;
      continue;
    }

    // 如果输入图像是灰度图像，则将彩色图像转换为灰度
    Mat inputImage = img.clone();
    if (isGrayscale && inputImage.channels() == 3)
    {
      cvtColor(inputImage, inputImage, COLOR_BGR2GRAY);
    }

    // 创建结果矩阵
    int result_cols = inputImage.cols - templ.cols + 1;
    int result_rows = inputImage.rows - templ.rows + 1;
    Mat result(result_rows, result_cols, CV_32FC1);

    // 执行模板匹配
    matchTemplate(inputImage, templ, result, TM_CCOEFF_NORMED);

    // 查找最佳匹配位置
    double minVal, maxVal;
    Point minLoc, maxLoc;
    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

    // 如果当前模板的最大匹配度高于之前的记录，则更新最佳匹配信息
    if (maxVal > bestMatchValue && maxVal >= threshold)
    {
      bestMatchValue = maxVal;
      bestMatchTemplatePath = templatePath;
      bestMatchRect = Rect(maxLoc.x, maxLoc.y, templ.cols, templ.rows);
    }
  }

  // 如果找到匹配度高于阈值的最佳匹配，则绘制矩形框
  if (!bestMatchTemplatePath.empty())
  {
    rectangle(img, bestMatchRect, Scalar(0, 255, 0), 2); // 绿色矩形框
    cout << "匹配到模板: " << bestMatchTemplatePath
         << ", 匹配度: " << bestMatchValue << endl;
  }
  else
  {
    cout << "未找到匹配度高于阈值的模板。" << endl;
  }
}

int main(int argc, char *argv[])
{
  lockzhiner_vision_module::edit::Edit edit;
  if (!edit.StartAndAcceptConnection())
  {
    std::cerr << "Error: Failed to start and accept connection." << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "Device connected successfully." << std::endl;

  cv::VideoCapture cap;
  int width = 320;  // 设置摄像头分辨率宽度
  int height = 240; // 设置摄像头分辨率高度
  cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
  cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);

  // 打开摄像头设备
  cap.open(0); // 参数 0 表示默认摄像头设备
  if (!cap.isOpened())
  {
    std::cerr << "Error: Could not open camera." << std::endl;
    return EXIT_FAILURE;
  }

  // 检查命令行参数是否提供了模板路径
  vector<string> templates;
  if (argc < 2)
  {
    cerr << "Usage: " << argv[0] << " template_path1 [template_path2 ...]" << endl;
    return EXIT_FAILURE;
  }

  // 从命令行读取模板路径
  for (int i = 1; i < argc; ++i)
  {
    templates.push_back(argv[i]);
  }

  while (true)
  {
    cv::Mat img; // 存储每一帧图像
    cap >> img;  // 获取新的一帧

    // 检查是否成功读取帧
    if (img.empty())
    {
      std::cerr << "Warning: Couldn't read a frame from the camera."
                << std::endl;
      continue;
    }

    // 执行多模板匹配（示例中仍使用彩色图像）
    multiTemplateMatching(img, templates, 0.7, true);

    // 显示结果
    edit.Print(img);
  }

  return 0;
}