#include "ICTranslator.h"

#include "_debug.h"

ICTranslator *ICTranslator::self = nullptr;

ICTranslator *ICTranslator::getInstance() {
    if (self == nullptr) {
        self = new ICTranslator();
    }
    return self;
}

ICTranslator::ICTranslator() {
    icEntries = new std::vector<ICEntry *>;
}

ICTranslator::~ICTranslator() {
    for (const auto *pItem: *icEntries) delete pItem;
    delete icEntries;
}

void ICTranslator::translate_ConstVarDef(bool isGlobal, SymbolTableEntry *tableEntry,
                                         SymbolTable *currentTable) const {
    auto *icItemVar = new ICItemVar(tableEntry->getName(), tableEntry,
                                    true, isGlobal, tableEntry->varGet());
    currentTable->addICItem(*icItemVar->originalName, icItemVar);

    auto *icEntry = new ICEntry(ICEntryType::ConstVarDefine, icItemVar);

    icEntries->push_back(icEntry);
}

void ICTranslator::translate_ConstArray1Def(bool isGlobal, SymbolTableEntry *tableEntry, int d1,
                                            SymbolTable *currentTable) const {
    auto *icItemArray = new ICItemArray(tableEntry->getName(), tableEntry, isGlobal, true, d1,
                                        tableEntry->array1ConstGetAll());
    currentTable->addICItem(*icItemArray->originalName, icItemArray);

    auto *icEntry = new ICEntry(ICEntryType::ConstArrayDefine, icItemArray);
    icEntries->push_back(icEntry);
}

void ICTranslator::translate_ConstArray2Def(bool isGlobal, SymbolTableEntry *tableEntry,
                                            int d1, int d2, SymbolTable *currentTable) const {
    int **oldArray = tableEntry->array2ConstGetAll();
    int *newArray = new int[d1 * d2];

    int k = 0;
    for (int i = 0; i < d1; ++i) {
        for (int j = 0; j < d2; ++j) {
            newArray[k++] = oldArray[i][j];
        }
    }

    auto *icItemArray = new ICItemArray(tableEntry->getName(), tableEntry, isGlobal,
                                        true, d1 * d2, newArray);
    currentTable->addICItem(*icItemArray->originalName, icItemArray);

    auto *icEntry = new ICEntry(ICEntryType::ConstArrayDefine, icItemArray);
    icEntries->push_back(icEntry);
}

void ICTranslator::translate_VarDef(ICItem *initItem, bool isGlobal,
                                    SymbolTableEntry *tableEntry, bool hasInitVal,
                                    SymbolTable *currentTable) const {
    ICEntry *icEntry;
    if (isGlobal) {  // 全局变量，初值确定，一个操作数
        auto *icItemImm = (ICItemImm *) initItem;
        auto *icItemVar = new ICItemVar(tableEntry->getName(), tableEntry,
                                        false, true, icItemImm->value);
        currentTable->addICItem(*icItemVar->originalName, icItemVar);

        icEntry = new ICEntry(ICEntryType::VarDefine, icItemVar);
    } else {  // 非全局变量，若hasInitVal == false, 则无初值!
        auto *rightValue = (ICItemVar *) initItem;
        auto *icItemVar = new ICItemVar(tableEntry->getName(), tableEntry,
                                        false, false);
        currentTable->addICItem(*icItemVar->originalName, icItemVar);

        if (hasInitVal) {
            icEntry = new ICEntry(ICEntryType::VarDefine, icItemVar, rightValue);
        } else {
            icEntry = new ICEntry(ICEntryType::VarDefine, icItemVar);
        }
    }
    icEntries->push_back(icEntry);
}

void ICTranslator::translate_ArrayDef(ICItem *initItem, bool isGlobal,
                                      SymbolTableEntry *tableEntry, bool hasInitVal, int length,
                                      SymbolTable *currentTable) const {
    ICEntry *icEntry;
    auto *initArray = ((ICItemArray *) initItem);

    if (isGlobal) {
        auto *icItemArray = new ICItemArray(tableEntry->getName(), tableEntry, true,
                                            false, length, initArray->value);
        currentTable->addICItem(*icItemArray->originalName, icItemArray);

        icEntry = new ICEntry(ICEntryType::ArrayDefine, icItemArray);
    } else {
        auto *icItemArray = new ICItemArray(tableEntry->getName(), tableEntry, false,
                                            false, length);
        currentTable->addICItem(*icItemArray->originalName, icItemArray);

        if (hasInitVal) {
            icEntry = new ICEntry(ICEntryType::ArrayDefine, icItemArray, initArray);
        } else {
            icEntry = new ICEntry(ICEntryType::VarDefine, icItemArray);
        }
    }
    icEntries->push_back(icEntry);
}

/**
 * 翻译双目运算
 * @param icEntryType
 * @param dst
 * @param src1
 * @param src2
 */
void ICTranslator::translate_BinaryOperator(ICEntryType icEntryType,
                                            ICItem *dst, ICItem *src1, ICItem *src2) const {
    if (icEntryType == ICEntryType::Assign) {
        icEntries->push_back(new ICEntry(icEntryType, dst, src1));
    } else {
        icEntries->push_back(new ICEntry(icEntryType, dst, src1, src2));
    }
}

ICEntryType ICTranslator::symbol2binaryOp(Symbol symbol) const {
    return _symbol2binaryOp.find(symbol)->second;
}

/**
 * 翻译单目运算符
 * @param icEntryType
 * @param dst
 * @param src
 */
void ICTranslator::translate_UnaryOperator(ICEntryType icEntryType, ICItem *dst, ICItem *src) const {
    icEntries->push_back(new ICEntry(icEntryType, dst, src));
}

ICEntryType ICTranslator::symbol2unaryOp(Symbol symbol) const {
    return _symbol2unaryOp.find(symbol)->second;
}


