<h1 align="center">凌智视觉模块人脸识别模型 Python 部署指南</h1>

发布版本：V0.0.0

日期：2024-11-04

文件密级：□绝密 □秘密 □内部资料 ■公开  

---

**免责声明**  

本文档按**现状**提供，福州凌睿智捷电子有限公司（以下简称**本公司**）不对本文档中的任何陈述、信息和内容的准确性、可靠性、完整性、适销性、适用性及非侵权性提供任何明示或暗示的声明或保证。本文档仅作为使用指导的参考。  

由于产品版本升级或其他原因，本文档可能在未经任何通知的情况下不定期更新或修改。  

**读者对象**  

本教程适用于以下工程师：  

- 技术支持工程师  
- 软件开发工程师  

**修订记录**  

| **日期**   | **版本** | **作者** | **修改说明** |
| :--------- | -------- | -------- | ------------ |
| 2024/11/04 | 0.0.0    | 郑必城     | 初始版本     |

## 1 简介

接下来让我们基于 Python 来部署 ArcFace 人脸识别模型。在开始本章节前：

- 请确保你已经参考 [凌智视觉模块人脸识别模型部署指南](../README.md) 正确下载了凌智视觉模块人脸识别模型。
- 请确保你已经按照 [开发环境搭建指南](../../../../docs/introductory_tutorial/python_development_environment.md) 正确配置了开发环境。

## 2 Python API 文档

```python
class FaceRecognitionResult:
    """
    人脸识别结果类，用于封装和处理人脸识别结果数据。

    该类主要提供了一个包装层，用于访问和管理由视觉模块产生的人脸识别结果。
    """

    def __init__(self):
        self.face_recognition_result = vision.FaceRecognitionResult()

    @property
    def feature(self):
        """
        获取人脸识别结果的特征信息

        Returns:
            list(float): 特征信息
        """
        return self.face_recognition_result.feature

class ArcFace:
    """
    ArcFace 类 - 用于人脸识别的 ArcFace 模型封装。

    该类封装了 InsightFace 框架下的 ArcFace 人脸识别模型，提供了初始化和预测的方法。
    """

    def __init__(self):
        """
        构造函数 - 初始化 PaddleDet 对象。
        """
        self.model = vision.ArcFace()

    def initialize(self, model_path):
        """
        初始化模型 - 加载预训练的 ArcFace 模型。

        Args:
            model_path (str): 模型文件的路径。

        Returns:
            bool: 初始化是否成功。
        """
        return self.model.initialize(model_path)

    def predict(self, input_mat):
        """
        进行预测 - 使用加载的模型对输入数据进行分类预测。

        Args:
            input_mat (cv2.Mat): 输入的图像数据，通常是一个 cv2.Mat 变量。

        Returns:
            FaceRecognitionResult: 人脸识别结果，包含了 128 维度的人脸特征
        """
        return self.model.predict(input_mat)
```

## 3 项目介绍

为了方便大家入手，我们做了一个简易的人脸识别例程。该程序可以使用摄像头进行端到端推理。

```python
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
```

## 4 上传并测试 Python 程序

参考 [连接设备指南](../../../../docs/introductory_tutorial/connect_device_using_ssh.md) 正确连接 Lockzhiner Vision Module 设备。

![](../../../../docs/introductory_tutorial/images/connect_device_using_ssh/ssh_success.png)

请使用 Electerm Sftp 依次上传以下两个文件:

- 进入存放 **test_arc_face.py** 脚本文件的目录，将 **test_arc_face.py** 上传到 Lockzhiner Vision Module
- 进入存放 **LZ-ArcFace.rknn(也可能是其他模型)** 模型存放的目录（模型存放在训练模型后下载的 output 文件夹内），将 **LZ-ArcFace.rknn** 上传到 Lockzhiner Vision Module

![](images/stfp_0.png)

![](images/stfp_1.png)

请使用 Electerm Ssh 并在命令行中执行以下命令:

```bash
python test_arc_face.py LZ-ArcFace.rknn image_path_0 image_path_1
```

运行程序后，屏幕上开始打印矩形框信息和置信度，并在一段时间后输出 FPS 值

![alt text](result_0.png)

<!-- ## 5 其他 -->

<!-- 如果你需要使用 C++ 来部署 PaddleClas 请参考[凌智视觉模块分类模型 C++ 部署指南](../cpp/README.md)。 -->
