#include <iostream>
#include <string>
#include <vector>
#include <opencv2/imgcodecs.hpp> // 包含 imdecode
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

int main() {
    // 1. 准备中文路径和读取图片 
    string imagePath = u8"test2.png";
    // 尝试读取图片
    Mat img = imread(imagePath);
    
    // 如果读失败，打印错误信息并退出 
    if (img.empty()) {
        cout << "无法读取图片，请检查路径是否正确！" << endl;
        cout << "当前尝试路径: " << imagePath << endl;
        return -1;
    }

    
    Mat imgHSV;
    cvtColor(img, imgHSV, COLOR_BGR2HSV);

    // 3. 分离 H / S / V 三个通道 (便于后续对照调试)
    vector<Mat> channels;
    split(imgHSV, channels);

    // 4. 定义红色检测的阈值 (根据原视频暗度调整了 S_min 和 V_min)
    int s_min = 100; 
    int v_min = 70;  

    // 5. 核心操作：提取红色掩膜 (红色跨越了0度，必须分两段提取)
    Mat mask1, mask2, mask;
    inRange(imgHSV, Scalar(0,   s_min, v_min), Scalar(10, 255, 255), mask1);  // 提取色相 0-10 区间
    inRange(imgHSV, Scalar(170, s_min, v_min), Scalar(180, 255, 255), mask2); // 提取色相 170-180 区间
    bitwise_or(mask1, mask2, mask); // 将两段掩膜合并 (bitwise_or)

    // 6. 形态学处理：填补灯条内部的空洞
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3)); // 3x3 矩形内核
    morphologyEx(mask, mask, MORPH_CLOSE, kernel);              // 闭运算

    // 7. 制作红白叠加对照图 (red_mask_overlay)
    Mat imgOverlay;
    bitwise_and(img, img, imgOverlay, mask); // 只保留白色掩膜区域的彩色像素，其他地方变黑

    // 8. 保存输出结果到 outputs 文件夹

    imwrite("outputs/red_mask.png", mask);         // 纯黑白掩膜
    imwrite("outputs/red_mask_overlay.png", imgOverlay); // 叠加图
    imwrite("outputs/H_channel.png", channels[0]); // H 通道图

    // 9. 显示结果窗口
    imshow("Original Image", img);       // 原图
    imshow("H Channel", channels[0]);    // H 通道
    imshow("S Channel", channels[1]);    // S 通道
    imshow("V Channel", channels[2]);    // V 通道
    imshow("Red mask(inRange)", mask);  // 最终检测掩膜
    imshow("mask Overlay", imgOverlay);   // 叠加对照图

    waitKey(0); // 等待按键关闭窗口
    destroyAllWindows(); // 销毁所有窗口

    return 0;
}