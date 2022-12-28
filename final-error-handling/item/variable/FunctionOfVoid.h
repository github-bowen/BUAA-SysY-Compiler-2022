#ifndef THIS_PROJECT_FUNCTION_OF_VOID_H
#define THIS_PROJECT_FUNCTION_OF_VOID_H

#include "item/variable/FuncParam.h"
#include <vector>

/*
 * 当 FuncFParam 为数组定义时，其第一维的长度省去（用方括号[ ]表示），而后面的各维则需要用
 * 表达式指明长度，长度是常量。
 */

/*
 * 三种类型参数 param: func(int param, int param[], int param[][3])
 */

class FunctionOfVoid {
public:
    std::vector<FuncParam *>* params;

    FunctionOfVoid() {
        params = new std::vector<FuncParam *>();
    }

    ~FunctionOfVoid() {
        delete params;
    }

    void addVarParam() {
        auto *param = new FuncParam(0);
        params->push_back(param);
    }

    void addArray1Param() {
        auto *param = new FuncParam(1);
        params->push_back(param);
    }

    void addArray2Param(int d2) {
        auto *param = new FuncParam(2, d2);
        params->push_back(param);
    }
};

#endif //THIS_PROJECT_FUNCTION_OF_VOID_H
