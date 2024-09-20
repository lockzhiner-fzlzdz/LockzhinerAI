<h1 align="center">凌智视觉模块 GPIO Python 部署指南</h1>

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

接下来让我们基于 Python 来部署 GPIO 例程。在开始本章节前：

- 请确保你已经按照 [开发环境搭建指南](../../../../docs/introductory_tutorial/python_development_environment.md) 正确配置了开发环境。
- 请确保你已经按照 [凌智视觉模块 GPIO 使用指南](../README.md) 正确将信号连接到开发板。

## 2 API 文档

```python
from ..lockzhiner_vision_module_wapper import periphery

from enum import Enum


class GPIOMode(Enum):
    """
    @class GPIOMode
    @brief GPIO 模式枚举类

    该类定义了GPIO 的输入(IN)和输出(OUT)两种模式，基于 periphery 库。

    """

    IN = periphery.GPIOMode.IN
    OUT = periphery.GPIOMode.OUT


class GPIOState(Enum):
    """
    @class GPIOState
    @brief GPIO 状态枚举类

    该类定义了 GPIO 的高电平(HIGH)、低电平(LOW)和错误(ERROR)三种状态，基于 periphery 库。
    """

    HIGH = periphery.GPIOState.HIGH
    LOW = periphery.GPIOState.LOW
    ERROR = periphery.GPIOState.ERROR


class GPIOBase:
    """
    @class GPIOBase
    @brief GPIO 基础类

    该类提供了 GPIO 的基本操作，包括配置模式、读取状态和写入状态。
    它依赖于一个外部的 gpio_class 来实现具体的功能。
    """

    def __init__(self, gpio_class):
        """
        @fn __init__
        @brief 构造函数

        @param gpio_class: 一个实现了 GPIO 操作的类
        """
        self.gpio = gpio_class()

    def config(self, gpio_mode):
        """
        @fn config
        @brief 配置 GPIO 模式
        @param gpio_mode: 要配置的 GPIO 模式，使用 GPIOMode 枚举类指定

        @return 配置操作的结果
        """
        return self.gpio.config(gpio_mode)

    def read(self):
        """
        @fn read
        @brief 读取 GPIO 状态

        @return GPIO 的当前状态，使用 GPIOState 枚举类表示
        """
        return self.gpio.read()

    def write(self, gpio_state):
        """
        @fn write
        @brief 写入GPIO状态
        @param gpio_state: 要写入的 GPIO 状态，使用 GPIOState 枚举类指定

        @return 写入操作的结果
        """
        return self.gpio.write(gpio_state)


class GPIO0A0(GPIOBase):
    def __init__(self):
        super().__init__(periphery.GPIO0A0)


class GPIO1C7(GPIOBase):
    def __init__(self):
        super().__init__(periphery.GPIO1C7)


class GPIO2A0(GPIOBase):
    def __init__(self):
        super().__init__(periphery.GPIO2A0)


class GPIO2A1(GPIOBase):
    def __init__(self):
        super().__init__(periphery.GPIO2A1)


class GPIO2A2(GPIOBase):
    def __init__(self):
        super().__init__(periphery.GPIO2A2)


class GPIO2A3(GPIOBase):
    def __init__(self):
        super().__init__(periphery.GPIO2A3)


class GPIO2A4(GPIOBase):
    def __init__(self):
        super().__init__(periphery.GPIO2A4)


class GPIO2A5(GPIOBase):
    def __init__(self):
        super().__init__(periphery.GPIO2A5)
```

## 3 项目介绍

为了方便大家入手，我们将项目拆分为**测试 GPIO 输出模式**和**测试 GPIO 输入模式**两个例程。

测试 GPIO 输入模式的核心代码如下:

```python
```

测试 GPIO 输出模式的核心代码如下:

```python
```