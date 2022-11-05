#ifndef IC_ENTRY_TYPE_H
#define IC_ENTRY_TYPE_H

enum class ICEntryType {
    // 第一次代码生成作业:
    VarDefine,
    ConstVarDefine,

    ArrayDefine,
    ConstArrayDefine,

    FuncDefine,
    FuncCall,

    Getint,
    PrintInt,
    PrintStr,
    Assign,
    Add,
    Sub,
    Mul,
    Div,
    Mod,
    Not,
    Neg,
    ArrayGet,
    // 第二次代码生成作业:
};

#endif //IC_ENTRY_TYPE_H
