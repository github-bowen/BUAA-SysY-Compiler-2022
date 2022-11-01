#ifndef IC_ITEM_VAR_H
#define IC_ITEM_VAR_H

#include "ICItem.h"

class ICItemVar : public ICItem {
    static int generateVarId() {
        static int i = 0;
        return ++i;
    }

    static int generateTempVarId() {
        static int i = 0;
        return ++i;
    }

public:
    const bool isTemp;
    const std::string *originalName;
    const SymbolTableEntry *symbolTableEntry;
    const bool isGlobal;
    const bool isConst;
    const int varId;
    const int tempVarId;
    const int value;

    /**
     *
     * @param isConst
     * @param isGlobal
     * @param value
     */
    ICItemVar(const std::string *originalName, SymbolTableEntry *symbolTableEntry,
              bool isConst, bool isGlobal, int value = 0)
            : ICItem(ICItemType::Var),
              isGlobal(isGlobal),
              isConst(isConst),
              varId(generateVarId()),
              tempVarId(-1),
              value(value),
              symbolTableEntry(symbolTableEntry),
              originalName(originalName),
              isTemp(false) {}

    explicit ICItemVar(bool isGlobal, int value = 0)
            : ICItem(ICItemType::Var),
              originalName(nullptr),
              symbolTableEntry(nullptr),
              isConst(false),
              isGlobal(isGlobal),
              value(value),
              varId(-1),
              tempVarId(generateTempVarId()),
              isTemp(true) {}

};

#endif //IC_ITEM_VAR_H
