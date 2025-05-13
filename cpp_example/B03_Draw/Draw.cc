#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{
    // 创建一个空白图像 (512x512，3通道)
    cv::Mat image = cv::Mat::zeros(512, 512, CV_8UC3);

    // 画线
    cv::line(image, cv::Point(10, 10), cv::Point(100, 100), cv::Scalar(255, 0, 0), 2);

    // 画矩形
    cv::rectangle(image, cv::Rect(50, 50, 100, 100), cv::Scalar(0, 255, 0), 2);

    // 画圆
    cv::circle(image, cv::Point(200, 200), 30, cv::Scalar(0, 0, 255), -1);

    // 画十字
    int cross_x = 300, cross_y = 300, cross_size = 10;
    cv::line(image, cv::Point(cross_x - cross_size, cross_y), cv::Point(cross_x + cross_size, cross_y), cv::Scalar(255, 255, 0), 2);
    cv::line(image, cv::Point(cross_x, cross_y - cross_size), cv::Point(cross_x, cross_y + cross_size), cv::Scalar(255, 255, 0), 2);

    // 写字
    cv::putText(image, "Hello, OpenCV!", cv::Point(10, 400), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);

    // 显示图像
    cv::imshow("Drawing Example", image);
    cv::waitKey(0);

    return 0;
}