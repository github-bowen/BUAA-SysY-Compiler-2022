#ifndef INTERMEDIATE_CODE_GENERATOR_H
#define INTERMEDIATE_CODE_GENERATOR_H

#include "tree/Node.h"
#include "item/symbolTable/SymbolTable.h"

#include "item/ic/ICItemType.h"
#include "item/ic/ICItem.h"
#include "item/ic/ICItemArray.h"
#include "item/ic/ICItemImm.h"
#include "item/ic/ICItemLabel.h"
#include "item/ic/ICItemString.h"
#include "item/ic/ICItemVar.h"
#include "item/ic/ICEntryType.h"
#include "item/ic/ICEntry.h"

/**
 * Intermediate Code Translator 中间代码翻译器
 */
class ICTranslator {
    static ICTranslator *self;  // 单例模式

    ICTranslator();

    ~ICTranslator();

public:
    static ICTranslator *getInstance();

    std::vector<ICEntry *> *icEntries;

    void translate_ConstVarDef(bool isGlobal, SymbolTableEntry *tableEntry) const;

    void translate_ConstArray1Def(bool isGlobal, SymbolTableEntry *tableEntry, int d1) const;

    void translate_ConstArray2Def(bool isGlobal, SymbolTableEntry *tableEntry, int d1, int d2) const;
};


#endif //INTERMEDIATE_CODE_GENERATOR_H
