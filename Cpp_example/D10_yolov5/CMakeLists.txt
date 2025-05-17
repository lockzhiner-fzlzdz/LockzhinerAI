# CMake最低版本要求  
cmake_minimum_required(VERSION 3.10)  

project(test-find-blobs)

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

set(RKNPU2_BACKEND_BASE_DIR "${LockzhinerVisionModule_ROOT_PATH}/include/lockzhiner_vision_module/vision/deep_learning/runtime")
if(NOT EXISTS ${RKNPU2_BACKEND_BASE_DIR})
    message(FATAL_ERROR "RKNPU2 backend base dir missing: ${RKNPU2_BACKEND_BASE_DIR}")
endif()

add_executable(yolov5_main
    main.cc
    postprocess.cc
    yolov5.cc
    yolov5.h  
    postprocess.h
)
target_include_directories(yolov5_main PRIVATE ${LOCKZHINER_VISION_MODULE_INCLUDE_DIRS} ${rknpu2_INCLUDE_DIRS} ${RKNPU2_BACKEND_BASE_DIR})
target_link_libraries(yolov5_main PRIVATE ${OPENCV_LIBRARIES} ${LOCKZHINER_VISION_MODULE_LIBRARIES})

install(
    TARGETS yolov5_main
    RUNTIME DESTINATION .  
)