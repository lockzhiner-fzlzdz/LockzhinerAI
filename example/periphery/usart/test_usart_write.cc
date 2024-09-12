#include <lockzhiner_vision_module/periphery/usart/usart.h>  // 引入串口类头文件

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

  // 尝试发送字符串"Hello World\n"
  if (!usart.Write("Hello World\n")) {
    // 如果发送失败，则输出错误信息
    std::cout << "Failed to send data." << std::endl;
  }
  return 0;
}