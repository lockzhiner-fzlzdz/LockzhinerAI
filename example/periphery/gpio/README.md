<h1 align="center">凌智视觉模块 GPIO 使用指南</h1>

发布版本：V0.0.0

日期：2024-09-14

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
| 2024/09/14 | 0.0.0    | 郑必城     | 初始版本     |

## 1 简介

GPIO（General Purpose Input/Output，通用输入/输出）是一种在嵌入式系统和微控制器上常用的接口，用于实现基本的输入输出操作。这些引脚可以被配置为输入模式，用于读取外部信号（如按钮按下、传感器数据等），或者配置为输出模式，用于控制外部设备（如LED灯、电机等）。本章节中，我们将教会你如何使用 Lockzhiner Vision Module 上的 GPIO 输出和读取电平。

## 2 正确连接设备

板子上的引脚丝印较小，如果看不清引脚信息，可以参考下图

![](../../../images/periphery.png)

### 2.1 测试 GPIO 输出模式

测试 GPIO 输出模式时，我们使用 **示波器** 进行调试，请正确的将 Lockzhiner Vision Module 的引脚按照以下方式连接

- LockzhinerVisionModule GPIO_0A0 <-> Input IO
- LockzhinerVisionModule GND <-> Input GND

### 2.2 测试 GPIO 输入模式（输入高电平）

为了方便调试，我们这里使用 **3V3 引脚** 进行调试，请正确的将 Lockzhiner Vision Module 的引脚按照以下方式连接

- LockzhinerVisionModule GPIO_0A0 <-> LockzhinerVisionModule 3V3

### 2.3 测试 GPIO 输入模式（输入低电平）

为了方便调试，我们这里使用 **GND 引脚** 进行调试，请正确的将 Lockzhiner Vision Module 的引脚按照以下方式连接

- LockzhinerVisionModule GPIO_0A0 <-> LockzhinerVisionModule GND

在 Lockzhiner Vision Module 上运行以下代码来执行 GPIO 接收程序

## 3 在凌智视觉模块上部署 ADC 例程

请参考以下教程使用 C++ 或 Python 在凌智视觉模块上部署 GPIO 例程：

- [凌智视觉模块 GPIO C++ 部署指南](./cpp/README.md)
- [凌智视觉模块 GPIO Python 部署指南](./python/README.md)
