#include <opencv2/opencv.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
    const Size boardSize(10, 7);      
    const float squareSize = 0.015f;  

    string imgDir = (argc > 1) ? argv[1] : "";
    if (imgDir.empty()) {
        const char* cands[] = {"images", "../image", "../images", "/home/hsj/cam_mix_img"};
        for (const char* c : cands) {
            vector<String> t;
            try { glob(string(c) + "/*.jpg", t, false); } catch (cv::Exception&) {}
            if (!t.empty()) { imgDir = c; break; }
        }
        if (imgDir.empty()) imgDir = "images";
    }

   
    string outDir = (argc > 2) ? argv[2] : "";
    if (outDir.empty()) {
        ifstream up("../main.cpp");
        outDir = up.good() ? ".." : ".";
    }

    
    vector<Point3f> modelPoints;
    for (int r = 0; r < boardSize.height; ++r)
        for (int c = 0; c < boardSize.width; ++c)
            modelPoints.push_back(Point3f(c * squareSize, r * squareSize, 0.0f));

   
    vector<String> files;
    try {
        glob(imgDir + "/*.jpg", files, false);
        if (files.empty()) glob(imgDir + "/*.png", files, false);
    } catch (cv::Exception&) {
        cout << "图片目录打不开：" << imgDir << endl;
        return -1;
    }
    if (files.empty()) {
        cout << "目录里没有图片：" << imgDir << endl;
        cout << "用法：./calib_demo <图片目录> [输出目录]" << endl;
        cout << "  例：./calib_demo /home/hsj/cam_mix_img .." << endl;
        return -1;
    }
    cout << "图片目录：" << imgDir << "，共 " << files.size() << " 张" << endl;

    // 3) 逐张找角点
    vector<vector<Point3f>> objectPoints;
    vector<vector<Point2f>> imagePoints;
    vector<String> goodFiles;      // 成功检出的图，后面用来挑图做矫正
    Size imageSize;

    for (size_t i = 0; i < files.size(); ++i) {
        Mat img = imread(files[i]);
        if (img.empty()) continue;
        if (imageSize.area() == 0) imageSize = img.size();

        Mat gray;
        cvtColor(img, gray, COLOR_BGR2GRAY);

        vector<Point2f> corners;
        bool found = findChessboardCorners(
            gray, boardSize, corners,
            CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE);

        if (!found) {
            cout << "  [跳过] 没检出角点：" << files[i] << endl;
            continue;
        }

        // 亚像素精修，误差能小一大截
        cornerSubPix(gray, corners, Size(11, 11), Size(-1, -1),
                     TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.001));

        objectPoints.push_back(modelPoints);
        imagePoints.push_back(corners);
        goodFiles.push_back(files[i]);
    }

    cout << "实际检出角点：" << goodFiles.size() << " / " << files.size() << " 张" << endl;
    if (goodFiles.size() < 5) {
        cout << "有效图片太少，无法标定" << endl;
        return -1;
    }

    // 4) 标定：解出内参 K 和畸变 D
    Mat K, D;
    vector<Mat> rvecs, tvecs;
    double rms = calibrateCamera(objectPoints, imagePoints, imageSize,
                                 K, D, rvecs, tvecs);

    cout << endl;
    cout << "重投影误差 rms = " << rms << " 像素（1~2 属正常）" << endl;
    cout << "内参矩阵 K = " << endl << K << endl;
    cout << "畸变系数 D = " << endl << D << endl;
    cout << "  fx=" << K.at<double>(0, 0) << "  fy=" << K.at<double>(1, 1) << endl;
    cout << "  cx=" << K.at<double>(0, 2) << "  cy=" << K.at<double>(1, 2) << endl;

    // 5) 存内参文件
    FileStorage fs(outDir + "/calib.yml", FileStorage::WRITE);
    fs << "image_width" << imageSize.width;
    fs << "image_height" << imageSize.height;
    fs << "camera_matrix" << K;
    fs << "dist_coeffs" << D;
    fs << "rms" << rms;
    fs.release();
    cout << "已保存 " << outDir << "/calib.yml" << endl;

    // 6) 用最中间那张图做矫正，前后并排存一张对比图
    string pick = goodFiles[goodFiles.size() / 2];
    Mat src = imread(pick);
    Mat dst;
    undistort(src, dst, K, D);

    imwrite(outDir + "/undistort_result.jpg", dst);
    cout << "已保存 " << outDir << "/undistort_result.jpg（原图 " << pick << "）" << endl;

    // 中间画一条分隔线，方便看前后差别
    Mat sep(src.rows, 4, src.type(), Scalar(0, 0, 255));
    Mat compare;
    hconcat(src, sep, compare);
    hconcat(compare, dst, compare);
    imwrite(outDir + "/undistort_compare.jpg", compare);
    cout << "已保存 " << outDir << "/undistort_compare.jpg（左：原图　右：矫正后）" << endl;

    return 0;
}
