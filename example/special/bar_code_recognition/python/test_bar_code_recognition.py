from lockzhiner_vision_module.cv2 import VideoCapture
from lockzhiner_vision_module.vision import Code128Detector, visualize
from lockzhiner_vision_module.edit import Edit
import sys

if __name__ == "__main__":
    edit = Edit()
    edit.start_and_accept_connection()

    model = Code128Detector()

    video_capture = VideoCapture()
    if video_capture.open(0) is False:
        print("Failed to open capture")
        exit(1)

    while True:
        ret, mat = video_capture.read()
        if ret is False:
            continue

        results = model.predict(mat)
        print(f"result size is {len(results)}")
        for result in results:
            top_left = result.top_left
            top_right = result.top_right
            bottom_left = result.bottom_left
            bottom_right = result.bottom_right
            text = result.text
            print(
                f"(top_left,top_right,bottom_left,bottom_right,text): [({top_left.x}),{top_left.y}),({top_right.x}),{top_right.y}),({bottom_left.x}),{bottom_left.y}),({bottom_right.x}),{bottom_right.y}),{text}]"
            )

        vis_mat = visualize(mat, results)
        edit.print(vis_mat)
