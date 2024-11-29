<h1 align="center">凌智视觉模块 OpenCV 边缘检测 Python 部署指南</h1>

发布版本：V0.0.0

日期：2024-11-29

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

| **日期**     | **版本** | **作者** | **修改说明** |
|:-----------| -------- |--------| ------------ |
| 2024/11/29 | 0.0.0    | 钟海滨    | 初始版本     |

## 1 简介

在现代计算机视觉应用中，边缘检测是一项基础而重要的任务，它能够帮助系统理解和解析图像中的关键结构信息。边缘检测技术广泛应用于图像分割、目标识别、特征提取等多个领域。
本指南将详细介绍如何使用 OpenCV 库在凌智视觉模块上部署高效的边缘检测算法，旨在为开发者提供一个清晰、实用的操作步骤。


## 2 Python API 文档

```python
def GaussianBlur(src, ksize, sigmaX, sigmaY=0, borderType=cv2.BORDER_DEFAULT):
    """
    对图像进行高斯模糊处理。
    参数:
    - src: 输入图像。
    - ksize: 高斯核的大小，可以是一个整数或两个整数的元组。
    - sigmaX: 高斯核在X方向上的标准差。
    - sigmaY: 高斯核在Y方向上的标准差，默认为0。
    - borderType: 图像边界的处理方式，默认为cv2.BORDER_DEFAULT。
    返回:
    - 模糊处理后的图像。
    """

def threshold(src, thresh, maxval, type):
    """
    对图像进行阈值处理。
    参数:
    - src: 输入图像。
    - thresh: 阈值。
    - maxval: 最大值，应用于type所指定的阈值类型。
    - type: 阈值处理的类型。
    返回:
    - 计算得到的阈值。
    - 阈值处理后的图像。
    """
    
def Canny(img, threshold1, threshold2, apertureSize=3, L2gradient=False):
    """
    使用Canny算法检测图像中的边缘。
    参数:
    - img: 输入图像。
    - threshold1: 第一个阈值，用于边缘检测。
    - threshold2: 第二个阈值，用于边缘检测。
    - apertureSize: Sobel算子的孔径大小，默认为3。
    - L2gradient: 一个布尔值，表示是否使用更精确的L2范数进行梯度计算，默认为False。
    返回:
    - 边缘检测后的图像。
    """
```


## 3 在凌智视觉模块上进行边缘检测案例   

为了快速上手，我们提供了边缘检测案例
**测试图片下载链接：**[边缘检测图片](https://gitee.com/LockzhinerAI/LockzhinerVisionModule/releases/download/v0.0.4/car.png)

```python
import lockzhiner_vision_module.cv2 as cv2
# 读取图片
image = cv2.imread('car.png')
# 检查图像是否成功读取
if image is None:
    print("Error: Unable to load image.")
    exit()
# 转换为灰度图像
gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
# 高斯模糊
blurred = cv2.GaussianBlur(gray, (3, 3), 0)
# 阈值操作
_, thresholded = cv2.threshold(blurred, 100, 255, cv2.THRESH_BINARY)
# 使用 Canny 边缘检测
edges = cv2.Canny(thresholded, 20, 10)
# 保存边缘检测结果
cv2.imwrite('edges.png', edges)
```

## 4 上传并测试 Python 程序

参考 [连接设备指南](../../../docs/introductory_tutorial/connect_device_using_ssh.md) 正确连接 Lockzhiner Vision Module 设备。

![](../../../../docs/introductory_tutorial/images/connect_device_using_ssh/ssh_success.png)

请使用 Electerm Sftp 依次上传以下文件:

- 进入存放 **test_canny.py** 脚本文件的目录，将 **test_canny.py** 上传到 Lockzhiner Vision Module
- 进入存放 **待检测图片** 存放的目录，将 **待检测图片** 上传到 Lockzhiner Vision Module

上传文件

![](./images/img.png)

请使用 Electerm Ssh 并在命令行中执行以下命令:

```bash
python test_canny.py
```

运行程序后，屏幕上输出 

![](./images/img_2.png)

下载结果

![](./images/img_1.png)

边缘检测原图

![](./images/car.png)

边缘检测结果图片

![](./images/edges.png)




