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

  if (!usart.Write("Hello World\n"))
  {
    std::cout << "Failed to send data." << std::endl;
    return 1;
  }
  return 0;
}