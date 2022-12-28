#ifndef THIS_PROJECT_FUNC_PARAM_H
#define THIS_PROJECT_FUNC_PARAM_H

class FuncParam {
public:
    const int type;  // eg  0: int, 1: int[], 2:int[][4]
    int d2;  // 如果type = 2

    explicit FuncParam(int type, int d2 = 0) : type(type), d2(d2) {}
};

#endif //THIS_PROJECT_FUNC_PARAM_H
