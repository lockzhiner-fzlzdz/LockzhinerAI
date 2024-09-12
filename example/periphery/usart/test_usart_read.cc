#include <lockzhiner_vision_module/periphery/usart/usart.h>  // 串口类头文件

#include <iostream>  // 引入标准输入输出流库

int main() {
  // 创建串口类对象
  lockzhiner_vision_module::periphery::USART usart;

  // 尝试以115200的波特率打开串口
  if (!usart.Open(115200)) {
    // 如果打开失败，则输出错误信息并返回1
    std::cout << "Failed to open usart." << std::endl;
    return 1;
  }

  // 输出开始接收串口数据的提示信息
  std::cout << "Start receiving serial port data." << std::endl;

  // 无限循环，持续接收串口数据
  while (1) {
    std::string buffer;  // 用于存储接收到的串口数据
    // 从串口读取数据到buffer中，最多读取1024字节
    usart.Read(buffer, 1024);
    // 如果buffer不为空，则输出接收到的数据
    if (!buffer.empty()) {
      std::cout << buffer << std::endl;
    }
  }
  return 0;
}