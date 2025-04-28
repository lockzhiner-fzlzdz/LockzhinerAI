cmake_minimum_required(VERSION 3.10)
project(test_classification)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../..")
include("${PROJECT_ROOT_PATH}/toolchains/arm-rockchip830-linux-uclibcgnueabihf.toolchain.cmake")

set(OpenCV_ROOT_PATH "${PROJECT_ROOT_PATH}/third_party/opencv-mobile-4.10.0-lockzhiner-vision-module")
set(OpenCV_DIR "${OpenCV_ROOT_PATH}/lib/cmake/opencv4")
find_package(OpenCV REQUIRED)

# Define NCNN paths
set(THIRD_PARTY_ROOT_PATH "${PROJECT_ROOT_PATH}/third_party")
set(NCNN_ROOT_DIR "${THIRD_PARTY_ROOT_PATH}/ncnn-20240820-lockzhiner-vision-module")
set(NCNN_INCLUDE_DIRS "${NCNN_ROOT_DIR}/include")
set(NCNN_LIBRARIES "${NCNN_ROOT_DIR}/lib/libncnn.a")

set(LockzhinerVisionModule_ROOT_PATH "${PROJECT_ROOT_PATH}/third_party/lockzhiner_vision_module_sdk")
set(LockzhinerVisionModule_DIR "${LockzhinerVisionModule_ROOT_PATH}/lib/cmake/lockzhiner_vision_module")
find_package(LockzhinerVisionModule REQUIRED)

add_executable(Test-ncnn-ocr ncnn_ocr.cc)

target_include_directories(Test-ncnn-ocr PUBLIC
    ${LockzhinerVisionModule_ROOT_PATH}/include
    ${OpenCV_INCLUDE_DIRS}
    ${NCNN_INCLUDE_DIRS}
    ${NCNN_ROOT_DIR} 
)

target_link_libraries(Test-ncnn-ocr PUBLIC
    ${OpenCV_LIBS}
    ${LOCKZHINER_VISION_MODULE_LIBRARIES}
    ${NCNN_LIBRARIES}
    stdc++fs
)

install(TARGETS Test-ncnn-ocr RUNTIME DESTINATION .)