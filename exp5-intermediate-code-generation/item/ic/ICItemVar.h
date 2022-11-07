#ifndef IC_ITEM_VAR_H
#define IC_ITEM_VAR_H

#include "ICItem.h"
#include "item/symbolTable/SymbolTableEntry.h"

class ICItemVar : public ICItem {
    static int generateVarId();

    static int generateTempVarId();

public:
    const bool isTemp;
    const int tempVarId;

    const std::string *originalName;
    const SymbolTableEntry *symbolTableEntry;
    const bool isGlobal;
    bool isConst;
    const int varId;
    int value;

    ICItemVar(const std::string *originalName, SymbolTableEntry *symbolTableEntry,
              bool isConst, bool isGlobal, int value = 0);

    explicit ICItemVar(bool isGlobal, int value = 0);

    std::string toString() const;

};

#endif //IC_ITEM_VAR_H
