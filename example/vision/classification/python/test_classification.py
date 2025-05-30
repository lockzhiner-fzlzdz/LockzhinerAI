from lockzhiner_vision_module.cv2 import VideoCapture
from lockzhiner_vision_module.vision import PaddleClas, visualize
from lockzhiner_vision_module.edit import Edit
import sys
import time


def predict_video(cls_model):
    edit = Edit()
    edit.start_and_accept_connection()
    video_capture = VideoCapture()
    if video_capture.open(0) is False:
        print("Failed to open capture")
        exit(1)

    while True:
        read_index = 0
        total_time_ms = 0
        for i in range(30):
            ret, mat = video_capture.read()
            if ret is False:
                continue

            start_time = time.time()
            result = cls_model.predict(mat)
            end_time = time.time()
            total_time_ms += end_time - start_time
            read_index += 1
            print(result.label_id, result.score)
            vis_mat = visualize(mat, result)
            edit.print(vis_mat)
        print(f"FPS is {1.0 / (total_time_ms / read_index)}")


def predict_image(cls_model, image_path):
    image = imread(image_path)
    result = cls_model.predict(image)
    print(result.label_id, result.score)


if __name__ == "__main__":
    args = sys.argv
    if len(args) < 2:
        print(
            "Need model path. Example: python test_classification.py LZ-MobileNetV3.rknn"
        )
        exit(1)

    model = PaddleClas()
    if model.initialize(args[1]) is False:
        print("Failed to initialize PaddleClas")
        exit(1)

    if len(args) == 2:
        print("\n")
        print("-----------------------------------------------------------------------")
        print("Open the LockzhinerVisionModuleImageFetcher and select the connection")
        predict_video(model)



    elif len(args) == 3:
        predict_image(model, args[2])
