from lockzhiner_vision_module.periphery import ADCIN1

if __name__ == "__main__":
    adc = ADCIN1()
    print(f"adc_data is {adc.read()} mV")