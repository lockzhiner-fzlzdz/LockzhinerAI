<h1 align="center">凌智视觉模块手写数字识别 Python 部署指南</h1>

发布版本：V0.0.0

日期：2024-09-26

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
| 2024/09/26 | 0.0.0    | 郑必城     | 初始版本     |

## 1 简介

接下来让我们基于 Python 来部署手写数字模型。在开始本章节前：

- 请确保你已经参考 [凌智视觉模块手写数字识别部署指南](../README.md) 正确下载了模型。
- 请确保你已经参考 [凌智视觉模块摄像头部署指南](../../periphery/capture/README.md) 正确下载了凌智视觉模块图片传输助手。
- 请确保你已经按照 [开发环境搭建指南](../../../../docs/introductory_tutorial/python_development_environment.md) 正确配置了开发环境。

## 2 Python API 文档

同[分类模型 Python 部署 API 文档](../../vision/classification/python/README.md)

## 3 项目介绍

为了方便大家入手，我们做了一个简易的手写数字识别例程。该程序可以使用摄像头进行端到端推理，并可视化推理结果到凌智视觉模块图片传输助手。

```python
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
```

## 4 上传并测试 Python 程序

参考 [连接设备指南](../../../../docs/introductory_tutorial/connect_device_using_ssh.md) 正确连接 Lockzhiner Vision Module 设备。

![](../../../docs/introductory_tutorial/images/connect_device_using_ssh/ssh_success.png)

请使用 Electerm Sftp 依次上传以下两个文件:

- 进入存放 **test_digit_handwritten_rec.py** 脚本文件的目录，将 **test_digit_handwritten_rec.py** 上传到 Lockzhiner Vision Module
- 进入存放 **LZ-DigitHandRecog.rknn** 模型存放的目录（模型存放在训练模型后下载的 output 文件夹内），将 **LZ-DigitHandRecog.rknn** 上传到 Lockzhiner Vision Module

![](images/stfp.png)

请使用 Electerm Ssh 并在命令行中执行以下命令:

```bash
python test_digit_handwritten_rec.py LZ-DigitHandRecog.rknn
```

运行程序后，使用凌智视觉模块图片传输助手连接设备，屏幕上开始打印标签索引和置信度，凌智视觉模块图片传输助手出现可视化的结果

![alt text](images/result_0.png)

<!-- ## 5 其他 -->

<!-- 如果你需要使用 C++ 来部署 PaddleClas 请参考[凌智视觉模块分类模型 C++ 部署指南](../cpp/README.md)。 -->
