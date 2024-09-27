from lockzhiner_vision_module.cv2 import VideoCapture
from lockzhiner_vision_module.vision import PaddleClas, visualize
from lockzhiner_vision_module.edit import Edit
import sys

if __name__ == "__main__":
    args = sys.argv
    if len(args) != 2:
        print("Need model path. Example: python test_digit_handwritten_rec.py LZ-DigitHandRecog.rknn")
        exit(1)
        
    edit = Edit()
    edit.start_and_accept_connection()

    model = PaddleClas()
    if model.initialize(args[1]) is False:
        print("Failed to initialize PaddleClas")
        exit(1)

    video_capture = VideoCapture()
    if video_capture.open(0) is False:
        print("Failed to open capture")
        exit(1)

    while True:
        ret, mat = video_capture.read()
        if ret is False:
            continue

        result = model.predict(mat)
        print(f"The label_id is {result.label_id} and the score is {result.score}")

        vis_mat = visualize(mat, result)
        edit.print(vis_mat)