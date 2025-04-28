# CMake最低版本要求  
cmake_minimum_required(VERSION 3.10)  

project(test_qr_code_detector)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 定义项目根目录路径
set(PROJECT_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../..")
message("PROJECT_ROOT_PATH = " ${PROJECT_ROOT_PATH})

include("${PROJECT_ROOT_PATH}/toolchains/arm-rockchip830-linux-uclibcgnueabihf.toolchain.cmake")

# 定义 OpenCV SDK 路径
set(OpenCV_ROOT_PATH "${PROJECT_ROOT_PATH}/third_party/opencv-mobile-4.10.0-lockzhiner-vision-module")
set(OpenCV_DIR "${OpenCV_ROOT_PATH}/lib/cmake/opencv4")
find_package(OpenCV REQUIRED)
set(OPENCV_LIBRARIES "${OpenCV_LIBS}")

# 定义 LockzhinerVisionModule SDK 路径
set(LockzhinerVisionModule_ROOT_PATH "${PROJECT_ROOT_PATH}/third_party/lockzhiner_vision_module_sdk")
set(LockzhinerVisionModule_DIR "${LockzhinerVisionModule_ROOT_PATH}/lib/cmake/lockzhiner_vision_module")
find_package(LockzhinerVisionModule REQUIRED)

# 定义 ZXing SDK 路径
set(ZXing_ROOT_PATH "${PROJECT_ROOT_PATH}/third_party/zxing-cpp-v2.2.1-lockzhiner-vision-module")
set(ZXing_DIR "${ZXing_ROOT_PATH}/lib/cmake/ZXing")
set(ZXing_INCLUDE_DIRS "${ZXing_ROOT_PATH}/include")
find_package(ZXing REQUIRED)
set(ZXing_LIBRARIES "${ZXing_LIBS}")

# 基本图像处理示例
add_executable(Test-qr_code-detector test_qr_code_detector.cc)
target_include_directories(Test-qr_code-detector PRIVATE
    ${ZXing_INCLUDE_DIRS}          
    ${LOCKZHINER_VISION_MODULE_INCLUDE_DIRS}
)
target_link_libraries(Test-qr_code-detector PRIVATE ${OPENCV_LIBRARIES} ${LOCKZHINER_VISION_MODULE_LIBRARIES} ${ZXing_LIBRARIES} )
install(
    TARGETS Test-qr_code-detector
    RUNTIME DESTINATION .  
)