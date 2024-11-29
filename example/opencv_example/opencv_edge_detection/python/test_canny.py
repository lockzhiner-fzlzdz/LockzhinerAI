import lockzhiner_vision_module.cv2 as cv2
# 读取图片
image = cv2.imread('car.png')
# 检查图像是否成功读取
if image is None:
    print("Error: Unable to load image.")
    exit()
# 转换为灰度图像
gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
# 高斯模糊
blurred = cv2.GaussianBlur(gray, (3, 3), 0)
# 阈值操作
_, thresholded = cv2.threshold(blurred, 100, 255, cv2.THRESH_BINARY)
# 使用 Canny 边缘检测
edges = cv2.Canny(thresholded, 20, 10)
# 保存边缘检测结果
cv2.imwrite('edges.png', edges)
