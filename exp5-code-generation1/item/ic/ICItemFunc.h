#ifndef IC_ITEM_FUNC_H
#define IC_ITEM_FUNC_H

#include "ICItem.h"
#include "ICItemLabel.h"
#include "ICEntry.h"
#include "item/symbolTable/SymbolTableEntry.h"
#include <vector>

class ICItemFunc : public ICItem{
public:
    ICItemLabel *funcLabel;
    std::string *originName;
    bool hasReturnType;
    std::vector<ICItem *> *params;
    std::vector<ICEntry *> *entries;
    const int paramNum;

    explicit ICItemFunc(SymbolTableEntry *funcEntry);
};

#endif //IC_ITEM_FUNC_H
