import lockzhiner_vision_module.cv2 as cv2
# 读取图片
image = cv2.imread('car.png')

# 检查图像是否成功读取
if image is None:
    print("Error: Unable to load image.")
    exit()

# 转换为灰度图像
gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
print("转换为灰度图像")

# 高斯模糊
blurred = cv2.GaussianBlur(gray, (3, 3), 0)
print("应用高斯模糊")

# 阈值操作
_, thresholded = cv2.threshold(blurred, 100, 255, cv2.THRESH_BINARY)
print("应用阈值操作")
cv2.imwrite('thresholded.png', thresholded)
# 使用 Canny 边缘检测
edges = cv2.Canny(thresholded, 20, 10)
print("使用 Canny 边缘检测")
# 保存边缘检测结果
cv2.imwrite('edges.png', edges)
