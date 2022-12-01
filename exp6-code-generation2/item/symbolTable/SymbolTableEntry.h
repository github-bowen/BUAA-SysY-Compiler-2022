#ifndef THIS_PROJECT_SYMBOL_TABLE_ENTRY_H
#define THIS_PROJECT_SYMBOL_TABLE_ENTRY_H

#include <string>
#include <vector>
#include "SymbolTableEntryType.h"

#include "item/variable/Var.h"
#include "item/variable/VarConst.h"
#include "item/variable/Array1.h"
#include "item/variable/Array2.h"
#include "item/variable/Array1Const.h"
#include "item/variable/Array2Const.h"
#include "item/variable/FunctionOfInt.h"
#include "item/variable/FunctionOfVoid.h"
#include "item/variable/FuncParam.h"
#include "item/symbolTable/ReferencedEntry.h"
#include "tree/Node.h"

/**
 * 符号表对应的表项
 * 区别于Symbol.h!
 */

class SymbolTableEntry {
public:
    const SymbolTableEntryType type;
    explicit SymbolTableEntry(int value);

    ~SymbolTableEntry();

    SymbolTableEntry(SymbolTableEntry *defineEntry, SymbolTableEntryType actualType, int d1);

    SymbolTableEntry(SymbolTableEntry *defineEntry, SymbolTableEntryType actualType);

    SymbolTableEntry(SymbolTableEntry *defineEntry, SymbolTableEntryType actualType, int d1, int d2);

    SymbolTableEntry(Node *node, Var *var, unsigned int defLineNum, bool isFuncFParam);

    SymbolTableEntry(Node *node, VarConst *varConst, unsigned int defLineNum, bool isFuncFParam);

    SymbolTableEntry(Node *node, Array1 *array1, unsigned int defLineNum, bool isFuncFParam);

    SymbolTableEntry(Node *node, Array1Const *array1Const, unsigned int defLineNum, bool isFuncFParam);

    SymbolTableEntry(Node *node, Array2 *array2, unsigned int defLineNum, bool isFuncFParam);

    SymbolTableEntry(Node *node, Array2Const *array2Const, unsigned int defLineNum, bool isFuncFParam);

    SymbolTableEntry(Node *node, FunctionOfInt *functionOfInt, unsigned int defLineNum);

    SymbolTableEntry(Node *node, FunctionOfVoid *functionOfVoid, unsigned int defLineNum);

    Node *node;
    const bool isFuncFParam;  // 函数的形参
    unsigned int defLineNum;

    Var *var{nullptr};
    VarConst *varConst{nullptr};

    Array1 *array1{nullptr};
    Array1Const *array1Const{nullptr};
    Array2 *array2{nullptr};
    Array2Const *array2Const{nullptr};

    FunctionOfInt *functionOfInt{nullptr};
    FunctionOfVoid *functionOfVoid{nullptr};

    // 引用表项
    ReferencedEntry *tempEntry{nullptr};  // 该类型不应该保存到符号表 !!!
    // 其对应的真正定义的表项
    SymbolTableEntry *definedEntry{nullptr};

    SymbolTableEntryType getActualType() const;

    std::string *getName() const;

    bool isReferencedEntry() const;

    int getD2ForArray2();

    int funcParamsNum() const;

    std::vector<FuncParam *> *getFuncParams() const;

    int varGet() const;

    int getValueFromReferencedArray1(int d1) const;

    int array1get(int index) const;

    int *array1ConstGetAll() const;

    int **array2ConstGetAll() const;

    int array2get(int i1, int i2) const;

    static bool hasSameType(SymbolTableEntry *realParam, FuncParam *funcParam);

    int getValueFromReferencedArray2(int d1, int d2) const;

    void addParamForFuncEntry(SymbolTableEntry *param);

    bool isConst() const;
};

#endif //THIS_PROJECT_SYMBOL_TABLE_ENTRY_H
