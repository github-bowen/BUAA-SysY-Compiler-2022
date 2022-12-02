#ifndef IC_ITEM_LABEL_H
#define IC_ITEM_LABEL_H

#include "ICItem.h"

/**
 * 用途：
 * 1: 在 ICItemFunc 中作为函数的 Label
 * 2: if-else / while 中，作为 ICEntryType::Beq 的跳转 Label
 */
class ICItemLabel : public ICItem {
    static int generateLabelId() {
        static int i = 0;
        return ++i;
    }

public:
    const int labelId;

    ICItemLabel() : ICItem(ICItemType::Label), labelId(generateLabelId()) {}

    std::string toString() const {
        return "Label_" + std::to_string(abs(labelId));
    }

};

#endif //IC_ITEM_LABEL_H
