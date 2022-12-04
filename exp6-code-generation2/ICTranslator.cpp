#include "ICTranslator.h"

#include "_debug.h"

extern std::ofstream icOutput;

ICTranslator *ICTranslator::self = nullptr;

ICTranslator *ICTranslator::getInstance() {
    if (self == nullptr) {
        self = new ICTranslator();
    }
    return self;
}

ICTranslator::ICTranslator() {
    mainEntries = new std::vector<ICEntry *>;
    id2allPureString = new std::map<int, std::string *>;
    name2icItemFunc = new std::map<std::string *, ICItemFunc *>;
    currentFunc = nullptr;
}

ICTranslator::~ICTranslator() {
    for (const auto *pItem: *mainEntries) delete pItem;
    delete mainEntries;
    for (const auto &i: *id2allPureString) delete i.second;
    delete id2allPureString;
    delete currentFunc;
}

void ICTranslator::translate_ConstVarDef(bool isGlobal, SymbolTableEntry *tableEntry,
                                         SymbolTable *currentTable) const {
    auto *icItemVar = new ICItemVar(tableEntry->getName(), tableEntry,
                                    true, isGlobal, tableEntry->varGet());
    currentTable->addICItem(*icItemVar->originalName, icItemVar);
    auto *icEntry = new ICEntry(ICEntryType::ConstVarDefine, icItemVar);

    if (currentFunc == nullptr) {
        mainEntries->push_back(icEntry);
    } else {
        currentFunc->entries->push_back(icEntry);
    }

}

void ICTranslator::translate_ConstArray1Def(bool isGlobal, SymbolTableEntry *tableEntry, int d1,
                                            SymbolTable *currentTable) const {
    auto *icItemArray = new ICItemArray(tableEntry->getName(), tableEntry, isGlobal, true, d1,
                                        tableEntry->array1ConstGetAll());
    icItemArray->setOriginType(1, d1, -1);
    currentTable->addICItem(*icItemArray->originalName, icItemArray);

    auto *icEntry = new ICEntry(ICEntryType::ConstArrayDefine, icItemArray);

    if (currentFunc == nullptr) {
        mainEntries->push_back(icEntry);
    } else {
        currentFunc->entries->push_back(icEntry);
    }
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
    icItemArray->setOriginType(2, d1, d2);
    currentTable->addICItem(*icItemArray->originalName, icItemArray);

    auto *icEntry = new ICEntry(ICEntryType::ConstArrayDefine, icItemArray);

    if (currentFunc == nullptr) {
        mainEntries->push_back(icEntry);
    } else {
        currentFunc->entries->push_back(icEntry);
    }
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

    if (currentFunc == nullptr) {
        mainEntries->push_back(icEntry);
    } else {
        currentFunc->entries->push_back(icEntry);
    }
}

void ICTranslator::translate_Array1Def(ICItem *initItem, bool isGlobal, SymbolTableEntry *tableEntry,
                                       bool hasInitVal, int length, SymbolTable *currentTable) const {
    ICEntry *icEntry;
    auto *initArray = ((ICItemArray *) initItem);

    if (isGlobal) {
        auto *icItemArray = new ICItemArray(tableEntry->getName(), tableEntry, true,
                                            false, length, initArray->value);
        icItemArray->setOriginType(1, length, -1);
        currentTable->addICItem(*icItemArray->originalName, icItemArray);

        icEntry = new ICEntry(ICEntryType::ArrayDefine, icItemArray);
    } else {
        auto *icItemArray = new ICItemArray(tableEntry->getName(), tableEntry, false,
                                            false, length);
        icItemArray->setOriginType(1, length, -1);
        currentTable->addICItem(*icItemArray->originalName, icItemArray);

        if (hasInitVal) {
            icEntry = new ICEntry(ICEntryType::ArrayDefine, icItemArray, initArray);
        } else {
            icEntry = new ICEntry(ICEntryType::ArrayDefine, icItemArray);
        }
    }

    if (currentFunc == nullptr) {
        mainEntries->push_back(icEntry);
    } else {
        currentFunc->entries->push_back(icEntry);
    }
}

void ICTranslator::translate_Array2Def(ICItem *initItem, bool isGlobal,
                                       SymbolTableEntry *tableEntry,
                                       bool hasInitVal, int d1, int d2,
                                       SymbolTable *currentTable) const {
    ICEntry *icEntry;
    auto *initArray = ((ICItemArray *) initItem);
    const int length = d1 * d2;

    if (isGlobal) {
        auto *icItemArray = new ICItemArray(tableEntry->getName(), tableEntry, true,
                                            false, length, initArray->value);
        icItemArray->setOriginType(2, d1, d2);
        currentTable->addICItem(*icItemArray->originalName, icItemArray);

        icEntry = new ICEntry(ICEntryType::ArrayDefine, icItemArray);
    } else {
        auto *icItemArray = new ICItemArray(tableEntry->getName(), tableEntry, false,
                                            false, length);
        icItemArray->setOriginType(2, d1, d2);
        currentTable->addICItem(*icItemArray->originalName, icItemArray);

        if (hasInitVal) {
            icEntry = new ICEntry(ICEntryType::ArrayDefine, icItemArray, initArray);
        } else {
            icEntry = new ICEntry(ICEntryType::ArrayDefine, icItemArray);
        }
    }

    if (currentFunc == nullptr) {
        mainEntries->push_back(icEntry);
    } else {
        currentFunc->entries->push_back(icEntry);
    }
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
    if (currentFunc == nullptr) {
        if (icEntryType == ICEntryType::Assign || icEntryType == ICEntryType::Beqz) {
            mainEntries->push_back(new ICEntry(icEntryType, dst, src1));
        } else {
            mainEntries->push_back(new ICEntry(icEntryType, dst, src1, src2));
        }
    } else {
        if (icEntryType == ICEntryType::Assign || icEntryType == ICEntryType::Beqz) {
            currentFunc->entries->push_back(new ICEntry(icEntryType, dst, src1));
        } else {
            currentFunc->entries->push_back(new ICEntry(icEntryType, dst, src1, src2));
        }
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
    if (currentFunc == nullptr) {
        mainEntries->push_back(new ICEntry(icEntryType, dst, src));
    } else {
        currentFunc->entries->push_back(new ICEntry(icEntryType, dst, src));
    }
}

ICEntryType ICTranslator::symbol2unaryOp(Symbol symbol) const {
    return _symbol2unaryOp.find(symbol)->second;
}

void ICTranslator::translate_getint(ICItem *dst) const {
    if (currentFunc == nullptr) {
        mainEntries->push_back(new ICEntry(ICEntryType::Getint, dst));
    } else {
        currentFunc->entries->push_back(new ICEntry(ICEntryType::Getint, dst));
    }
}

void ICTranslator::translate_printf(std::vector<int> *indexOfPercentSign,
                                    std::vector<ICItem *> *intItems,
                                    std::string *s) const {
    auto *icItemString = new ICItemString();
    int start = 1, len, intPos = 0;
    for (const int cur: *indexOfPercentSign) {
        len = cur - start;
        if (len > 0) {
            auto *substring = new std::string(s->substr(start, len));
            icItemString->addStringItem(substring);
        }
        start = cur + 2;
        icItemString->addIntItem(intItems->at(intPos++));
    }
    // 没有%d; 最后一个%d后的字符串
    if (indexOfPercentSign->empty()) {
        auto *substring = new std::string(s->substr(1, s->size() - 2));
        icItemString->addStringItem(substring);
    } else if (indexOfPercentSign->back() + 2 < s->size() - 1) {
        len = s->size() - 1 - (indexOfPercentSign->back() + 2);
        auto *substring = new std::string(s->substr(indexOfPercentSign->back() + 2, len));
        icItemString->addStringItem(substring);
    }
    // 加入全局记录字符串
    for (const auto &item: *(icItemString->id2pureString)) {
        id2allPureString->insert(item);
    }
    // 记录printf操作
    if (currentFunc == nullptr) {
        mainEntries->push_back(new ICEntry(ICEntryType::Print, icItemString));
    } else {
        currentFunc->entries->push_back(new ICEntry(ICEntryType::Print, icItemString));
    }
}

void ICTranslator::translate_Beqz(ICItem *condition, ICItemLabel *label) const {
    if (currentFunc == nullptr) {
        mainEntries->push_back(new ICEntry(ICEntryType::Beqz, condition, label));
    } else {
        currentFunc->entries->push_back(new ICEntry(ICEntryType::Beqz, condition, label));
    }
}

void ICTranslator::translate_InsertLabel(ICItemLabel *label) const {
    if (currentFunc == nullptr) {
        mainEntries->push_back(new ICEntry(ICEntryType::InsertLabel, label));
    } else {
        currentFunc->entries->push_back(new ICEntry(ICEntryType::InsertLabel, label));
    }
}

ICItemFunc *ICTranslator::translate_FuncDef(SymbolTableEntry *funcEntry,
                                            SymbolTable *currentTable) const {
    auto *func = new ICItemFunc(funcEntry);
    name2icItemFunc->insert({funcEntry->getName(), func});
    for (const auto *p: *(func->params)) {
        // TODO: 假设都是变量
        auto *var = (ICItemVar *) p;
        currentTable->addICItem(*var->originalName, var);
    }

    return func;
}


void ICTranslator::translate_FuncCall(std::string *funcName, std::vector<ICItem *> *params) {
    ICItemFunc *calledFunc = name2icItemFunc->find(funcName)->second;
    if (currentFunc != nullptr) {
        currentFunc->entries->push_back(new ICEntry(calledFunc, params));
    } else {
        mainEntries->push_back(new ICEntry(calledFunc, params));
    }

}

void ICTranslator::translate_return(ICItem *icItem) const {
    if (currentFunc != nullptr) {
        currentFunc->entries->push_back(new ICEntry(ICEntryType::FuncReturnWithValue, icItem));
    } else {
        mainEntries->push_back(new ICEntry(ICEntryType::MainFuncEnd));
    }
}

void ICTranslator::translate_return() const {
    if (currentFunc != nullptr) {
        currentFunc->entries->push_back(new ICEntry(ICEntryType::FuncReturn));
    } else {
        mainEntries->push_back(new ICEntry(ICEntryType::MainFuncEnd));
    }
}

void ICTranslator::output() {

}

void ICTranslator::translate_MainFunc() const {
    assert(currentFunc == nullptr);
    mainEntries->push_back(new ICEntry(ICEntryType::MainFuncStart));
}
