# from rknn.api import RKNN
# import cv2

# if __name__ == '__main__':
#     # 第一步：创建RKNN对象
#     rknn = RKNN(verbose=True)

#     # 第二步：配置RKNN对象参数
#     rknn.config(
#         mean_values=[[149.94, 149.94, 149.94]],
#         std_values=[[255 * 0.193, 255 * 0.193, 255 * 0.193]],
#         target_platform='rv1106',
#     )

#     # 第三步：加载 TFLite 模型（注意：不要重新赋值给 rknn）
#     rknn.load_tflite(model='./model.tflite')

#     # 第四步：构建RKNN模型
#     rknn.build(
#         do_quantization=True,
#         dataset='./dataset.txt',
#     )

#     # 第五步：导出RKNN模型
#     rknn.export_rknn(export_path='./resnet.rknn')

#     # 第六步：进行精度分析
#     img = cv2.imread('../data/0.png')
#     img = cv2.resize(img, (96, 96))
#     rknn.accuracy_analysis(
#         inputs=[img],
#         output_dir='snapshot',
#         target='rv1106',
#         device_id='10.1.1.144:5555',
#     )

#     # 最后一步：释放RKNN对象
#     rknn.release()
import numpy as np
from rknn.api import RKNN
import cv2

if __name__ == '__main__':
    # 创建RKNN对象
    rknn = RKNN(verbose=True)
    rknn.config(
        mean_values=[[149.94, 149.94, 149.94]],
        std_values=[[255 * 0.193, 255 * 0.193, 255 * 0.193]],
        target_platform='rv1106',
    )
    # 配置参数...
    rknn.config(
        mean_values=[[149.94, 149.94, 149.94]],
        std_values=[[255 * 0.193, 255 * 0.193, 255 * 0.193]],
        target_platform='rv1106',
    )

    # 加载 TFLite 模型
    rknn.load_tflite(model='./model.tflite')

    # 构建模型
    rknn.build(do_quantization=True, dataset='./dataset.txt')

    # 导出模型
    rknn.export_rknn(export_path='./resnet.rknn')

    # 图像预处理
    img = cv2.imread('../data/0.png')
    img = cv2.resize(img, (96, 96))

    # 归一化 + 量化（根据你设置的 mean_values 和 std_values）
    mean = [149.94, 149.94, 149.94]
    std = [255 * 0.193, 255 * 0.193, 255 * 0.193]

    # 转换为 float32 并应用归一化
    img = img.astype(np.float32)
    img -= mean  # 减均值
    img /= std   # 除以标准差

    # 量化到 uint8
    img = np.round(img).astype(np.uint8)

    # 调整通道顺序为 CHW 并添加 batch 维度
    img = np.transpose(img, (2, 0, 1))  # HWC -> CHW
    img = np.expand_dims(img, axis=0)   # CHW -> BCHW
    # 精度分析
    rknn.accuracy_analysis(
        inputs=[img],
        output_dir='snapshot',
        target='rv1106',
        device_id='10.1.1.144:5555',
    )

    # 释放资源
    rknn.release()