# import cv2
#
# # import lockzhiner_vision_module.cv2 as cv2
# # 定义π
# pi = 3.14159265358979323846
#
# # 读取图片
# image = cv2.imread('img.png')
#
# # 转换为灰度图像
# gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
# print("转换为灰度图像")
#
# # 高斯模糊
# blurred = cv2.GaussianBlur(gray, (3, 3), 0)
#
# # 使用 Canny 边缘检测
# edges = cv2.Canny(blurred, 30, 80)
# print("使用Canny边缘检测")
# cv2.imwrite('edges.png', edges)
#
# # 使用霍夫变换检测直线
# # 注意这里使用的是 cv2.HoughLinesP 而不是 cv2.HoughLines
# lines = cv2.HoughLinesP(edges, 1, pi / 180, threshold=90, minLineLength=30, maxLineGap=10)
# print("使用霍夫变换检测直线")
# print(lines.shape)
# # 绘制检测到的直线
# if lines is not None:
#     for line in lines:
#         # 注意这里的line格式，它是一个列表，包含了两个点坐标[x1, y1, x2, y2]
#         x1, y1, x2, y2 = line[0]
#         cv2.line(image, (x1, y1), (x2, y2), (0, 0, 255), 2)
#
# # 显示原始边缘检测图像和带有检测直线的图像
# cv2.imshow('Edges', edges)
# cv2.imshow('Line Detection', image)
# cv2.waitKey(0)
# cv2.destroyAllWindows()
#
# # 保存带有检测直线的图片
# cv2.imwrite('line_detected.png', image)
import lockzhiner_vision_module.cv2 as cv2
# import cv2

# import cv2
pi = 3.1415926535897932384626433832795
# 读取图片
image = cv2.imread('img.png')

# 检查图片是否成功读

# 转换为灰度图像
gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
print("转换为灰度图像")
# 使用高斯模糊减少噪声
# blurred = cv2.GaussianBlur(gray, (5, 5), 0.5)
print("高斯滤波")
# 使用 Canny 边缘检测
edges = cv2.Canny(gray, 50, 150)
print("使用Canny边缘检测")
# 使用霍夫变换检测直线
lines = cv2.HoughLinesP(edges, 1, pi / 180, threshold=100, minLineLength=50, maxLineGap=20)
print("使用霍夫变换检测直线")
# 绘制检测到的直线
if lines is not None:
    # print(lines.tolist())
    for line in lines[0]:
        # print(type(line))
        x1, y1, x2, y2 = line
        cv2.line(image, (x1, y1), (x2, y2), (0, 255, 0), 2)

cv2.imwrite('line_detected.png', image)
