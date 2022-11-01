#include "SymbolTableEntry.h"
#include <iostream>
#include "_debug.h"

SymbolTableEntry::SymbolTableEntry(int value) :
        isFuncFParam(false), type(SymbolTableEntryType::VarConst) {
    varConst = new VarConst(value);
    // FIXME: 实际上用于提供函数调用时传入的常数
}

SymbolTableEntry::SymbolTableEntry(SymbolTableEntry *defineEntry, SymbolTableEntryType actualType)
        : type(SymbolTableEntryType::ReferencedEntry), defLineNum(-1), isFuncFParam(false) {
    tempEntry = new ReferencedEntry(actualType, defineEntry->type);
    definedEntry = defineEntry;
}

SymbolTableEntry::SymbolTableEntry(SymbolTableEntry *defineEntry,
                                   SymbolTableEntryType actualType, int d1)
        : type(SymbolTableEntryType::ReferencedEntry), defLineNum(-1), isFuncFParam(false) {
    tempEntry = new ReferencedEntry(actualType, defineEntry->type, d1);
    definedEntry = defineEntry;
}

SymbolTableEntry::SymbolTableEntry(SymbolTableEntry *defineEntry,
                                   SymbolTableEntryType actualType, int d1, int d2)
        : type(SymbolTableEntryType::ReferencedEntry),
          defLineNum(-1), isFuncFParam(false) {
    tempEntry = new ReferencedEntry(actualType, defineEntry->type, d1, d2);
    definedEntry = defineEntry;
}

SymbolTableEntry::SymbolTableEntry(Node *node, Var *var,
                                   unsigned int defLineNum, bool isFuncFParam)
        : type(SymbolTableEntryType::Var),
          var(var), node(node), defLineNum(defLineNum), isFuncFParam(isFuncFParam) {}

SymbolTableEntry::SymbolTableEntry(Node *node, VarConst *varConst,
                                   unsigned int defLineNum, bool isFuncFParam)
        : type(SymbolTableEntryType::VarConst),
          varConst(varConst), node(node), defLineNum(defLineNum), isFuncFParam(isFuncFParam) {}

SymbolTableEntry::SymbolTableEntry(Node *node, Array1 *array1,
                                   unsigned int defLineNum, bool isFuncFParam)
        : type(SymbolTableEntryType::Array1),
          array1(array1), node(node), defLineNum(defLineNum), isFuncFParam(isFuncFParam) {}

SymbolTableEntry::SymbolTableEntry(Node *node, Array1Const *array1Const,
                                   unsigned int defLineNum, bool isFuncFParam)
        : type(SymbolTableEntryType::Array1Const),
          array1Const(array1Const), node(node), defLineNum(defLineNum), isFuncFParam(isFuncFParam) {}

SymbolTableEntry::SymbolTableEntry(Node *node, Array2 *array2,
                                   unsigned int defLineNum, bool isFuncFParam)
        : type(SymbolTableEntryType::Array2),
          array2(array2), node(node), defLineNum(defLineNum), isFuncFParam(isFuncFParam) {}

SymbolTableEntry::SymbolTableEntry(Node *node, Array2Const *array2Const,
                                   unsigned int defLineNum, bool isFuncFParam)
        : type(SymbolTableEntryType::Array2Const),
          array2Const(array2Const), node(node), defLineNum(defLineNum), isFuncFParam(isFuncFParam) {}


SymbolTableEntry::SymbolTableEntry(Node *node, FunctionOfInt *functionOfInt, unsigned int defLineNum)
        : type(SymbolTableEntryType::FunctionOfInt),
          functionOfInt(functionOfInt), node(node), defLineNum(defLineNum), isFuncFParam(false) {}

SymbolTableEntry::SymbolTableEntry(Node *node, FunctionOfVoid *functionOfVoid, unsigned int defLineNum)
        : type(SymbolTableEntryType::FunctionOfVoid),
          functionOfVoid(functionOfVoid), node(node), defLineNum(defLineNum), isFuncFParam(false) {}

std::string *SymbolTableEntry::getName() const {
    return &(node->getToken()->value);
}

bool SymbolTableEntry::isReferencedEntry() const {
    return this->type == SymbolTableEntryType::ReferencedEntry;
}

int SymbolTableEntry::funcParamsNum() const {
    if (type == SymbolTableEntryType::FunctionOfInt) {
        return functionOfInt->params->size();
    }
    return functionOfVoid->params->size();
}

std::vector<FuncParam *> *SymbolTableEntry::getFuncParams() const {
    if (type == SymbolTableEntryType::FunctionOfInt) {
        return functionOfInt->params;
    }
    return functionOfVoid->params;
}

int SymbolTableEntry::varGet() const {
    if (type == SymbolTableEntryType::Var) {
        return var->value;
    }
    if (type != SymbolTableEntryType::VarConst) {
        assert(type == SymbolTableEntryType::VarConst);
    }
    return varConst->value;
}

int SymbolTableEntry::getValueFromReferencedArray1(int d1) const {
    return definedEntry->array1get(d1);
}

int SymbolTableEntry::getValueFromReferencedArray2(int d1, int d2) const {
    return definedEntry->array2get(d1, d2);
}

int SymbolTableEntry::array1get(int index) const {
    if (type == SymbolTableEntryType::Array1) {
        return array1->values[index];
    }
    assert(type == SymbolTableEntryType::Array1Const);
    return array1Const->values[index];
}

int SymbolTableEntry::array2get(int i1, int i2) const {
    if (type == SymbolTableEntryType::Array2) {
        return 0;
//        return ((array2->values))[i1][i2];
    }
    assert(type == SymbolTableEntryType::Array2Const);
    return ((array2Const->values))[i1][i2];
}

bool SymbolTableEntry::hasSameType(SymbolTableEntry *realParam, FuncParam *funcParam) {
    if (realParam->getActualType() == SymbolTableEntryType::Var ||
        realParam->getActualType() == SymbolTableEntryType::VarConst) {  // 变量
        return funcParam->type == 0;
    }
    if (realParam->getActualType() == SymbolTableEntryType::Array1 ||
        realParam->getActualType() == SymbolTableEntryType::Array1Const) {  // 一维数组
        return funcParam->type == 1;
    }
    if (realParam->getActualType() == SymbolTableEntryType::Array2 ||
        realParam->getActualType() == SymbolTableEntryType::Array2Const) {  // 一维数组
        return funcParam->type == 2;
    }
    // 不合法情况
    return false;
}

void SymbolTableEntry::addParamForFuncEntry(SymbolTableEntry *param) {
    if (type == SymbolTableEntryType::FunctionOfInt) {
        if (param->type == SymbolTableEntryType::Var ||
            param->type == SymbolTableEntryType::VarConst) {  // 变量
            functionOfInt->addVarParam();
        }
        if (param->type == SymbolTableEntryType::Array1 ||
            param->type == SymbolTableEntryType::Array1Const) {  // 一维数组
            functionOfInt->addArray1Param();
        }
        if (param->type == SymbolTableEntryType::Array2 ||
            param->type == SymbolTableEntryType::Array2Const) {  // 一维数组
            functionOfInt->addArray2Param(param->getD2ForArray2());
        }
    } else {
        if (param->type == SymbolTableEntryType::Var ||
            param->type == SymbolTableEntryType::VarConst) {  // 变量
            functionOfVoid->addVarParam();
        }
        if (param->type == SymbolTableEntryType::Array1 ||
            param->type == SymbolTableEntryType::Array1Const) {  // 一维数组
            functionOfVoid->addArray1Param();
        }
        if (param->type == SymbolTableEntryType::Array2 ||
            param->type == SymbolTableEntryType::Array2Const) {  // 一维数组
            functionOfVoid->addArray2Param(param->getD2ForArray2());
        }
    }
}

int SymbolTableEntry::getD2ForArray2() {
    if (this->type == SymbolTableEntryType::Array2) {
        return array2->d2;
    }
    assert(type == SymbolTableEntryType::Array2Const);
    return array2Const->d2;
}

bool SymbolTableEntry::isConst() const {
    return (type == SymbolTableEntryType::VarConst ||
            type == SymbolTableEntryType::Array1Const ||
            type == SymbolTableEntryType::Array2Const) || (
                   isReferencedEntry() && definedEntry->isConst());
}

SymbolTableEntryType SymbolTableEntry::getActualType() const {
    if (isReferencedEntry()) {
        return tempEntry->actualType;
    }
    return type;
}

int *SymbolTableEntry::array1ConstGetAll() const {
    assert(type == SymbolTableEntryType::Array1Const);
    return array1Const->values;
}

int **SymbolTableEntry::array2ConstGetAll() const {
    assert(type == SymbolTableEntryType::Array2Const);
    return array2Const->values;
}

SymbolTableEntry::~SymbolTableEntry() {
    delete node;
    delete var;
    delete varConst;
    delete array1;
    delete array1Const;
    delete array2;
    delete array2Const;
    delete functionOfInt;
    delete functionOfVoid;
    delete tempEntry;
    delete definedEntry;
}
