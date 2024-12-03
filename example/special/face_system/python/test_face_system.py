import sys
import time

from lockzhiner_vision_module.cv2 import VideoCapture, imread, imwrite
from lockzhiner_vision_module.vision import FaceRecognitionSystem, visualize
from lockzhiner_vision_module.edit import Edit


# 主程序入口
if __name__ == '__main__':
    # 获取命令行参数
    args = sys.argv
    # 确保提供了足够的参数
    if len(args) != 5:
        print("Need model path. Example: python face_system.py LZ-Face.rknn LZ-ArcFace.rknn baseDataset_root crop_root")
        exit(1)

    # 初始化面部识别系统
    face_system = FaceRecognitionSystem()
    # 验证面部识别系统的初始化是否成功
    if face_system.initialize(args[1], args[2]) is False:
        print("Failed to initialize face_system model")
        exit(1)
    # 构建面部数据库
    face_system.buildDatabase(args[3],args[4])

    # 初始化图像编辑处理对象
    edit = Edit()
    # 开始并接受连接，准备进行图像编辑处理
    edit.start_and_accept_connection()
    # 初始化视频捕捉对象
    video_capture = VideoCapture()
    # 验证摄像头是否成功打开
    if video_capture.open(0) is False:
        print("Failed to open capture")
        exit(1)
    # 主循环，用于持续捕捉和处理视频帧
    while True:
        read_index = 0
        total_time_ms = 0
        for i in range(100):
            # 从摄像头读取帧
            ret, input_mat = video_capture.read()
            # 如果读取失败，则继续循环直到成功读取
            if ret is False:
                continue
            # 预测调用predict方法
            start_time = time.time()
            results = face_system.predict(input_mat)
            end_time = time.time()
            total_time_ms += end_time - start_time
            read_index += 1
            # 可视化处理结果
            vis_mat = visualize(input_mat, results)
            # 打印处理后的图像
            edit.print(vis_mat)
        print(f"FPS is {1.0 / (total_time_ms / read_index)}")