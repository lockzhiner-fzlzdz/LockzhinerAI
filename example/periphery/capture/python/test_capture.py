from lockzhiner_vision_module.cv2 import VideoCapture
from lockzhiner_vision_module.edit import Edit
import time
import sys

if __name__ == "__main__":
    args = sys.argv
    if len(args) != 1 and len(args) != 3:
        print("Need model path. Example: python test_capture.py <width> <height>")
        exit(1)

    width = 640
    height = 480

    if len(args) == 3:
        width = int(args[1])
        height = int(args[2])
    print(f"Capture width: {width}, height: {height}")

    edit = Edit()
    edit.start_and_accept_connection()

    video_capture = VideoCapture()
    video_capture.set_width(width)
    video_capture.set_height(height)

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
            edit.print(mat)
            total_time_ms += end_time - start_time
            read_index += 1
        print(f"FPS is {1.0 / (total_time_ms/read_index)}")
