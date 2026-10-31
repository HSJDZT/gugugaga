#include <iostream>
using namespace std;
#include "student.hpp"

int main() {
    // 1. 创建一个学生
    Student stu("张三", "20260101", 85.5);
    cout << "--- 初始信息 ---" << endl;
    stu.printInfo();

    // 2. 修改成绩
    cout << "--- 修改成绩 ---" << endl;
    stu.setScore(92.0);
    stu.printInfo();

    // 3. 测试非法成绩
    cout << "--- 非法成绩测试 ---" << endl;
    stu.setScore(150.0);
    stu.printInfo();

    // 4. 读取属性
    cout << "--- 单独读取 ---" << endl;
    cout << "姓名: " << stu.getName()
         << ", 成绩: " << stu.getScore() << endl;

    return 0;
}
