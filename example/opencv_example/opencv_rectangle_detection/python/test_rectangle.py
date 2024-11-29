import lockzhiner_vision_module.cv2 as cv2
# 读取图像
image = cv2.imread('rectangle.png')
gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
# 高斯模糊
blurred = cv2.GaussianBlur(gray, (5, 5), 0)
# 二值化
_, binary = cv2.threshold(blurred, 127, 255, cv2.THRESH_BINARY_INV)
edges = cv2.Canny(binary, 30, 200)
contours, _ = cv2.findContours(edges.copy(), cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)
for contour in contours:
    # 近似轮廓
    # 计算轮廓周长
    epsilon = 0.02 * cv2.arcLength(contour, True)
    # 将轮廓近似为多边形
    approx = cv2.approxPolyDP(contour, epsilon, True)
    # 如果近似轮廓有4个顶点，则认为是矩形
    if len(approx) == 4:
        cv2.drawContours(image, [approx], -1, (0, 255, 0), 2)
cv2.imwrite('img_rectangle.png', image)