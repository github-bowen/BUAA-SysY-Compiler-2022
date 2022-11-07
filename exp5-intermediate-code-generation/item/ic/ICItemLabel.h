#ifndef IC_ITEM_LABEL_H
#define IC_ITEM_LABEL_H

#include "ICItem.h"

class ICItemLabel : public ICItem {
    static int generateLabelId() {
        static int i = 0;
        return ++i;
    }

public:
    const int labelId;

    ICItemLabel() : ICItem(ICItemType::Label), labelId(generateLabelId()) {}

    std::string toString() const {
        return "Label_" + std::to_string(labelId);
    }

};

#endif //IC_ITEM_LABEL_H
