#include "ICItemFunc.h"
#include "ICItemVar.h"


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

    // TODO: 假设没数组
    for (const auto *item: *funcEntry->getFuncParams()) {
        auto * icItemVar = new ICItemVar(item->name, nullptr, false, false);
        params->push_back(icItemVar);
    }
}
