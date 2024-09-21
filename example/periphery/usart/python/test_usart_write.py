from lockzhiner_vision_module.periphery import USART1


if __name__ == "__main__":
    usart = USART1()
    if usart.open(115200) is False:
        print("Failed to open usart.")
        exit(1)

    if usart.write("Hello World") is False:
        print("Failed to send data.")
        exit(0)
