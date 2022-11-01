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
    const std::vector<ICItem *> *params;

    /**
     *
     * @param type
     * @param operator1
     * @param operator2
     * @param operator3
     * @param params
     */
    ICEntry(ICEntryType type, ICItem *operator1, ICItem *operator2 = nullptr,
            ICItem *operator3 = nullptr)
            : entryType(type), operator1(operator1), operator2(operator2),
              operator3(operator3), params(nullptr) {}
};

#endif //IC_ENTRY_H
