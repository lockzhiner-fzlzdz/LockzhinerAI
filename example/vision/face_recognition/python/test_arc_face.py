from lockzhiner_vision_module.cv2 import imread
from lockzhiner_vision_module.vision import ArcFace, cosine_similarity
import time
import sys

if __name__ == "__main__":
    args = sys.argv
    if len(args) != 4:
        print("Need model path. Example: python test_arc_face.py LZ-ArcFace.rknn image_path_0 image_path_1")
        exit(1)
        
    model = ArcFace()
    if model.initialize(args[1]) is False:
        print("Failed to initialize ArcFace")
        exit(1)

    input_mat_0 = imread(args[2])
    result_0 = model.predict(input_mat_0)

    input_mat_1 = imread(args[3])
    result_1 = model.predict(input_mat_1)


    print(f"Similarity is {cosine_similarity(result_0, result_1)}")