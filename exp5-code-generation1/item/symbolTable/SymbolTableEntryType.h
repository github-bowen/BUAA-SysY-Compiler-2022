#ifndef THIS_PROJECT_SYMBOL_TABLE_ENTRY_TYPE_H
#define THIS_PROJECT_SYMBOL_TABLE_ENTRY_TYPE_H

enum class SymbolTableEntryType {
    Var,  // int 变量
    VarConst,  // int 常量

    Array1,  // 一维数组
    Array1Const,  // 常量一维数组
    Array2,  // 二维数组
    Array2Const,  // 常量二维数组

    FunctionOfVoid,  // 返回值是void的函数
    FunctionOfInt,   // 返回值是int的函数

    ReferencedEntry  // 临时表项，用于引用数组，如：记录函数调用中 func(arr[1][2]) 中的 arr[1][2]
};

#endif //THIS_PROJECT_SYMBOL_TABLE_ENTRY_TYPE_H
