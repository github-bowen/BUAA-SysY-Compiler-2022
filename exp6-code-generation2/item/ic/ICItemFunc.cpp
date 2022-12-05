#include "ICItemFunc.h"
#include "ICItemVar.h"
#include "ICItemArray.h"


ICItemFunc::ICItemFunc(SymbolTableEntry *funcEntry) :
        ICItem(ICItemType::Func),
        paramNum(funcEntry->funcParamsNum()),
        funcLabel(new ICItemLabel()),
        originName(funcEntry->getName()),
        params(new std::vector<ICItem *>),
        entries(new std::vector<ICEntry *>){

    if (funcEntry->type == SymbolTableEntryType::FunctionOfInt) {
        hasReturnType = true;
    } else {
        hasReturnType = false;
    }

    for (const auto *item: *funcEntry->getFuncParams()) {
        if (item->type == 0) {
            auto *icItemVar = new ICItemVar(item->name, nullptr, false, false);
            params->push_back(icItemVar);
        } else {
            auto *icItemArray = new ICItemArray(item->name, nullptr, false, false);
            icItemArray->originType.d = item->type;
            icItemArray->originType.length2 = item->d2;
            params->push_back(icItemArray);
        }
    }
}
