#include "ICEntry.h"

#include "_debug.h"
#include "ICItemVar.h"
#include "ICItemArray.h"

ICEntry::ICEntry(ICEntryType type, ICItem *operator1) :
        entryType(type),
        calledFunc(nullptr),
        operator1(operator1),
        operator2(nullptr),
        operator3(nullptr),
        params(nullptr),
        opNum(1) {}

ICEntry::ICEntry(ICEntryType type, ICItem *operator1, ICItem *operator2) :
        entryType(type),
        calledFunc(nullptr),
        operator1(operator1),
        operator2(operator2),
        operator3(nullptr),
        params(nullptr),
        opNum(2) {}

ICEntry::ICEntry(ICEntryType type, ICItem *operator1, ICItem *operator2, ICItem *operator3) :
        entryType(type),
        calledFunc(nullptr),
        operator1(operator1),
        operator2(operator2),
        operator3(operator3),
        params(nullptr),
        opNum(3) {}

ICEntry::ICEntry(ICEntryType type) :
        entryType(type),
        calledFunc(nullptr),
        operator1(nullptr),
        operator2(nullptr),
        operator3(nullptr),
        params(nullptr),
        opNum(0) {}

ICEntry::ICEntry(ICItem *calledFunc, std::vector<ICItem *> *params) :
        entryType(ICEntryType::FuncCall),
        calledFunc(calledFunc),
        operator1(nullptr),
        operator2(nullptr),
        operator3(nullptr),
        params(params),
        opNum(1) {}

bool ICEntry::isVarOrConstDef() const {
    return (entryType == ICEntryType::VarDefine ||
            entryType == ICEntryType::ConstVarDefine ||
            entryType == ICEntryType::ArrayDefine ||
            entryType == ICEntryType::ConstArrayDefine);

}

bool ICEntry::isGlobalVarOrConstDef() const {
    if (entryType == ICEntryType::VarDefine || entryType == ICEntryType::ConstVarDefine) {
        auto *var = ((ICItemVar *) operator1);
        return var->isGlobal;
    }
    if (entryType == ICEntryType::ArrayDefine || entryType == ICEntryType::ConstArrayDefine) {
        auto *array = ((ICItemArray *) operator1);
        return array->isGlobal;
    }
    return false;
}
