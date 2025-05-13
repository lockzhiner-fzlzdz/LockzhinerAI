#include <lockzhiner_vision_module/vision/deep_learning/detection/paddle_det.h>
#include <ncnn/net.h>
#include <lockzhiner_vision_module/edit/edit.h>
#include <chrono>
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "myfontface.h"

using namespace std;
using namespace std::chrono;

// OCR字符集配置
const string plate_chars[68] = {
    "京", "沪", "津", "渝", "冀", "晋", "蒙", "辽", "吉", "黑", "苏", "浙",
    "皖", "闽", "赣", "鲁", "豫", "鄂", "湘", "粤", "桂", "琼", "川", "贵",
    "云", "藏", "陕", "甘", "青", "宁", "新", "0",  "1",  "2",  "3",  "4",
    "5",  "6",  "7",  "8",  "9",  "A",  "B",  "C",  "D",  "E",  "F",  "G",
    "H",  "J",  "K",  "L",  "M",  "N",  "P",  "Q",  "R",  "S",  "T",  "U",
    "V",  "W",  "X",  "Y",  "Z",  "I",  "O",  "-"};

ncnn::Net ocr_net;

bool InitOCRModel(const string& param_path, const string& model_path) {
    return ocr_net.load_param(param_path.c_str()) == 0 &&
           ocr_net.load_model(model_path.c_str()) == 0;
}

string RecognizePlate(cv::Mat plate_img) {
    cv::resize(plate_img, plate_img, cv::Size(94, 24));
    
    ncnn::Mat in = ncnn::Mat::from_pixels(plate_img.data, 
                                        ncnn::Mat::PIXEL_BGR,
                                        plate_img.cols, 
                                        plate_img.rows);

    const float mean[3] = {127.5f, 127.5f, 127.5f};
    const float norm[3] = {0.0078125f, 0.0078125f, 0.0078125f};
    in.substract_mean_normalize(mean, norm);

    ncnn::Extractor ex = ocr_net.create_extractor();
    ex.input("input.1", in);
    
    ncnn::Mat feat;
    ex.extract("131", feat);

    string license;
    vector<int> preb;
    for (int c = 0; c < feat.c; c++) {
        const float* data = feat.channel(c);
        for (int w = 0; w < feat.w; w++) {
            float max_val = -FLT_MAX;
            int max_idx = 0;
            for (int h = 0; h < feat.h; h++) {
                float val = data[w + h * feat.w];
                if (val > max_val) {
                    max_val = val;
                    max_idx = h;
                }
            }
            preb.push_back(max_idx);
        }
    }

    // 后处理去重
    vector<int> valid_labels;
    int prev = -1;
    for (size_t i = 0; i < preb.size(); ++i) {
        if (preb[i] != 67 && preb[i] != prev) {
            valid_labels.push_back(preb[i]);
        }
        prev = preb[i];
    }

    for (int idx : valid_labels) {
        license += plate_chars[idx];
    }

    return license.empty() ? "UNKNOWN" : license;
}

int main(int argc, char** argv) {
    // 参数验证
    if (argc < 4 || argc > 5) {
        cerr << "Usage: " << argv[0] 
             << " <det_model> <ocr_param> <ocr_model> [image_path]\n"
             << "Example:\n"
             << "  Realtime: " << argv[0] << " det_model ocr.param ocr.bin\n"
             << "  Image:    " << argv[0] << " det_model ocr.param ocr.bin test.jpg\n";
        return 1;
    }

    // 初始化检测模型
    lockzhiner_vision_module::vision::PaddleDet detector;
    if (!detector.Initialize(argv[1])) {
        cerr << "Failed to load detection model: " << argv[1] << endl;
        return 1;
    }

    // 初始化OCR模型
    if (!InitOCRModel(argv[2], argv[3])) {
        cerr << "Failed to load OCR model: " << argv[2] << " and " << argv[3] << endl;
        return 1;
    }
    MyFontFace myfont;
    // 设置文字参数
    double font_scale = 0.6;
    int thickness = 1;

    // 图片处理模式
    if (argc == 5) {
        cv::Mat image = cv::imread(argv[4]);
        if (image.empty()) {
            cerr << "Failed to read image: " << argv[4] << endl;
            return 1;
        }

        auto results = detector.Predict(image);
        // 可视化并显示结果
        cv::Mat output_image = image.clone();
        for (const auto& det : results) {
            cv::Rect rect(
                static_cast<int>(det.box.x),
                static_cast<int>(det.box.y),
                static_cast<int>(det.box.width),
                static_cast<int>(det.box.height)
            );
            cv::rectangle(
                output_image,
                rect,
                cv::Scalar(0, 255, 0),
                1,
                cv::LINE_AA
            );
        }
        cout << "\n===== 检测到 " << results.size() << " 个车牌 =====" << endl;

        for (size_t i = 0; i < results.size(); ++i) {
            const auto& det = results[i];
            cv::Rect roi(det.box.x, det.box.y, det.box.width, det.box.height);
            roi &= cv::Rect(0, 0, image.cols, image.rows);

            if (roi.area() > 0) {
                cv::Mat plate_img = image(roi);
                cv::imshow("DetectionSeg Result", plate_img);
                string plate_num = RecognizePlate(plate_img);
                // 左上角偏移
                cv::Point text_org(roi.x + 2, roi.y - 2); 
                // 先绘制黑色背景提升可读性
                cv::putText(output_image, plate_num, text_org,
                    cv::Scalar(0, 0, 0),   // 颜色
                    myfont,                // 字体对象
                    font_scale,            // 字体尺寸
                    thickness + 2,         // 线宽
                    cv::PutTextFlags::PUT_TEXT_ALIGN_LEFT,  // 对齐方式
                    cv::Range(0, 0));                     // 自动换行范围（0表示不换行）
                cv::putText(output_image, plate_num, text_org, cv::Scalar(127, 0, 127), myfont, 10);

                cout << "车牌 " << i+1 << ":\n"
                     << "  位置: [" << roi.x << ", " << roi.y 
                     << ", " << roi.width << "x" << roi.height << "]\n"
                     << "  置信度: " << det.score << "\n"
                     << "  识别结果: " << plate_num << "\n" << endl;

                cv::imshow("Detection Result", output_image);
            }
        }
        cv::waitKey(0);
    }
    // 实时摄像头模式
    else {
        // 初始化设备连接
        lockzhiner_vision_module::edit::Edit edit;
        if (!edit.StartAndAcceptConnection()) {
            std::cerr << "Error: Failed to start and accept connection." << std::endl;
            return EXIT_FAILURE;
        }
        std::cout << "Device connected successfully." << std::endl;

        cv::VideoCapture cap;
        cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
        cap.open(0);
        if (!cap.isOpened()) {
            cerr << "Error: Could not open camera" << endl;
            return 1;
        }

        cout << "实时识别模式启动 (按ESC退出)..." << endl;

        cv::Mat frame;
        while (true) {
            cap >> frame;
            if (frame.empty()) continue;

            auto results = detector.Predict(frame);

            cv::Mat display_frame = frame.clone();
            for (const auto& det : results) {
                cv::Rect rect(
                    static_cast<int>(det.box.x),
                    static_cast<int>(det.box.y),
                    static_cast<int>(det.box.width),
                    static_cast<int>(det.box.height)
                );
                cv::rectangle(
                    display_frame,
                    rect,
                    cv::Scalar(0, 255, 0),
                    1,
                    cv::LINE_AA
                );
            }

            // 添加时间戳
            auto now = system_clock::now();
            time_t now_time = system_clock::to_time_t(now);
            cout << "\n===== " << ctime(&now_time)
                 << "检测到 " << results.size() << " 个车牌 =====" << endl;

            for (const auto& det : results) {
                cv::Rect roi(det.box.x, det.box.y, det.box.width, det.box.height);
                roi &= cv::Rect(0, 0, frame.cols, frame.rows);

                if (roi.area() > 0) {
                    cv::Mat plate_img = frame(roi);
                    string plate_num = RecognizePlate(plate_img);
                    // 左上角偏移
                    cv::Point text_org(roi.x + 2, roi.y - 2); 
                    // 先绘制黑色背景提升可读性
                    cv::putText(display_frame, plate_num, text_org,
                        cv::Scalar(0, 0, 0),   // 颜色
                        myfont,                // 字体对象
                        font_scale,            // 字体尺寸
                        thickness + 2,         // 线宽
                        cv::PutTextFlags::PUT_TEXT_ALIGN_LEFT,  // 对齐方式
                        cv::Range(0, 0));                     // 自动换行范围（0表示不换行）
                    cv::putText(display_frame, plate_num, text_org, cv::Scalar(127, 0, 127), myfont, 10);

                    cout << "[实时结果] 位置(" << roi.x << "," << roi.y 
                         << ") 识别: " << plate_num 
                         << " (置信度: " << det.score << ")" << endl;
                }
            }
            edit.Print(display_frame);
            // 退出检测
            if (cv::waitKey(1) == 27) break;
        }
    }
    return 0;
}