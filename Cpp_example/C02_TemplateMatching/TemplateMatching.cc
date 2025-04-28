#include <lockzhiner_vision_module/edit/edit.h>
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// 模板匹配函数
bool performTemplateMatching(const Mat &inputImage, const Mat &templateImage, double threshold, Mat &outputImage, bool isGrayscale = false)
{
    // 确保模板图像比输入图像小
    if (templateImage.rows > inputImage.rows || templateImage.cols > inputImage.cols)
    {
        cout << "模板图像不能大于输入图像！" << endl;
        return false;
    }

    // 创建用于匹配的图像副本
    Mat templ = templateImage.clone();
    Mat img = inputImage.clone();

    // 如果选择灰度处理，则将输入图像和模板图像转换为灰度
    if (isGrayscale)
    {
        if (img.channels() == 3)
        {
            cvtColor(img, img, COLOR_BGR2GRAY);
        }
        if (templ.channels() == 3)
        {
            cvtColor(templ, templ, COLOR_BGR2GRAY);
        }
    }

    // 打印调试信息
    cout << "输入图像尺寸: " << img.size() << ", 通道数: " << img.channels() << endl;
    cout << "模板图像尺寸: " << templ.size() << ", 通道数: " << templ.channels() << endl;

    // 创建结果矩阵，用于存储匹配结果
    int resultRows = img.rows - templ.rows + 1;
    int resultCols = img.cols - templ.cols + 1;
    if (resultRows <= 0 || resultCols <= 0)
    {
        cout << "结果矩阵尺寸无效！请检查输入图像和模板图像的尺寸。" << endl;
        return false;
    }
    Mat result(resultRows, resultCols, CV_32FC1);

    // 使用归一化互相关（NCC）方法进行模板匹配
    double start = static_cast<double>(getTickCount());
    matchTemplate(img, templ, result, TM_CCOEFF_NORMED);
    double end = static_cast<double>(getTickCount());
    double elapsedTime = (end - start) / getTickFrequency();
    cout << "matchTemplate 运行时间: " << elapsedTime << " 秒" << endl;

    // 找到匹配结果中的最大值及其位置
    double minVal, maxVal;
    Point minLoc, maxLoc;
    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

    // 输出相似度阈值判断
    if (maxVal >= threshold)
    {
        cout << "匹配成功！最大相似度: " << maxVal << endl;

        // 绘制矩形框标记匹配区域
        rectangle(outputImage, maxLoc, Point(maxLoc.x + templ.cols, maxLoc.y + templ.rows), Scalar(0, 255, 0), 2);
        return true;
    }
    else
    {
        cout << "匹配失败！最大相似度: " << maxVal << endl;
        return false;
    }
}

int main(int argc, char **argv)
{
    // 检查命令行参数数量是否正确
    if (argc != 2)
    {
        cout << "用法: " << argv[0] << " <模板图像路径>" << endl;
        return -1;
    }

    // 声明并初始化变量
    string templateImagePath = argv[1]; // 模板图像路径

    lockzhiner_vision_module::edit::Edit edit;
    if (!edit.StartAndAcceptConnection())
    {
        std::cerr << "Error: Failed to start and accept connection." << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Device connected successfully." << std::endl;

    // 初始化摄像头
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

    // 加载模板图像
    Mat templ = imread(templateImagePath, IMREAD_COLOR); // 默认加载彩色模板
    if (templ.empty())
    {
        std::cerr << "Error: Could not load template image." << std::endl;
        return EXIT_FAILURE;
    }

    while (true)
    {
        // 读取输入图像
        cv::Mat img;
        cap >> img;
        if (img.empty())
        {
            std::cerr << "Error: Captured frame is empty." << std::endl;
            break;
        }

        // 调用模板匹配函数
        double similarityThreshold = 0.7; // 相似度阈值
        bool useGrayscale = false;        // 是否使用灰度处理
        bool matchSuccess = performTemplateMatching(img, templ, similarityThreshold, img, useGrayscale);

        // 显示结果
        imshow("Template Matching Result", img);
        if (waitKey(1) == 27)
        { // 按 ESC 键退出
            break;
        }

        // 输出图像
        edit.Print(img);
    }

    return 0;
}