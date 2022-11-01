#ifndef IC_ENTRY_TYPE_H
#define IC_ENTRY_TYPE_H

enum class ICEntryType {
    VarDefine,
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
    Mod
};

#endif //IC_ENTRY_TYPE_H
