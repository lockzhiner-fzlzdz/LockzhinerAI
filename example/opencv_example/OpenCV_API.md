**OPENCV_API 文档**

``` markdown
def arcLength(curve, closed):
    return cv2.arcLength(curve, closed)


def approxPolyDP(curve, epsilon, closed):
    return cv2.approxPolyDP(curve, epsilon, closed)


def boundingRect(rect):
    return cv2.boundingRect(rect)


def Canny(img, threshold1, threshold2, apertureSize=3, L2gradient=False):
    return cv2.Canny(img, threshold1, threshold2, apertureSize, L2gradient)


def circle(img, center, radius, color, thickness=1, lineType=8, shift=0):
    temp_center = convert2point(center)
    temp_color = convert2scalar(color)
    cv2.circle(img, temp_center, radius, temp_color, thickness, lineType, shift)


def cvtColor(src, code, dstCn=0):
    return cv2.cvtColor(src, code, dstCn)


def findContours(image, mode, method, point=cv2.Point()):
    contours, hierarchy = cv2.findContours(image, mode, method, point)
    return contours, hierarchy


def GaussianBlur(src, ksize, sigmaX, sigmaY=0, borderType=cv2.BORDER_DEFAULT):
    temp_ksize = convert2size(ksize)
    return cv2.GaussianBlur(src, temp_ksize, sigmaX, sigmaY, borderType)


def HoughCircles(image, method, dp, minDist, param1, param2, minRadius, maxRadius):
    return [cv2.HoughCircles(
        image, method, dp, minDist, param1, param2, minRadius, maxRadius
    )]


def HoughLines(
    image, rho, theta, threshold, srn=0, stn=0, min_theta=0, max_theta=cv2.CV_PI
):
    return [cv2.HoughLines(image, rho, theta, threshold, srn, stn, min_theta, max_theta)]


def HoughLinesP(image, rho, theta, threshold, minLineLength=0, maxLineGap=0):
    return [cv2.HoughLinesP(image, rho, theta, threshold, minLineLength, maxLineGap)]


def line(img, pt1, pt2, color, thickness=1, lineType=8, shift=0):
    temp_pt1 = convert2point(pt1)
    temp_pt2 = convert2point(pt2)
    temp_color = convert2scalar(color)
    cv2.line(img, temp_pt1, temp_pt2, temp_color, thickness, lineType, shift)


def polylines(img, pts, isClosed, color, thickness=1, lineType=8, shift=0):
    # 判读 pts 是否为 list
    if not isinstance(pts, list):
        raise TypeError("pts must be point list")
    if len(pts) == 0:
        return img

    if not isinstance(pts[0], list):
        raise TypeError("pts[0] must be point list")
    if len(pts[0]) == 0:
        return img

    temp_pts = [convert2point(pt) for pt in pts[0]]
    temp_color = convert2scalar(color)
    cv2.polylines(img, temp_pts, isClosed, temp_color, thickness, lineType, shift)


def putText(
    img,
    text,
    org,
    fontFace,
    fontScale,
    color,
    thickness=1,
    lineType=8,
    bottomLeftOrigin=False,
):
    temp_org = convert2point(org)
    temp_color = convert2scalar(color)
    cv2.putText(
        img,
        text,
        temp_org,
        fontFace,
        fontScale,
        temp_color,
        thickness,
        lineType,
        bottomLeftOrigin,
    )


def rectangle(img, pt1, pt2, color, thickness=1, lineType=8, shift=0):
    temp_pt1 = convert2point(pt1)
    temp_pt2 = convert2point(pt2)
    temp_color = convert2scalar(color)
    cv2.rectangle(img, temp_pt1, temp_pt2, temp_color, thickness, lineType, shift)


def threshold(src, thresh, maxval, type):
    computed_threshold, dst = cv2.threshold(src, thresh, maxval, type)
    return computed_threshold, dst
def convertScaleAbs(src, alpha=1, beta=0):
    return cv2.convertScaleAbs(src, alpha, beta)


def inRange(src, lowerb, upperb):
    return cv2.inRange(src, lowerb, upperb)
def imread(filename, flags=cv2.ImreadModes.IMREAD_COLOR):
    return cv2.imread(filename, flags)


def imwrite(filename, mat):
    return cv2.imwrite(filename, mat)

```