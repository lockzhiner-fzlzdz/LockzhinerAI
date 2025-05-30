<h1 align="center">凌智视觉模块人脸识别模型部署指南</h1>

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

ArcFace 是一种基于深度学习的人脸识别模型，专注于在人脸特征向量（即嵌入）上实现更高的区分度。其核心在于 **角度间隔 (Angular Margin)** 的思想，通过将类间距离投影到角度空间进行学习，使得模型能够更精确地将不同类别（人脸）的特征分离开来。

在实际应用中, ArcFace 通过改进传统的 Softmax 损失函数，引入角度间隔损失来优化人脸的嵌入特征。具体而言，该模型将人脸特征映射到一个单位圆上，并在不同类别之间引入一个固定的角度间隔，从而保证不同人脸类别的特征向量在角度空间上更加分离。这一方法使得模型在区分相似人脸或小样本人脸方面更加有效。

ArcFace 的应用领域广泛，尤其在需要高度准确的身份验证、访问控制和智能安防等场景中展现出显著的性能优势。其优势在于模型能够在多样化和大规模的人脸识别任务中保持高效，同时也具有良好的泛化能力。

## 2 运行前的准备

* 请确保你已经下载了 [凌智视觉模块人脸识别模型](https://gitee.com/LockzhinerAI/LockzhinerVisionModule/releases/download/v0.0.0/LZ-ArcFace.rknn)

## 3 在凌智视觉模块上部署模型

训练完模型后，请参考以下教程在凌智视觉模块上部署检测模型例程：

<!-- * [凌智视觉模块检测模型 C++ 部署指南](./cpp/README.md) -->
* [凌智视觉模块人脸识别模型 Python 部署指南](./python)

## 4 各模型性能指标

以下测试数据为模型执行 Predict 函数运行 1000 次耗时的平均时间

| 人脸识别模型 | FPS(帧/s) | 精度(%) |
|:-------:|:----:|:----:|
|LZ-ArcFace|  120   |   -   |