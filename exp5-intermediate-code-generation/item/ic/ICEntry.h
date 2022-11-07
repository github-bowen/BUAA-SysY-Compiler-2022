#ifndef IC_ENTRY_H
#define IC_ENTRY_H

#include <vector>
#include "ICEntryType.h"
#include "ICItem.h"

/**
 * 一条中间代码
 */
class ICEntry {
public:
    const ICEntryType entryType;

    ICItem *operator1;
    ICItem *operator2;
    ICItem *operator3;
    const ICItem *calledFunc;  // 仅用于函数调用
    const int opNum;
    const std::vector<ICItem *> *params;

    explicit ICEntry(ICEntryType type);

    ICEntry(ICEntryType type, ICItem *operator1);

    ICEntry(ICItem *calledFunc, std::vector<ICItem *> *params = nullptr);

    ICEntry(ICEntryType type, ICItem *operator1, ICItem *operator2);

    ICEntry(ICEntryType type, ICItem *operator1, ICItem *operator2, ICItem *operator3);

    bool isVarOrConstDef() const;

    bool isGlobalVarOrConstDef() const;
};

#endif //IC_ENTRY_H
