#include <opencv2/opencv.hpp>
#include <algorithm>
#include <iostream>
#include <vector>
using namespace cv;
using namespace std;
struct Bar { RotatedRect rr; Point2f p[4]; float len, wid; Point2f c; };
static Mat redMask(const Mat& bgr) {
    Mat hsv, m1, m2;
    cvtColor(bgr, hsv, COLOR_BGR2HSV);
    inRange(hsv, Scalar(0, 100, 90), Scalar(12, 255, 255), m1);
    inRange(hsv, Scalar(158, 100, 90), Scalar(180, 255, 255), m2);
    Mat mask = m1 | m2;
    Mat k = getStructuringElement(MORPH_RECT, Size(3, 3));
    morphologyEx(mask, mask, MORPH_CLOSE, k);
    morphologyEx(mask, mask, MORPH_OPEN, k);
    return mask;
}
static void sortBar(Bar& b) {
    vector<Point2f> v(b.p, b.p + 4);
    sort(v.begin(), v.end(), [](const Point2f& a, const Point2f& c){ return a.y < c.y; });
    Point2f t0 = v[0], t1 = v[1], b0 = v[2], b1 = v[3];
    if (t0.x > t1.x) swap(t0, t1);
    if (b0.x > b1.x) swap(b0, b1);
    b.p[0] = t0; b.p[1] = t1; b.p[2] = b1; b.p[3] = b0;
}
static vector<Bar> findBars(const Mat& mask, Size sz) {
    vector<vector<Point>> cs;
    findContours(mask, cs, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    vector<Bar> out;
    float minA = sz.width * sz.height * 0.00005f;
    for (auto& c : cs) {
        if (contourArea(c) < minA) continue;
        Bar b; b.rr = minAreaRect(c);
        float w = b.rr.size.width, h = b.rr.size.height;
        b.len = max(w, h); b.wid = min(w, h);
        if (b.wid < 1) continue;
        if (b.len / b.wid < 1.8f) continue;
        if (b.len < sz.height * 0.03f) continue;
        b.c = b.rr.center;
        b.rr.points(b.p);
        sortBar(b);
        out.push_back(b);
    }
    return out;
}
static bool ok(const Bar& a, const Bar& b) {
    if (max(a.len, b.len) / min(a.len, b.len) > 1.6f) return false;
    float dx = fabs(b.c.x - a.c.x), dy = fabs(b.c.y - a.c.y);
    if (dx < max(a.wid, b.wid) * 1.2f) return false;
    if (dx > min(a.len, b.len) * 2.5f) return false;
    if (dy > min(a.len, b.len) * 0.45f) return false;
    return true;
}
int main(int argc, char** argv) {
    string in = argc > 1 ? argv[1] : "test.mp4";
    string out = argc > 2 ? argv[2] : "out.mp4";
    bool show = argc > 3;
    VideoCapture cap(in);
    if (!cap.isOpened()) { cout << "open fail" << endl; return -1; }
    int W = cap.get(CAP_PROP_FRAME_WIDTH), H = cap.get(CAP_PROP_FRAME_HEIGHT);
    double fps = cap.get(CAP_PROP_FPS);
    if (fps < 1 || fps > 240) fps = 30;
    VideoWriter w(out, 0x7634706D, fps, Size(W, H));
    Mat f;
    int n = 0, hits = 0;
    vector<Mat> sheet;
    int total = (int)cap.get(CAP_PROP_FRAME_COUNT);
    int step = total > 12 ? total / 12 : 25;
    while (cap.read(f)) {
        Mat mask = redMask(f);
        vector<Bar> bars = findBars(mask, f.size());
        for (auto& b : bars) {
            for (int i = 0; i < 4; i++) line(f, b.p[i], b.p[(i+1)%4], Scalar(0,255,255), 2);
            for (int i = 0; i < 4; i++) circle(f, b.p[i], 3, Scalar(0,255,0), -1);
        }
        int bi = -1, bj = -1; float best = 1e9f;
        for (int i = 0; i < (int)bars.size(); i++)
            for (int j = i+1; j < (int)bars.size(); j++) {
                if (!ok(bars[i], bars[j])) continue;
                float s = fabs(bars[i].c.y - bars[j].c.y) + fabs(bars[i].len - bars[j].len) * 0.3f;
                if (s < best) { best = s; bi = i; bj = j; }
            }
        if (bi >= 0) {
            const Bar& A = bars[bi].c.x < bars[bj].c.x ? bars[bi] : bars[bj];
            const Bar& B = bars[bi].c.x < bars[bj].c.x ? bars[bj] : bars[bi];
            Point2f P[4];
            P[0] = A.p[0];
            P[1] = B.p[1];
            P[2] = B.p[2];
            P[3] = A.p[3];
            for (int i = 0; i < 4; i++) line(f, P[i], P[(i+1)%4], Scalar(0,255,0), 2);
            for (int i = 0; i < 4; i++) {
                circle(f, P[i], 6, Scalar(0,255,0), 2);
                char t[8]; snprintf(t, sizeof(t), "P%d", i + 1);
                putText(f, t, P[i] + Point2f(8, -8), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0,255,0), 2);
            }
            hits++;
        }
        if (n % step == 0 && (int)sheet.size() < 12) {
            Mat t; resize(f, t, Size(320, 240)); sheet.push_back(t);
        }
        w.write(f);
        n++;
        if (show) { imshow("detect_demo", f); if (waitKey(1) == 27) break; }
    }
    Mat g(720, 1280, CV_8UC3, Scalar(0, 0, 0));
    for (size_t i = 0; i < sheet.size(); i++) {
        int x = (int)(i % 4) * 320, y = (int)(i / 4) * 240;
        sheet[i].copyTo(g(Rect(x, y, 320, 240)));
    }
    imwrite("sheet.jpg", g);
    cout << "frames " << n << " hits " << hits << endl;
    return 0;
}
