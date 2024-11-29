import lockzhiner_vision_module.cv2 as cv2
# 读取图像
image_path = 'circle.png'
img = cv2.imread(image_path)
if img is None:
    print("Error: Image not loaded.")
else:
    # 转换为灰度图像
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    # 使用高斯模糊减少噪声
    blurred = cv2.GaussianBlur(gray, (9, 9), 2)
    # 使用 HoughCircles 检测圆形
    circles = cv2.HoughCircles(blurred, cv2.HOUGH_GRADIENT, dp=1, minDist=50,
                               param1=50, param2=30, minRadius=0, maxRadius=0)
    if circles is not None:
        # 在图像上绘制检测到的圆
        for i in circles[0]:
            center = (int(i[0]), int(i[1]))
            radius = int(i[2])
            # 绘制圆心
            cv2.circle(img, center, 1, (0, 100, 100), 3)
            # 绘制圆
            cv2.circle(img, center, radius, (0, 255, 0), 3)
    # # 保存结果图像
    cv2.imwrite('img_circles.png', img)
