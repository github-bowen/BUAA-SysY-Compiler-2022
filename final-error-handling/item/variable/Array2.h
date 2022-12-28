#ifndef THIS_PROJECT_ARRAY2_H
#define THIS_PROJECT_ARRAY2_H

#include <vector>

class Array2 {
public:
    const int d1;  // -1 表示是函数形参
    const int d2;
    int **values;

    Array2(int d1, int d2) : d1(d1), d2(d2) {
        values = new int *[d1 + 1];
        for (int i = 0; i < d1 + 1; i++) {
            values[i] = new int[d2 + 1]();
        }
    }

    ~Array2() {
        for (int i = 0; i < d1 + 1; i++) {
            delete[] values[i];
        }
        delete[] values;
        values = nullptr;
    }
};

#endif //THIS_PROJECT_ARRAY2_H
