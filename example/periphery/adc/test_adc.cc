#include <lockzhiner_vision_module/periphery/adc/adc.h>

#include <iostream>

int main() {
  lockzhiner_vision_module::periphery::ADC adc;
  if (!adc.Open()) {
    std::cout << "Failed to open adc." << std::endl;
    return 1;
  }

  float adc_data;
  if (!adc.Read(adc_data)) {
    std::cout << "Failed to read adc data." << std::endl;
    return 1;
  }
  std::cout << "adc_data is " << adc_data << "mV" << std::endl;
  return 0;
}