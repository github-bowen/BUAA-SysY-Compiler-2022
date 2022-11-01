#ifndef IC_ITEM_CONST_NUM_H
#define IC_ITEM_CONST_NUM_H

#include "ICItem.h"

class ICItemConstNum : public ICItem {
public:
    const bool isGlobal;
    const int value;  // 立即数

    /**
     * @param value
     * @param isGlobal
     */
    explicit ICItemConstNum(int value, const bool isGlobal)
            : ICItem(ICItemType::ConstNum), value(value), isGlobal(isGlobal) {}
};

#endif //IC_ITEM_CONST_NUM_H
