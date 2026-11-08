#ifndef STUDENT_HPP
#define STUDENT_HPP
using namespace std;
#include <iostream>
#include <string>

class Student {
public:
    // 构造函数：姓名、学号、成绩
    Student(const string &name = "",
            const string &id = "",
            double score = 0.0);

    // 打印学生信息
    void printInfo() const;

    // 修改成绩（限制 0~100，非法值不修改）
    void setScore(double score);

    // 读取属性
    double getScore() const;
    const string &getName() const;
    const string &getId() const;

private:
    string name_;
    string id_;
    double score_;
};

#endif // STUDENT_HPP
