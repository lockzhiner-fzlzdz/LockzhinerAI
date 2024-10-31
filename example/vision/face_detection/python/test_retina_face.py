from lockzhiner_vision_module.cv2 import VideoCapture
from lockzhiner_vision_module.vision import RetinaFace, visualize
from lockzhiner_vision_module.edit import Edit
import time
import sys

if __name__ == "__main__":
    args = sys.argv
    if len(args) != 2:
        print("Need model path. Example: python test_retina_face.py LZ-RetinaFace.rknn")
        exit(1)
        
    edit = Edit()
    edit.start_and_accept_connection()

    model = RetinaFace()
    if model.initialize(args[1]) is False:
        print("Failed to initialize RetinaFace")
        exit(1)

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
            results = model.predict(mat)
            end_time = time.time()
            total_time_ms += end_time - start_time
            read_index += 1

            print(f"result size is {len(results)}")
            for result in results:
                box = result.box
                score = result.score
                label_id = result.label_id
                print(
                    f"(x,y,w,h,score): [{box.x},{box.y},{box.width},{box.height},{score}]"
                )
            vis_mat = visualize(mat, results)
            edit.print(vis_mat)
        print(f"FPS is {1.0 / (total_time_ms/read_index)}")