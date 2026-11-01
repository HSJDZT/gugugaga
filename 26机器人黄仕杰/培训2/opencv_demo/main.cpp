#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    cv::Mat img = cv::imread("test.jpg");
    if (img.empty()) {
        std::cout << "找不到图片，请确保文件名是 test.jpg" << std::endl;
        return -1;
    }
    cv::imshow("My Image", img);
    cv::waitKey(0);
    return 0;
}