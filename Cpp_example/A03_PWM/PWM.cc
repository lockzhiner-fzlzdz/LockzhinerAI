#include <lockzhiner_vision_module/periphery/pwm/pwm.h>

#include <cstdlib> // 为了使用atoi()
#include <cstring> // 为了使用strtof()
#include <iostream>
#include <thread>

int main(int argc, char *argv[])
{
  uint32_t frequency = 1000000;
  float duty_cycle = 0.5;
  if (argc == 3)
  {
    frequency = std::atoi(argv[1]);
    duty_cycle = std::strtof(argv[2], nullptr);
  }
  std::cout << "frequency is " << frequency << std::endl;
  std::cout << "duty_cycle is " << duty_cycle << std::endl;

  lockzhiner_vision_module::periphery::PWM9 pwm;

  if (!pwm.Open(frequency, duty_cycle))
  {
    std::cout << "Failed to open adc." << std::endl;
    return 1;
  }

  for (int i = 0; i < 10; i++)
  {
    std::cout << "Wait: " << i << "/" << 10 << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  // 关闭 PWM
  if (!pwm.Close())
  {
    std::cout << "Failed to open adc." << std::endl;
    return 1;
  }
  return 0;
}