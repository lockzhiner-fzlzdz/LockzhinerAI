#include <lockzhiner_vision_module/periphery/usart/usart.h>

#include <iostream>

int main()
{
  lockzhiner_vision_module::periphery::USART1 usart;
  if (!usart.Open(115200))
  {
    std::cout << "Failed to open usart." << std::endl;
    return 1;
  }

  std::cout << "Start receiving serial port data." << std::endl;
  while (1)
  {
    std::string buffer;
    usart.Read(buffer, 1024);
    if (!buffer.empty())
    {
      std::cout << buffer << std::endl;
    }
  }
  return 0;
}