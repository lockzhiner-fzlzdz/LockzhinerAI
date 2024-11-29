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
            blurred = cv2.GaussianBlur(gray, (3, 3), 0)
            _, thresh = cv2.threshold(blurred, 127, 255, cv2.THRESH_BINARY)
            edges = cv2.Canny(thresh, 50, 150)

            # 3. 查找轮廓
            contours, _ = cv2.findContours(edges, cv2.RETR_LIST, cv2.CHAIN_APPROX_NONE)
            print(f"len:{len(contours)}")
            # 4. 筛选轮廓
            for contour in contours:
                # 计算轮廓的周长
                perimeter = cv2.arcLength(contour, True)
                if perimeter<15:
                    continue
                # 近似轮廓
                approx = cv2.approxPolyDP(contour, 0.04 * perimeter, True)

                # 如果近似轮廓有3个顶点，则认为它是三角形
                if len(approx) == 3:
                    # 5. 绘制结果
                    cv2.putText(mat, "Triangle", (50,50), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), )
                    cv2.drawContours(mat, [approx], -1, (0, 255, 0), 2)
            # cv2.imwrite("blurred.png", blurred)
            edit.print(mat)
            total_time_ms += end_time - start_time
            read_index += 1
        print(f"FPS is {1.0 / (total_time_ms / read_index)}")
