<h1 align="center">凌智视觉模块圆形检测案例</h1>

发布版本：V0.0.0

日期：2024-11-28

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
| 2024/11/28 | 0.0.0    | 钟海滨    | 初始版本     |

## 1 简介

OpenCV作为计算机图像处理库，提供了丰富的功能。我们在凌智视觉模块上适配了OpenCV的霍夫圆检测算法，并封装了相关函数，使得用户能够快速使用OpenCV的霍夫圆检测算法进行检测。

## 2 在凌智视觉模块上进行边缘检测案例   
![OPENCV API 文档](../OpenCV_API.md)
为了快速上手，我们提供了边缘检测案例
```python
import lockzhiner_vision_module.cv2 as cv2
# 读取图像
image_path = 'circle.png'
img = cv2.imread(image_path)

if img is None:
    print("Error: Image not loaded.")
else:
    # 转换为灰度图像
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

    # 使用高斯模糊减少噪声
    blurred = cv2.GaussianBlur(gray, (9, 9), 2)

    # 使用 HoughCircles 检测圆形
    circles = cv2.HoughCircles(blurred, cv2.HOUGH_GRADIENT, dp=1, minDist=50,
                               param1=50, param2=30, minRadius=0, maxRadius=0)
    if circles is not None:
        # 在图像上绘制检测到的圆
        for i in circles[0]:
            center = (int(i[0]), int(i[1]))
            radius = int(i[2])

            # 绘制圆心
            cv2.circle(img, center, 1, (0, 100, 100), 3)

            # 绘制圆
            cv2.circle(img, center, radius, (0, 255, 0), 3)

    # # 保存结果图像
    cv2.imwrite('img_circles.png', img)

```
## 3 上传并测试 Python 程序

参考 [连接设备指南](../../../docs/introductory_tutorial/connect_device_using_ssh.md) 正确连接 Lockzhiner Vision Module 设备。

![](../../../docs/introductory_tutorial/images/connect_device_using_ssh/ssh_success.png)

请使用 Electerm Sftp 依次上传以下文件:

- 进入存放 **test_canny.py** 脚本文件的目录，将 **test_hough_circle.py]** 上传到 Lockzhiner Vision Module
- 进入存放 **待检测图片** 存放的目录，将 **待检测图片** 上传到 Lockzhiner Vision Module

上传文件
![](./python/images/ssh.png)

请使用 Electerm Ssh 并在命令行中执行以下命令:

```bash
python test_hough_circle.py
```

运行程序后，屏幕上输出 
![](./python/images/img.png)
下载结果
![](./python/images/result.png)
边缘检测结果图片
![](./python/images/img_circles.png)
![alt text](./images/build_database.png)
![alt text](./images/fps.png)


