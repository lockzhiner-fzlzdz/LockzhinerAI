from lockzhiner_vision_module.periphery import GPIO0A0, GPIOMode, GPIOState
import time

if __name__ == "__main__":
    gpio = GPIO0A0()

    if gpio.config(GPIOMode.OUT) is False:
        print("Failed to config gpio mode")
        exit(1)

    if gpio.write(GPIOState.HIGH) is False:
        print("Failed to write gpio state")
        exit(1)

    time_index = 0
    total_time = 10
    while time_index < total_time:
        print(f"Wait: {time_index}/{total_time}")
        time_index += 1
        time.sleep(1)

    if gpio.write(GPIOState.LOW) is False:
        print("Failed to write gpio state")
        exit(1)
