<h1 align="center">凌智视觉模块 ADC C++ 部署指南</h1>

发布版本：V0.0.0

日期：2024-09-11

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
| 2024/09/11 | 0.0.0    | 郑必城     | 初始版本     |


## 1 简介

接下来让我们基于 C++ 来部署 ADC 例程。在开始本章节前：

- 请确保你已经按照 [开发环境搭建指南](../../../../docs/introductory_tutorial/cpp_development_environment.md) 正确配置了开发环境。

## 2 API 文档

```c++
/// \class ADCBase
/// \brief ADCBase
/// 类是一个模板类，用于处理特定设备和引脚的模拟数字转换器（ADC）的基础操作。
///
/// \template<uint8_t device_index, uint8_t in_voltage_index>
/// \param device_index ADC 设备编号。
/// \param in_voltage_index ADC 引脚编号。
template <uint8_t device_index, uint8_t in_voltage_index>
class ADCBase {
 public:
  /// \brief 构造函数，初始化 ADC 设备。
  ADCBase();

  /// \brief 析构函数，默认实现。
  ~ADCBase() = default;

  /// \brief 从 ADC 读取数据，并将结果存储在提供的引用中。
  ///
  /// \param[out] adc_data 存储读取数据的浮点数引用。
  /// \return 读取成功返回 true，否则返回 false。
  bool Read(float& adc_data);

  /// \brief 从 ADC 读取数据，并返回结果。
  ///
  /// \return 读取到的 ADC 数据值（mV），如果读取失败则返回 0。
  float Read();

 private:
  // clang-format off
  inline const static std::string scale_file_path_ = fmt::format("/sys/bus/iio/devices/iio:device{}/in_voltage_scale", device_index);
  inline static float scale_ = 0.0;
  inline const static std::string raw_file_path_ = fmt::format("/sys/bus/iio/devices/iio:device{}/in_voltage{}_raw", device_index, in_voltage_index);
  // clang-format on
};

/// \brief 定义 ADCIN1 别名。
using ADCIN1 = ADCBase<0, 1>;
```

## 3 项目介绍

为了方便大家入手，我们做了一个简易的 ADC 例程。该例程可以输出当前的 ADC 数值。

```cmake
# CMake最低版本要求  
cmake_minimum_required(VERSION 3.10)  

project(test_adc)

# 定义项目根目录路径
set(PROJECT_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../..")
# 定义 LockzhinerVisionModule SDK 路径
set(LockzhinerVisionModule_ROOT_PATH "${PROJECT_ROOT_PATH}/third_party/lockzhiner_vision_module_sdk")
set(LockzhinerVisionModule_DIR "${LockzhinerVisionModule_ROOT_PATH}/lib/cmake/lockzhiner_vision_module")
find_package(LockzhinerVisionModule REQUIRED)

# 配置 ADC 读取例程
add_executable(Test-ADC test_adc.cc)
target_include_directories(Test-ADC PRIVATE ${LOCKZHINER_VISION_MODULE_INCLUDE_DIRS})
target_link_libraries(Test-ADC PRIVATE ${LOCKZHINER_VISION_MODULE_LIBRARIES})
```

读取 ADC 数据的核心代码如下:

```cpp
#include <lockzhiner_vision_module/periphery/adc/adc.h>

#include <iostream>

int main() {
  lockzhiner_vision_module::periphery::ADCIN1 adc;
  float adc_data;
  if (!adc.Read(adc_data)) {
    std::cout << "Failed to read adc data." << std::endl;
    return 1;
  }
  std::cout << "adc_data is " << adc_data << "mV" << std::endl;
  return 0;
}
```

## 4 编译项目

使用 Docker Destop 打开 LockzhinerVisionModule 容器并执行以下命令来编译项目

```bash
# 进入 Demo 目录
cd /LockzhinerVisionModuleWorkSpace/LockzhinerVisionModule/example/periphery/adc/cpp

# 创建编译目录
rm -rf build && mkdir build && cd build

# 配置交叉编译工具链
export TOOLCHAIN_ROOT_PATH=${PWD}/../../../../../../arm-rockchip830-linux-uclibcgnueabihf

# 使用 cmake 配置项目
cmake -DCMAKE_TOOLCHAIN_FILE=../../../../../toolchains/arm-rockchip830-linux-uclibcgnueabihf.toolchain.cmake \
      -DCMAKE_BUILD_TYPE=Release \
      ..

# 执行编译
make -j8
```

![](images/build_example.png)

## 5 执行 ADC 测试程序

参考 [连接设备指南](../../../../docs/introductory_tutorial/connect_device_using_ssh.md) 正确连接 Lockzhiner Vision Module 设备。

![](../../../../docs/introductory_tutorial/images/connect_device_using_ssh/ssh_success.png)

使用 SFTP 功能将软件上传到 Lockzhiner Vision Module

![](images/sftp.png)

在 Lockzhiner Vision Module 上运行以下代码来执行 ADC 测试程序

```bash
chmod +x ./Test-ADC
./Test-ADC
```

![](images/result.png)

可以看到有一定的误差，误差一般在10mv以内

## 6 其他

如果你需要使用 Python 来部署 ADC 例程请参考[凌智视觉模块分类模型 Python 部署指南](../python/README.md)。