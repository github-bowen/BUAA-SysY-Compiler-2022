#ifndef IC_ITEM_VAR_H
#define IC_ITEM_VAR_H

#include "ICItem.h"

class ICItemVar : public ICItem {
    static int generateVarId() {
        static int i = 0;
        return ++i;
    }

public:
    const bool isGlobal;
    const int varId;

    explicit ICItemVar(bool isGlobal)
            : ICItem(ICItemType::Var), isGlobal(isGlobal), varId(generateVarId()) {}

};

#endif //IC_ITEM_VAR_H
