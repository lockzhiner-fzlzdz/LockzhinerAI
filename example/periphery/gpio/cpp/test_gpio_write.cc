#include <lockzhiner_vision_module/periphery/gpio/gpio.h>

#include <iostream>
#include <thread>

int main() {
  lockzhiner_vision_module::periphery::GPIO0A0 gpio;

  if (!gpio.Config(lockzhiner_vision_module::periphery::GPIOMode::OUT)) {
    std::cout << "Failed to config gpio mode" << std::endl;
    return 1;
  }

  if (!gpio.Write(lockzhiner_vision_module::periphery::GPIOState::HIGH)) {
    std::cout << "Failed to config gpio mode" << std::endl;
    return 1;
  }

  for (int i = 0; i < 10; i++) {
    std::cout << "Wait: " << i << "/" << 10 << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  if (!gpio.Write(lockzhiner_vision_module::periphery::GPIOState::LOW)) {
    std::cout << "Failed to config gpio mode" << std::endl;
    return 1;
  }

  return 0;
}