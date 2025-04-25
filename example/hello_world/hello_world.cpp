#include <iostream>
#include <opencv2/core/version.hpp>

int main() {
    std::cout << "OpenCV version: "
              << CV_VERSION_MAJOR << "."
              << CV_VERSION_MINOR << "."
              << CV_VERSION_REVISION << std::endl;
    return 0;
}