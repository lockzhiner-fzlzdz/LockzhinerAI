#include <unistd.h> 

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <ncnn/net.h>
#include <fstream>
#include <numeric>

#include <chrono>
#include <cstdlib> 
#include <ctime> 
#include <opencv2/opencv.hpp>

#include <lockzhiner_vision_module/edit/edit.h>


using namespace cv;
using namespace std::chrono;
template <class ForwardIterator>
inline static size_t argmax(ForwardIterator first, ForwardIterator last) {
  return std::distance(first, std::max_element(first, last));
}

// 后续处理函数
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
        << "Usage: ./ncnn_ocr <image_path> <param_path> <bin_path> <key_paths>"
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

  std::string argument(argv[1]);
  // Default: BGR
  Mat src = imread(argv[1]);  
  if (src.empty()) {
      std::cerr << "Error opening image file" << std::endl;
      return -1;
  }
  processFrame(src, net, keys, meanValues, normValues, dstHeight);
  return 0;
}
