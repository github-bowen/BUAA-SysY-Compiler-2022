#ifndef THIS_PROJECT_ARRAY1_H
#define THIS_PROJECT_ARRAY1_H

#include <vector>

class Array1 {
public:
    int d;  // 维度, = -1 表示是函数形参
    int *values;

    explicit Array1(int d) : d(d) {
        values = new int[d + 1];
    }

    ~Array1() {
        delete[] values;
        delete values;
        values = nullptr;
    }
};

#endif //THIS_PROJECT_ARRAY1_H
