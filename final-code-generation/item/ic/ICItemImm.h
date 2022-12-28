#ifndef IC_ITEM_CONST_NUM_H
#define IC_ITEM_CONST_NUM_H

#include "ICItem.h"

class ICItemImm : public ICItem {
public:
    int value;  // 立即数

    explicit ICItemImm(int value = 0) : ICItem(ICItemType::Imm), value(value) {}
};

#endif //IC_ITEM_CONST_NUM_H
