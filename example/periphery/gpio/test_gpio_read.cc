#include <lockzhiner_vision_module/periphery/gpio/gpio.h>

#include <iostream>
#include <thread>

int main() {
  lockzhiner_vision_module::periphery::GPIO_0A0 gpio_0A0;

  if (!gpio_0A0.Config(lockzhiner_vision_module::periphery::GPIOMode::IN)) {
    std::cout << "Failed to config gpio mode" << std::endl;
    return 1;
  }

  lockzhiner_vision_module::periphery::GPIOState state;
  if (!gpio_0A0.Read(state)) {
    std::cout << "Failed to read gpio mode" << std::endl;
    return 1;
  }

  std::cout << "state is " << static_cast<uint8_t>(state) << std::endl;
  return 0;
}