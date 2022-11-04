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

    const ICItem *operator1;
    const ICItem *operator2;
    const ICItem *operator3;
    const int opNum;
    const std::vector<ICItem *> *params;


    ICEntry(ICEntryType type, ICItem *operator1);

    ICEntry(ICEntryType type, ICItem *operator1, ICItem *operator2);
};

#endif //IC_ENTRY_H
