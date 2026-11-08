#include "student.hpp"
using namespace std;


Student::Student(const string &name,
                 const string &id,
                 double score)
    : name_(name), id_(id), score_(score) {}

void Student::printInfo() const {
    cout << "姓名: " << name_
              << " | 学号: " << id_
              << " | 成绩: " << score_ << endl;
}

void Student ::setScore(double score) {
    if (score < 0.0 || score > 100.0) {
        cout << "[错误] 成绩必须在 0~100 之间，未做修改。" << endl;
        return;
    }
    score_ = score;
    cout << "[修改] 成绩已更新为 " << score_ << endl;
}

double Student::getScore() const { return score_; }

const string &Student::getName() const { return name_; }

const string &Student::getId() const { return id_; }
