from lockzhiner_vision_module.periphery import USART1


if __name__ == "__main__":
    usart = USART1()
    if usart.open(115200) is False:
        print("Failed to open usart.")
        exit(1)

    print("Start receiving serial port data.")

    while True:
        print(usart.read(1024))
