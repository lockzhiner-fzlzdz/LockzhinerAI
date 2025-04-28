# OCR 字符识别
本章节基于 Lockzhiner Vision Module 的chineseocr_lite模型 ,实现了一个OCR字符识别系统。
## 1. 基本知识讲解
### 1.1 OCR 字符识别简介
OCR（光学字符识别）是指通过电子设备读取并转换纸质文档或图像中的文字为可编辑和处理的数字文本的技术。它涉及图像预处理、字符分割、特征提取、字符识别及后处理等步骤，以实现高准确度的文字转换。OCR技术极大提升了信息数字化的效率，广泛应用于数字化图书馆、自动化数据录入、车牌识别系统及辅助阅读工具等领域，是现代办公与生活中不可或缺的一部分。
### 1.2 OCR 字符识别常用方法
- 模板匹配：通过与预定义字符模板比较来识别字符，适用于固定字体和字号。
- 特征提取：从字符中提取关键特征（如线条、端点）并使用分类器识别，适应字体变化。
- 神经网络：利用卷积神经网络自动学习字符特征，特别适合复杂背景和多变字体，提供高准确率。
这些方法各有优势，选择取决于具体应用需求和文档特性。随着技术发展，基于神经网络的方法因其高性能而得到广泛应用。

## 2. C++ API 文档
### 2.1 Net类
#### 2.1.1 头文件
```cpp
#include </LockzhinerVisionModuleWorkSpace/LockzhinerVisionModuleLibrary/third_party/ncnn-20240820-lockzhiner-vision-module/include/ncnn/net.h>
```
- 作用：用于声明Net类，使得Net类可以在当前文件中使用。
#### 2.1.2 构造类函数
```cpp
ncnn::Net net;
```
- 作用：创建一个Net类型的对象实例，用于实现OCR模型的加载，并基于加载的模型实现字符识别任务。
- 参数说明：
    - 无
- 返回值：
    - 无
### 2.2 Extractor类
#### 2.2.1 头文件
```cpp
#include </LockzhinerVisionModuleWorkSpace/LockzhinerVisionModuleLibrary/third_party/ncnn-20240820-lockzhiner-vision-module/include/ncnn/net.h>
```
- 作用：用于声明Extractor类，使得Extractor类可以在当前文件中使用。
#### 2.2.2 构造类函数
```cpp
ncnn::Extractor extractor = net.create_extractor();
```
- 作用：从已经加载了神经网络模型的 net 中创建一个 Extractor 实例，用于执行具体的推理任务。
- 参数说明：
    - 无
- 返回值：
    - 无
### 2.3 OCR 字符识别
```cpp
void processFrame(Mat &src, ncnn::Net &net, const std::vector<std::string> &keys, const float meanValues[], const float normValues[], const int dstHeight)
```
- 参数说明：
    - src：输入的图像帧。
    - net：一个已经加载了特定神经网络模型的对象。
    - keys：包含了所有可能被模型识别的字符集。
    - meanVaues：图像每个通道的平均值。
    - normValues：归一化图像像素值的比例因子。
- 返回值：
    - 无

## 3. OCR 字符识别代码解析
### 3.1 流程图

<img src="./images/1.png" width="500" height="1200">

### 3.2 核心代码解析
- 加载模型参数和权重
```cpp
net.load_param(argv[2]);
net.load_model(argv[3]);
```
- 读取字符集文件
```cpp
std::ifstream in(argv[4]);
std::string line;
if (in) {
while (getline(in, line)) {  // line中不包括每行的换行符
    keys.push_back(line);
}
} else {
printf("The keys.txt file was not found\n");
return false;
}
if (keys.size() != 5531) {
fprintf(stderr, "missing keys\n");
return false;
}
```
- 对输入图像进行识别
```cpp
for (int i = 0; i < out.h; i++) {
    int maxIndex = 0;
    float maxValue = -1000.f;

    // Softmax 计算
    std::vector<float> exps(out.w);
    for (int j = 0; j < out.w; j++) {
      float expSingle = exp(outputData[i * out.w + j]);
      exps.at(j) = expSingle;
    }
    float partition = accumulate(exps.begin(), exps.end(), 0.0);  // 行总和

    // 找到最大值及其索引
    auto maxElementIt = std::max_element(exps.begin(), exps.end());
    maxIndex = std::distance(exps.begin(), maxElementIt);
    maxValue = *maxElementIt / partition;

    // 检测到有效字符
    if (maxIndex > 0 && maxIndex < keySize &&
        (!(i > 0 && maxIndex == lastIndex))) {
      scores.emplace_back(maxValue);
      strRes.append(keys[maxIndex - 1]);  // 将字符追加到结果字符串中
    }

    lastIndex = maxIndex;
  }
```

### 3.3 完整代码实现
```cpp
#include <unistd.h>  // sleep()
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include </LockzhinerVisionModuleWorkSpace/LockzhinerVisionModuleLibrary/third_party/ncnn-20240820-lockzhiner-vision-module/include/ncnn/net.h>
#include <fstream>
#include <numeric>
#include <chrono>
#include <cstdlib>  // for std::rand and std::srand
#include <ctime>    // for std::time
#include <opencv2/opencv.hpp>
#include <lockzhiner_vision_module/edit/edit.h>

using namespace cv;
using namespace std::chrono;
template <class ForwardIterator>
inline static size_t argmax(ForwardIterator first, ForwardIterator last) {
  return std::distance(first, std::max_element(first, last));
}

// 图像推理函数
void processFrame(Mat &src, ncnn::Net &net, const std::vector<std::string> &keys, const float meanValues[], const float normValues[], const int dstHeight) {
  float scale = (float)dstHeight / (float)src.rows;
  int dstWidth = int((float)src.cols * scale);
  std::cout << "resize" << std::endl;
  cv::Mat srcResize;
  resize(src, srcResize, cv::Size(dstWidth, dstHeight));
  std::cout << "resize success" << std::endl;
  ncnn::Mat input = ncnn::Mat::from_pixels(srcResize.data, ncnn::Mat::PIXEL_RGB,
                                          srcResize.cols, srcResize.rows);
  std::cout << "input success" << std::endl;
  input.substract_mean_normalize(meanValues, normValues);

  ncnn::Extractor extractor = net.create_extractor();
  // net.num_threads  = 6;
  extractor.input("input", input);
  std::cout << "extract success" << std::endl;
  ncnn::Mat out;
  extractor.extract("out", out);

  float *floatArray = (float *)out.data;
  std::vector<float> outputData(floatArray, floatArray + out.h * out.w);

  int keySize = keys.size();
  std::string strRes;
  std::vector<float> scores;
  int lastIndex = 0;
  int maxIndex;
  float maxValue;
  std::cout << "开始检测" << std::endl;
  high_resolution_clock::time_point start_time =
          high_resolution_clock::now();
  for (int i = 0; i < out.h; i++) {
    int maxIndex = 0;
    float maxValue = -1000.f;

    // Softmax 计算
    std::vector<float> exps(out.w);
    for (int j = 0; j < out.w; j++) {
      float expSingle = exp(outputData[i * out.w + j]);
      exps.at(j) = expSingle;
    }
    // 行总和
    float partition = accumulate(exps.begin(), exps.end(), 0.0);  
    // 找到最大值及其索引
    auto maxElementIt = std::max_element(exps.begin(), exps.end());
    maxIndex = std::distance(exps.begin(), maxElementIt);
    maxValue = *maxElementIt / partition;

    // 检测到有效字符
    if (maxIndex > 0 && maxIndex < keySize &&
        (!(i > 0 && maxIndex == lastIndex))) {
      scores.emplace_back(maxValue);
      // 将字符追加到结果字符串中
      strRes.append(keys[maxIndex - 1]);  
    }

    lastIndex = maxIndex;
  }
  high_resolution_clock::time_point end_time = high_resolution_clock::now();
  auto time_span = duration_cast<milliseconds>(end_time - start_time);
  // 所有检测完成后，一次性输出结果
  std::cout << "检测完成，最终结果：" << std::endl;
  std::cout << "识别的文本: " << strRes << std::endl;
  std::cout << "单张图片推理时间(ms): " << time_span.count() << std::endl;
  std::cout << std::endl;
  
} 

int main(int argc, char *argv[]) {
  if (argc != 5) {
    std::cerr
        << "Usage: ./ncnn_ocr <image_path|Capture> <param_path> <bin_path> <key_paths>"
        << std::endl;
  }
  const float meanValues[3] = {127.5, 127.5, 127.5};
  const float normValues[3] = {1.0 / 127.5, 1.0 / 127.5, 1.0 / 127.5};
  const int dstHeight = 32;
  ncnn::Net net;
  std::vector<std::string> keys;
  // Mat src = imread(argv[1]);  // default : BGR
  // Step 2: 加载 .param 和 .bin 文件
  net.load_param(argv[2]);
  net.load_model(argv[3]);
  std::cout << "load model success" << std::endl;
  std::ifstream in(argv[4]);
  std::string line;
  if (in) {
    // line中不包括每行的换行符
    while (getline(in, line)) {  
      keys.push_back(line);
    }
  } else {
    printf("The keys.txt file was not found\n");
    return false;
  }
  if (keys.size() != 5531) {
    fprintf(stderr, "missing keys\n");
    return false;
  }
  printf("total keys size(%lu)\n", keys.size());

  // 判断是否调用摄像头识别
  std::string argument(argv[1]);
  if (argument == "Capture") {
    // 初始化 edit 模块
    lockzhiner_vision_module::edit::Edit edit;
    if (!edit.StartAndAcceptConnection()) {
      std::cerr << "Error: Failed to start and accept connection." << std::endl;
      return EXIT_FAILURE;
    }

    std::cout << "Device connected successfully." << std::endl;
    cv::VideoCapture cap;  // 设置摄像头长宽
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    // 打开摄像头
    cap.open(0);

    if (!cap.isOpened()) {
      std::cerr << "Error: Could not open camera." << std::endl;
      return EXIT_FAILURE;
    }
    Mat input_image;
    while (true) {
        // Get a new frame from camera
        cap >> input_image; 
        if (input_image.empty()) {
          continue;
        }
        processFrame(input_image, net, keys, meanValues, normValues, dstHeight);
        // 使用 edit 模块处理帧
        edit.Print(input_image);
    }
    // 释放摄像头资源
    cap.release();
} else {
    // Default: BGR
    Mat src = imread(argv[1]);  
    if (src.empty()) {
        std::cerr << "Error opening image file" << std::endl;
        return -1;
    }
    processFrame(src, net, keys, meanValues, normValues, dstHeight);
}
return 0;
}
```

## 4. 编译调试

## 5. 执行结果
### 5.1 运行
```shell
chmod 777 Test-Capture
# 对图像进行识别
./Test-Capture image_path crnn_lite_op.param crnn_lite_op.bin keys.txt
# 调用摄像头识别
./Test-Capture Capture crnn_lite_op.param crnn_lite_op.bin keys.txt
```
### 5.2 识别效果图
- 图像OCR识别
