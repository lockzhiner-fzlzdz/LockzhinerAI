<h1 align="center">凌智视觉模块串口 Python 部署指南</h1>

发布版本：V0.0.0

日期：2024-09-21

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
| 2024/09/21 | 0.0.0    | 郑必城     | 初始版本     |


## 1 简介

串口，也称为串行接口或串行通信接口，是一种用于连接计算机与外部设备并进行数据传输的接口技术。它使用较少的导线（通常只需要几根线），并且可以在较长的距离上可靠地传输数据，尽管速度相对较慢。本章节中，我们将教会你如何使用 Lockzhiner Vision Module 上的串口进行传输数据。

## 2 前期准备

接下来让我们基于串口来部署 GPIO 例程。在开始本章节前：

- 在开始这个章节前，请确保你已经按照 [开发环境搭建指南](../../../../docs/introductory_tutorial/python_development_environment.md) 正确配置了开发环境。
- 请确保你已经按照 [凌智视觉模块 GPIO 使用指南](../README.md) 正确将信号连接到开发板并打开了串口工具。

## 3 API 文档

## 4 项目介绍

## 5 执行串口测试程序

参考 [连接设备指南](../../../../docs/introductory_tutorial/connect_device_using_ssh.md) 正确连接 Lockzhiner Vision Module 设备。

![](../../../../docs/introductory_tutorial/images/connect_device_using_ssh/ssh_success.png)

使用 SFTP 功能将软件上传到 Lockzhiner Vision Module

![](images/sftp.png)

### 5.1 测试串口发送例程

```bash
python test_usart_write.py
```

![](images/usart_wirte_electerm.png)

vofa+ 上显示发送的字符串（Hello World）

![](../images/usart_write_voft.png)

### 5.2 测试串口接收例程

```bash
python test_usart_read.py
```

vofa+ 点击发送字符串（Hello World）

![](../images/usart_send_voft.png)

Lockzhiner Vision Module 打印接受到的字符串（Hello World）

![](images/usart_send_show_electerm.png)