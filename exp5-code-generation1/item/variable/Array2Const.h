#ifndef THIS_PROJECT_ARRAY2_CONST_H
#define THIS_PROJECT_ARRAY2_CONST_H

#include <vector>

class Array2Const {
public:
    const int d1;
    const int d2;
    int **values;

    Array2Const(int d1, int d2, int **values)
            : d1(d1), d2(d2), values(values) {}

    ~Array2Const() {
        for (int i = 0; i < d1 + 1; i++) {
            delete[] values[i];
        }
        delete[] values;
        values = nullptr;
    }
};

#endif //THIS_PROJECT_ARRAY2_CONST_H
