from lockzhiner_vision_module.periphery import GPIO0A0
from lockzhiner_vision_module.periphery import GPIOMode
from lockzhiner_vision_module.periphery import GPIOState

if __name__ == "__main__":
    gpio = GPIO0A0()
    
    if gpio.config(GPIOMode.IN) is False:
        print("Failed to config gpio mode")
        exit(1)
        
    state = gpio.read()
    if state == GPIOState.ERROR:
        print("Failed to read gpio mode")
        exit(1)
        
    print(f"state is {state}")