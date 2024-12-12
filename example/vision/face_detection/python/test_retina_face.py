from lockzhiner_vision_module.cv2 import imread, imwrite, VideoCapture
from lockzhiner_vision_module.vision import RetinaFace, visualize
from lockzhiner_vision_module.edit import Edit
import time
import sys


def predict_video(face_det_model, width, height):
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
            ret, mat = video_capture.read()
            if ret is False:
                continue

            start_time = time.time()
            results = face_det_model.predict(mat)
            end_time = time.time()
            total_time_ms += end_time - start_time
            read_index += 1
            print(f"result size is {len(results)}")
            for result in results:
                box = result.box
                score = result.score
                print(
                    f"(x,y,w,h,score): [{box.x},{box.y},{box.width},{box.height},{score}]"
                )
            vis_mat = visualize(mat, results)
            edit.print(vis_mat)
        print(f"FPS is {1.0 / (total_time_ms/read_index)}")


def predict_image(face_det_model, image_path):
    image = imread(image_path)
    results = face_det_model.predict(image)
    vis_mat = visualize(image, results)
    print(f"result size is {len(results)}")
    for result in results:
        box = result.box
        score = result.score
        print(
            f"(x,y,w,h,score): [{box.x},{box.y},{box.width},{box.height},{score}]"
        )
    imwrite("face_det.png", vis_mat)
    

if __name__ == "__main__":
    args = sys.argv
    if len(args) < 2:
        print("Need model path. Example: python test_retina_face.py LZ-RetinaFace.rknn width height")
        exit(1)

    model = RetinaFace()
    if model.initialize(args[1]) is False:
        print("Failed to initialize RetinaFace")
        exit(1)

    video_width = 640
    video_height = 480
    if len(args) == 5:
        video_width = int(args[3])
        video_height = int(args[4])

    if len(args) == 2:
        predict_video(model, video_width, video_height)
    elif len(args) == 3:
        predict_image(model, args[2])