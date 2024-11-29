from lockzhiner_vision_module.cv2 import VideoCapture
import lockzhiner_vision_module.cv2 as cv2
from lockzhiner_vision_module.edit import Edit
import time
import sys
pi = 3.14159265358979323846
if __name__ == "__main__":
    args = sys.argv
    if len(args) != 3:
        print("Need model path. Example: python test_capture.py width height")
        exit(1)

    edit = Edit()
    edit.start_and_accept_connection()

    video_capture = VideoCapture()
    video_capture.set_width(int(args[1]))
    video_capture.set_height(int(args[2]))
    if video_capture.open(0) is False:
        print("Failed to open capture")
        exit(1)

    while True:
        read_index = 0
        total_time_ms = 0
        for i in range(30):
            start_time = time.time()
            ret, mat = video_capture.read()
            if ret is False:
                continue
            end_time = time.time()
            # 转换为灰度图像
            gray = cv2.cvtColor(mat, cv2.COLOR_BGR2GRAY)
            # 使用高斯模糊减少噪声
            blurred = cv2.GaussianBlur(gray, (5, 5), 1)
            # 使用 Canny 边缘检测
            edges = cv2.Canny(gray, 50, 150)
            # 使用霍夫变换检测直线
            lines = cv2.HoughLinesP(edges, 1, pi / 180, threshold=100, minLineLength=40, maxLineGap=40)
            # 绘制检测到的直线
            if lines is not None:
                for line in lines[0]:
                    x1, y1, x2, y2 = line
                    cv2.putText(mat, "line", (50, 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)
                    cv2.line(mat, (x1, y1), (x2, y2), (0, 255, 0), 2)

            edit.print(mat)
            total_time_ms += end_time - start_time
            read_index += 1
        print(f"FPS is {1.0 / (total_time_ms/read_index)}")