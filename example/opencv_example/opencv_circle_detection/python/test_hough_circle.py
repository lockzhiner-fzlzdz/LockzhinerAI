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
            # 转换为灰度图像
            gray = cv2.cvtColor(mat, cv2.COLOR_BGR2GRAY)

            # 使用高斯模糊减少噪声
            blurred = cv2.GaussianBlur(gray, (9, 9), 2)

            # 使用 HoughCircles 检测圆形
            circles = cv2.HoughCircles(blurred, cv2.HOUGH_GRADIENT, dp=1, minDist=50,
                                       param1=50, param2=30, minRadius=120, maxRadius=250)
            # print(circles)
            if circles is not None:
                # 在图像上绘制检测到的圆
                for i in circles[0]:
                    center = (int(i[0]), int(i[1]))
                    radius = int(i[2])
                    cv2.putText(mat, "circle", (50, 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)
                    # 绘制圆心
                    cv2.circle(mat, center, 1, (0, 100, 100), 3)

                    # 绘制圆
                    cv2.circle(mat, center, radius, (0, 255, 0), 3)

            edit.print(mat)
            total_time_ms += end_time - start_time
            read_index += 1
        print(f"FPS is {1.0 / (total_time_ms/read_index)}")