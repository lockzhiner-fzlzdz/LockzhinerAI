import lockzhiner_vision_module.cv2 as cv2
pi = 3.1415926535897932384626433832795
# 读取图片
image = cv2.imread('line.png')
# 转换为灰度图像
gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
# 使用高斯模糊减少噪声
blurred = cv2.GaussianBlur(gray, (5, 5), 1)
# 使用 Canny 边缘检测
edges = cv2.Canny(gray, 50, 150)
# 使用霍夫变换检测直线
lines = cv2.HoughLinesP(edges, 1, pi / 180, threshold=100, minLineLength=50, maxLineGap=20)
# 绘制检测到的直线
if lines is not None:
    for line in lines[0]:
        x1, y1, x2, y2 = line
        cv2.line(image, (x1, y1), (x2, y2), (0, 255, 0), 2)
cv2.imwrite('img_detected.png', image)
