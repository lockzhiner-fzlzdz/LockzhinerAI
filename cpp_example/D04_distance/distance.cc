#include <lockzhiner_vision_module/vision/deep_learning/detection/paddle_det.h>
#include <lockzhiner_vision_module/vision/utils/visualize.h>
#include <lockzhiner_vision_module/edit/edit.h>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <opencv2/opencv.hpp>

const float REAL_OBJECT_SIZE = 0.02f;
const float FOCAL_LENGTH = 800.0f;

float CalculateDistance(float pixel_size, float real_size, float focal_length)
{
    return (real_size * focal_length) / pixel_size;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: Test-PaddleDet model_path" << std::endl;
        return 1;
    }

    // 初始化模型和设备连接
    lockzhiner_vision_module::vision::PaddleDet model;
    if (!model.Initialize(argv[1]))
    {
        std::cout << "Failed to initialize model." << std::endl;
        return 1;
    }

    lockzhiner_vision_module::edit::Edit edit;
    if (!edit.StartAndAcceptConnection())
    {
        std::cerr << "Error: Failed to start and accept connection." << std::endl;
        return EXIT_FAILURE;
    }

    cv::VideoCapture cap;
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    cap.open(0);

    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open camera." << std::endl;
        return 1;
    }

    cv::Mat input_mat;
    while (true)
    {
        cap >> input_mat;
        if (input_mat.empty())
        {
            std::cerr << "Warning: Captured an empty frame." << std::endl;
            continue;
        }

        // 预测并可视化结果
        auto start_time = std::chrono::high_resolution_clock::now();
        auto results = model.Predict(input_mat);
        auto end_time = std::chrono::high_resolution_clock::now();
        auto time_span = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        std::cout << "Inference time: " << time_span.count() << " ms" << std::endl;

        cv::Mat output_image;
        lockzhiner_vision_module::vision::Visualize(input_mat, output_image, results);

        // 在每个检测框上绘制距离信息
        for (size_t i = 0; i < results.size(); ++i)
        {
            int width = results[i].box.width;
            int height = results[i].box.height;
            float pixel_size = (width + height) / 2.0f;
            float distance = CalculateDistance(pixel_size, REAL_OBJECT_SIZE, FOCAL_LENGTH);

            // 格式化距离文本（保留两位小数）
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << distance << " m";
            std::string distance_text = ss.str();

            // 在检测框左上方绘制距离
            cv::Rect box = results[i].box;
            cv::putText(
                output_image,
                distance_text,
                cv::Point(box.x, box.y - 25), // 在框上方5像素处显示
                cv::FONT_HERSHEY_SIMPLEX,
                0.5,                   // 字体大小
                cv::Scalar(0, 255, 0), // 绿色文本
                2                      // 线宽
            );
        }

        // 显示结果到外部设备
        edit.Print(output_image);
    }

    cap.release();
    return 0;
}