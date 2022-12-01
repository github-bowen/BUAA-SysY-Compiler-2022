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
#include "item/ic/ICItemFunc.h"

/**
 * Intermediate Code Translator 中间代码翻译器
 */
class ICTranslator {
    static ICTranslator *self;  // 单例模式

    ICTranslator();

    ~ICTranslator();

    const std::map<Symbol, ICEntryType> _symbol2binaryOp = {
            {Symbol::PLUS,   ICEntryType::Add},
            {Symbol::ASSIGN, ICEntryType::Assign},
            {Symbol::MINU,   ICEntryType::Sub},
            {Symbol::MULT,   ICEntryType::Mul},
            {Symbol::DIV,    ICEntryType::Div},
            {Symbol::MOD,    ICEntryType::Mod}

    };

    const std::map<Symbol, ICEntryType> _symbol2unaryOp = {
            {Symbol::NOT,  ICEntryType::Not},
            {Symbol::MINU, ICEntryType::Neg},
    };

public:
    static ICTranslator *getInstance();

    std::vector<ICEntry *> *mainEntries;  // 包括全局变量、常量和main函数

    std::map<int, std::string *> *id2allPureString;

    std::map<std::string *, ICItemFunc *> *name2icItemFunc;

    ICItemFunc *currentFunc;

    bool inFunc{false};

    ICEntryType symbol2binaryOp(Symbol symbol) const;

    ICEntryType symbol2unaryOp(Symbol symbol) const;

    /* 常量声明 */

    void translate_ConstVarDef(bool isGlobal, SymbolTableEntry *tableEntry,
                               SymbolTable *currentTable) const;

    void translate_ConstArray1Def(bool isGlobal, SymbolTableEntry *tableEntry, int d1,
                                  SymbolTable *currentTable) const;

    void translate_ConstArray2Def(bool isGlobal, SymbolTableEntry *tableEntry, int d1, int d2,
                                  SymbolTable *currentTable) const;

    /* 变量声明 */
    void translate_VarDef(ICItem *initItem, bool isGlobal,
                          SymbolTableEntry *tableEntry, bool hasInitVal,
                          SymbolTable *currentTable) const;

    void translate_ArrayDef(ICItem *initItem, bool isGlobal,
                            SymbolTableEntry *tableEntry, bool hasInitVal, int length,
                            SymbolTable *currentTable) const;

    /* 双目运算符 */
    void translate_BinaryOperator(
            ICEntryType icEntryType, ICItem *dst, ICItem *src1, ICItem *src2 = nullptr) const;

    /* 单目运算符 */
    void translate_UnaryOperator(ICEntryType icEntryType, ICItem *dst, ICItem *src) const;

    /* getint() */
    void translate_getint(ICItem *dst) const;

    /* printf */
    void translate_printf(std::vector<int> *indexOfPercentSign,
                          std::vector<ICItem *> *intItems,
                          std::string *s) const;

    /* 转到主函数 */
    void translate_MainFunc() const;

    /* 函数定义 */
    ICItemFunc *translate_FuncDef(SymbolTableEntry *funcEntry,
                                  SymbolTable *currentTable) const;

    /* 函数调用 */
    void translate_FuncCall(std::string *funcName, std::vector<ICItem *> *params = nullptr);

    /* 函数返回 */
    void translate_return(ICItem *) const;

    void translate_return() const;

    /* 中间代码输出 */
    void output();


};


#endif //INTERMEDIATE_CODE_GENERATOR_H
