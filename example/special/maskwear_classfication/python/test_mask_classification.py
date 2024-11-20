from lockzhiner_vision_module.cv2 import VideoCapture
from lockzhiner_vision_module.vision import PaddleClas, visualize
from lockzhiner_vision_module.edit import Edit
import time
import sys


labels =['mask_weared_incorrect','without_mask','with_mask']

if __name__ == "__main__":
    args = sys.argv
    if len(args) != 2:
        print("Need model path. Example: python test_mask_classification.py LZ-Maskwear-Classification-2024-1120-1209")
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
        read_index = 0
        total_time_ms = 0
        for i in range(30):
            start_time = time.time()
            ret, mat = video_capture.read()
            if ret is False:
                continue

            result = model.predict(mat)

            end_time = time.time()
            total_time_ms += end_time - start_time
            read_index += 1
            print(result.label_id, result.score)
            vis_mat = visualize(mat, result, labels)
            # vis_mat = visualize(mat,result)
            edit.print(vis_mat)
        print(f"FPS is {1.0 / (total_time_ms / read_index)}")
