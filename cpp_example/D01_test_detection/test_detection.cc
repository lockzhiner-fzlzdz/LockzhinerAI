#include <lockzhiner_vision_module/vision/deep_learning/detection/paddle_det.h>
#include <lockzhiner_vision_module/vision/utils/visualize.h>
#include <lockzhiner_vision_module/edit/edit.h>
#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std::chrono;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: Test-PaddleDet model_path" << std::endl;
        return 1;
    }

    // 初始化模型
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
    std::cout << "Device connected successfully." << std::endl;
    // 打开摄像头
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
        // 捕获一帧图像
        cap >> input_mat;
        if (input_mat.empty())
        {
            std::cerr << "Warning: Captured an empty frame." << std::endl;
            continue;
        }

        // 调用模型进行预测
        high_resolution_clock::time_point start_time = high_resolution_clock::now();
        auto results = model.Predict(input_mat);
        high_resolution_clock::time_point end_time = high_resolution_clock::now();

        // 计算推理时间
        auto time_span = duration_cast<milliseconds>(end_time - start_time);
        std::cout << "Inference time: " << time_span.count() << " ms" << std::endl;

        // 可视化结果
        cv::Mat output_image;
        lockzhiner_vision_module::vision::Visualize(input_mat, output_image, results);

        edit.Print(output_image);
    }

    cap.release();
    return 0;
}