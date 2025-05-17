#include <stdio.h>
#include <string.h>
#include <lockzhiner_vision_module/edit/edit.h>
#include "yolov5.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <lockzhiner_vision_module/core/logger.h>
#include <chrono>
// output img size
#define DISP_WIDTH 320
#define DISP_HEIGHT 320

// disp size
int width = DISP_WIDTH;
int height = DISP_HEIGHT;

// model size
int model_width = 320;
int model_height = 320;

int leftPadding;
int topPadding;

// label size
extern int obj_class_num;
char *lable;

int main(int argc, char *argv[])
{
	if (argc != 4)
	{
		LOGGER_INFO("Usage: %s ./yolov5_main model_path ./label size ./label txtn ./label_txt");
	}
	obj_class_num = atoi(argv[2]);
	lable = argv[3];
	// Rknn model
	char text[16];
	// rknn上下文结构体
	rknn_app_context_t rknn_app_ctx;
	object_detect_result_list od_results;
	int ret;
	const char *model_path = argv[1];
	memset(&rknn_app_ctx, 0, sizeof(rknn_app_context_t));

	// Step 1: Load RKNN model
	if (init_yolov5_model(model_path, &rknn_app_ctx) != 0)
	{
		printf("❌ Failed to load RKNN model!\n");
		return -1;
	}
	printf("✅ RKNN model loaded successfully.\n");

	// 加载标签文件
	init_post_process();

	// 打开摄像头
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
	cv::Mat frame;
	// 在 while 循环外声明 start 和 end 变量
	std::chrono::steady_clock::time_point start, end;

	while (true)
	{
		// 记录开始时间
		start = std::chrono::steady_clock::now();
		// Step 2: Load image from command line
		cap >> frame;
		if (frame.empty())
		{
			LOGGER_INFO("❌ Failed to read frame from camera.\n");
			continue;
		}
		cv::resize(frame, frame, cv::Size(width, height), 0, 0, cv::INTER_LINEAR);
		cv::Mat letterboxImage = letterbox(frame);

		if (letterboxImage.empty() || letterboxImage.total() * letterboxImage.elemSize() != model_width * model_height * 3)
		{

			LOGGER_ERROR("❌ Input image format or size mismatch!\n");
			release_yolov5_model(&rknn_app_ctx);
			return -1;
		}

		if (rknn_app_ctx.input_mems == nullptr || rknn_app_ctx.input_mems[0] == nullptr)
		{

			LOGGER_ERROR("❌ RKNN input memory not allocated!\n");
			release_yolov5_model(&rknn_app_ctx);
			return -1;
		}

		memcpy(rknn_app_ctx.input_mems[0]->virt_addr, letterboxImage.data, model_width * model_height * 3);

		if (inference_yolov5_model(&rknn_app_ctx, &od_results) != 0)
		{
			LOGGER_ERROR("inference_yolov5_model failed");
			release_yolov5_model(&rknn_app_ctx);
			return -1;
		}

		draw_detections(od_results.count,	// 传入结果数量
						od_results.results, // 传入结果数组
						frame,				// 图像帧
						mapCoordinates);	// 直接使用现有坐标映射函数

		edit.Print(frame);
		// 记录结束时间
		end = std::chrono::steady_clock::now();
		// 计算耗时（秒）
		double elapsed_time = std::chrono::duration<double>(end - start).count();
		printf("Frame processed in %.4f seconds\n", elapsed_time);
	}
	release_yolov5_model(&rknn_app_ctx);
	deinit_post_process();
	cap.release();
	return 0;
}