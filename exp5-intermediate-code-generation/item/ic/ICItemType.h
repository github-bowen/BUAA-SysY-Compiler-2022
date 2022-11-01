#ifndef IC_ITEM_TYPE_H
#define IC_ITEM_TYPE_H

enum class ICItemType {
    String,  // 字符串，包括若干个纯字符串和引用的数值 (字符和数字输出方式不同)
    Imm,  // 常量数字 (立即数)
    Label,  // 函数、数组、循环跳转、字符串存储等用到的标签 (用于挑战到函数入口)
    Array,
    Var,
    Func
};

#endif //IC_ITEM_TYPE_H
