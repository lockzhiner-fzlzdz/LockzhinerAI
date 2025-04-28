#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{
    // 读取图像
    cv::Mat image = cv::imread("example.jpg");
    if (image.empty())
    {
        std::cerr << "Error: Could not open image!" << std::endl;
        return -1;
    }

    // 定义 ROI 并提取
    cv::Rect roiRect(50, 50, 200, 200);
    cv::Mat roi = image(roiRect);

    // 转换为灰度图
    cv::Mat grayRoi;
    cv::cvtColor(roi, grayRoi, cv::COLOR_BGR2GRAY);

    // 计算均值和标准差
    cv::Scalar mean, stddev;
    cv::meanStdDev(grayRoi, mean, stddev);

    // 计算最小值和最大值
    double minVal, maxVal;
    cv::Point minLoc, maxLoc;
    cv::minMaxLoc(grayRoi, &minVal, &maxVal, &minLoc, &maxLoc);

    // 输出结果
    std::cout << "Mean: " << mean[0] << std::endl;
    std::cout << "Standard Deviation: " << stddev[0] << std::endl;
    std::cout << "Min Value: " << minVal << " at " << minLoc << std::endl;
    std::cout << "Max Value: " << maxVal << " at " << maxLoc << std::endl;

    return 0;
}