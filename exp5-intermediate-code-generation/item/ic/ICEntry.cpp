#include "ICEntry.h"

ICEntry::ICEntry(ICEntryType type, ICItem *operator1) :
        entryType(type),
        operator1(operator1),
        operator2(nullptr),
        operator3(nullptr),
        params(nullptr),
        opNum(1) {}

ICEntry::ICEntry(ICEntryType type, ICItem *operator1, ICItem *operator2) :
        entryType(type),
        operator1(operator1),
        operator2(operator2),
        operator3(nullptr),
        params(nullptr),
        opNum(1) {}
