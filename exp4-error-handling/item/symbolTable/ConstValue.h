#ifndef THIS_PROJECT_CONST_VALUE_H
#define THIS_PROJECT_CONST_VALUE_H

#include <vector>

union Value {
    int var;
    int *array1;
    int **array2;
};

class ConstValue {
public:
    int dimension;  // 0 1 2
    int d1;
    int d2;
    Value value;
};

#endif //THIS_PROJECT_CONST_VALUE_H
