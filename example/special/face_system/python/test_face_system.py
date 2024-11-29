import os
import sys
from lockzhiner_vision_module.cv2 import VideoCapture,imread,imwrite
from lockzhiner_vision_module.vision import PaddleDet, ArcFace, visualize, cosine_similarity
from lockzhiner_vision_module.edit import Edit
import time
import sys

def predict_face(det_model, rec_model, face_mat):
    if face_mat.empty():
        return None, None

    det_results = det_model.predict(face_mat)
    result_size = len(det_results)
    if result_size != 1:
        print(f"Failed to find face. The size of faces is {result_size}, but need 1.")
        return None, None

    det_result = det_results[0]
    face_roi = det_result.box
    crop_mat = input_mat.crop(face_roi)
    if crop_mat.empty():
        return None, None

    rec_result = rec_model.predict(crop_mat)
    return det_results, rec_result


if __name__ == '__main__':
    args = sys.argv
    if len(args) != 3:
        print("Need model path. Example: python face_system.py LZ-Face.rknn LZ-ArcFace.rknn")
        exit(1)

    face_det = PaddleDet()
    if face_det.initialize(args[1]) is False:
        print("Failed to initialize face_det model")
        exit(1)

    face_rec = ArcFace()
    if face_rec.initialize(args[2]) is False:
        print("Failed to initialize face_rec model")
        exit(1)

    # 原始数据路径
    base_dataset_path = 'BaseDataset'
    # 裁剪后的数据路径
    crop_dataset_path = 'CropDataset'

    # 如果裁剪文件夹不存在，则创建
    if not os.path.exists(crop_dataset_path):
        os.makedirs(crop_dataset_path)
    # 构建数据库
    # 临时存储特征向量的字典
    face_dict = {}
    # 遍历BaseDataset中的每个文件夹
    for user_folder in os.listdir(base_dataset_path):
        user_path = os.path.join(base_dataset_path, user_folder)
        if os.path.isdir(user_path) is False:
            continue
        # 创建对应用户的裁剪文件夹
        user_crop_path = os.path.join(crop_dataset_path, user_folder)
        if not os.path.exists(user_crop_path):
            os.makedirs(user_crop_path)
        # 初始化用户的特征向量列表
        face_dict[user_folder] = []
        for img_name in os.listdir(user_path):
            img_path = os.path.join(user_path, img_name)
            print(img_path)

            # 读取图片并检测图片是否为空
            input_mat = imread(img_path)
            if  input_mat.empty():
                print(f"{img_path} is empty.")
                continue

            _, feature_result = predict_face(face_det, face_rec, input_mat)
            if feature_result is None:
                print(f"Failed to find face in {img_path}.")
                continue
            face_dict[user_folder].append(feature_result)

    edit = Edit()
    edit.start_and_accept_connection()

    video_capture = VideoCapture()
    if video_capture.open(0) is False:
        print("Failed to open capture")
        exit(1)

    while True:
        ret, input_mat = video_capture.read()
        if ret is False:
            continue
        # 预测
        result_0, result_1 = predict_face(face_det, face_rec, input_mat)
        if result_0 is None or result_1 is None:
            print(f"Failed to find face on video.")
            continue
        vis_mat = visualize(input_mat, result_0)
        edit.print(vis_mat)

        max_name = "None"
        max_similarity = 0.0
        for name, feature_results in face_dict.items():
            for feature_result in feature_results:
                similarity = cosine_similarity(result_1, feature_result)
                if max_similarity < similarity:
                    max_similarity = similarity
                    max_name = name
        print(f"User: {max_name}, score: {max_similarity}")









