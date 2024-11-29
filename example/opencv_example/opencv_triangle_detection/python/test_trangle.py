import lockzhiner_vision_module.cv2 as cv2

def detect_triangles(image_path):
    # 1. 读取图像
    image = cv2.imread(image_path)
    if image is None:
        print("Error: Unable to load image.")
        return

    # 2. 预处理图像
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    blurred = cv2.GaussianBlur(gray, (5, 5), 0)
    edges = cv2.Canny(blurred, 50, 150)

    # 3. 查找轮廓
    contours, _ = cv2.findContours(edges, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    # 4. 筛选轮廓
    for contour in contours:
        # 计算轮廓的周长
        perimeter = cv2.arcLength(contour, True)

        # 近似轮廓
        approx = cv2.approxPolyDP(contour, 0.04 * perimeter, True)

        # 如果近似轮廓有3个顶点，则认为它是三角形
        if len(approx) == 3:
            # 5. 绘制结果
            cv2.drawContours(image, [approx], -1, (0, 255, 0), 2)
    cv2.imwrite('result_triangle.png', image)

# 示例调用
image_path = 'triangle.png'
detect_triangles(image_path)