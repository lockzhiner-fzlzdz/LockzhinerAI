from lockzhiner_vision_module.periphery import PWM9
import time


if __name__ == "__main__":
    pwm = PWM9()
    pwm.config(1000000, 0.5)
    pwm.open()
    
    time_index = 0
    total_time = 10
    while time_index < total_time:
        print(f"Wait: {time_index}/{total_time}")
        time_index += 1
        time.sleep(1)
        
    pwm.close()