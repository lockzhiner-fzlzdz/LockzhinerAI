<h1 align="center">凌智视觉模块猫狗分类部署指南</h1>

发布版本：V0.0.0

日期：2024-11-13

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
| 2024/11/13 | 0.0.0    | 钟海滨     | 初始版本     |

## 1 简介

猫狗分类是计算机视觉入门的常见任务，我们基于 [凌智视觉模块分类模型部署指南](../../vision/classification) 训练了凌智视觉模块专用的模型，该模型能够实现对猫狗分类识别。

- 注：为了帮助大家进行二次开发，我们已经在星河社区上开源了[基于凌智视觉模块的猫狗分类数据集](https://aistudio.baidu.com/datasetdetail/304279)

## 2 运行前的准备

- 请确保你已经下载了 [凌智视觉模块猫狗分类模型](https://gitee.com/LockzhinerAI/LockzhinerVisionModule/releases/download/v0.0.2/LZ-Dog-and-Cat-classfication.rknn)

## 3 在凌智视觉模块上部署猫狗分类识别案例

下载模型后，请参考以下教程使用 Python 在凌智视觉模块上部署分类模型例程：

- [凌智视觉模块猫狗分类 Python 部署指南](./python)

## 4 模型性能指标

以下测试数据为模型执行 Predict 函数运行 1000 次耗时的平均时间

| 分类模型 | FPS(帧/s) | 精度(%) |
|:-------:|:----:|:----:|
|LZ-Flower-Classfication|35||
