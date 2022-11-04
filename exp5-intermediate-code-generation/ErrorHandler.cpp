#include "ErrorHandler.h"
#include "item/symbolTable/SymbolTable.h"
#include "tree/ErrorNode.h"
#include "item/symbolTable/ConstValue.h"
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <cstring>
#include "_debug.h"

#define IS_GLOBAL ((currentTable->isRoot))

extern std::map<int, std::string> errorLog;

static bool createSymbolTableBeforeEnterBlock = false;
static int inWhile = 0;
enum class Func {
    IntFunc,
    VoidFunc,
    MainFunc,
    NotFunc
};
static Func currentFunction = Func::NotFunc;
static bool receiveReturn = false;
static int funcEndLineNum = 0;   // 结尾分号的行号

ErrorHandler::ErrorHandler(Node *root) : root(root) {
    this->rootTable = new SymbolTable(nullptr, true);
    this->currentTable = this->rootTable;
    this->icTranslator = ICTranslator::getInstance();
}


void ErrorHandler::check() {
    this->check_CompUnit(this->root);
}

// CompUnit → {Decl} {FuncDef} MainFuncDef
void ErrorHandler::check_CompUnit(Node *node) {
    for (Node *child: *(node->getAllChildren())) {
        if (child->is(GrammarItem::Decl)) this->check_Decl(child);
        else if (child->is(GrammarItem::FuncDef)) this->check_FuncDef(child);
        else this->check_MainFuncDef(child);
    }
}

// Decl → ConstDecl | VarDecl
void ErrorHandler::check_Decl(Node *node) {
    Node *child = node->getFirstChild();
    if (child->is(GrammarItem::ConstDecl)) this->check_ConstDecl(child);
    else this->check_VarDecl(child);
}

// FuncDef → FuncType Ident '(' [FuncFParams] ')' Block // b g j
// FIXME: b => ErrorType::IdentRedefined
// FIXME: g => ErrorType::ReturnMissing
// FIXME: j => ErrorType::MissingRPARENT )
void ErrorHandler::check_FuncDef(Node *node) {
    // 在check_Block前写到符号表
    Node *funcToken = node->getFirstChild()->getFirstChild();
    Node *ident = node->getChildAt(1);
    SymbolTableEntry *funcEntry;

    if (currentTable->nameExistedInCurrentTable(ident)) {
        errorLog.insert({ident->getToken()->lineNumber,
                         errorType2string.find(ErrorType::IdentRedefined)->second});
        return;
    }
    auto *funcTable = new SymbolTable(currentTable, false);
    currentTable->addChildTable(funcTable);
    if (funcToken->is(Symbol::VOIDTK)) {
        currentFunction = Func::VoidFunc;
        auto *functionOfVoid = new FunctionOfVoid();
        funcEntry = new SymbolTableEntry(ident, functionOfVoid, ident->getToken()->lineNumber);
    } else {
        currentFunction = Func::IntFunc;
        auto *functionOfInt = new FunctionOfInt();
        funcEntry = new SymbolTableEntry(ident, functionOfInt,
                                         ident->getToken()->lineNumber);
    }
    currentTable->addEntry(*funcEntry->getName(), funcEntry);
    currentTable = funcTable;
    if (node->getChildAt(3)->is(GrammarItem::FuncFParams)) {  // 有参数
        this->check_FuncFParams(node->getChildAt(3), funcEntry);
    }
    auto *errorNode = dynamic_cast<ErrorNode *>(node->getChildAt(-2));  // 倒数第二个
    if (errorNode != nullptr) {
        errorLog.insert({errorNode->lineNum, errorNode->error()});
    }
    createSymbolTableBeforeEnterBlock = true;
    receiveReturn = false;
    this->check_Block(node->getLastChild(), true);
    if (currentFunction == Func::IntFunc && (!receiveReturn)) {
        errorLog.insert({funcEndLineNum, errorType2string.find(ErrorType::ReturnMissing)->second});
    }
    currentTable = currentTable->parent;
    assert(currentTable->isRoot);
    receiveReturn = false;
    currentFunction = Func::NotFunc;
}

// ConstDecl → 'const' BType ConstDef { ',' ConstDef } ';'
// FIXME: i => ErrorType::MissingSEMICN
void ErrorHandler::check_ConstDecl(Node *node) {
    // 'const' BType 不用check
    for (auto i = 2; i < node->getChildrenNum() - 1; i += 2) {
        this->check_ConstDef(node->getChildAt(i));
    }
    Node *lastNode = node->getLastChild();
    auto *errorNode = dynamic_cast<ErrorNode *>(lastNode);
    if (errorNode != nullptr) {  // error
        errorLog.insert({errorNode->lineNum, errorNode->error()});
    }
}

// VarDecl → BType VarDef { ',' VarDef } ';' // i
// FIXME: i => ErrorType::MissingSEMICN
void ErrorHandler::check_VarDecl(Node *node) {
    for (int i = 1; i < node->getChildrenNum() - 1; i += 2) {
        this->check_VarDef(node->getChildAt(i));
    }
    auto *errorNode = dynamic_cast<ErrorNode *>(node->getLastChild());
    if (errorNode != nullptr) {  // error
        errorLog.insert({errorNode->lineNum, errorNode->error()});
    }
}

// MainFuncDef → 'int' 'main' '(' ')' Block // g j
// FIXME: g => ErrorType::ReturnMissing
// FIXME: j => ErrorType::MissingRPARENT
void ErrorHandler::check_MainFuncDef(Node *node) {
    auto *mainFuncTable = new SymbolTable(currentTable, false);
    currentTable->addChildTable(mainFuncTable);
    currentTable = mainFuncTable;
    createSymbolTableBeforeEnterBlock = true;
    receiveReturn = false;
    currentFunction = Func::MainFunc;
    this->check_Block(node->getLastChild(), true);
    if (!receiveReturn) {
        errorLog.insert({funcEndLineNum, errorType2string.find(ErrorType::ReturnMissing)->second});
    }
    receiveReturn = false;
    currentFunction = Func::NotFunc;
    currentTable = currentTable->parent;
}

// VarDef → Ident { '[' ConstExp ']' }  FIXME: b => ErrorType::IdentRedefined
// VarDef → Ident { '[' ConstExp ']' } '=' InitVal  FIXME: k => ErrorType::MissingRBRACK ]
void ErrorHandler::check_VarDef(Node *node) {
    Node *ident = node->getFirstChild();
    bool hasError = false;
    if (currentTable->nameExistedInCurrentTable(ident)) {
        hasError = true;
        errorLog.insert({ident->getToken()->lineNumber,
                         errorType2string.find(ErrorType::IdentRedefined)->second});
    } else {
        int i = 1, length = node->getChildrenNum();
        std::vector<int> arrayDimensions;  // size() = 0 || 1 || 2
        while (i < length - 2 && node->getChildAt(i)->is(Symbol::LBRACK)) {
            arrayDimensions.push_back(this->check_ConstExp(node->getChildAt(i + 1)));
            auto errorNode = dynamic_cast<ErrorNode *>(node->getChildAt(i + 2));
            if (errorNode != nullptr) {
                hasError = true;
                errorLog.insert({errorNode->lineNum, errorNode->error()});
            }
            i += 3;
        }
        int d = arrayDimensions.size();
        ICItem *icItem;
        if (d == 0) {
            if (IS_GLOBAL) icItem = new ICItemImm();  // 全局变量初始化，要求出初始值
            else icItem = new ICItemVar(false);
        } else {  // 一维数组或二维数组
            icItem = new ICItemArray(IS_GLOBAL);
        }
        bool hasInitVal = false;
        if (node->getLastChild()->is(GrammarItem::InitVal)) {
            hasInitVal = true;
            this->check_InitVal(node->getLastChild(), d, icItem);
        }
        if (hasError) return;
        if (IS_GLOBAL && (!hasInitVal)) {
            // 全局变量但没有显示初始化，自动初始化为0
            if (d == 0) ((ICItemImm *) icItem)->value = 0;
            else {
                int len;
                if (d == 1) len = arrayDimensions[0];
                else len = arrayDimensions[0] * arrayDimensions[1];
                ((ICItemArray *) icItem)->value = new int[len];
                for (int j = 0; j < len; ++j) ((ICItemArray *) icItem)->value[j] = 0;
            }
        }
        switch (d) {
            case 0: {
                auto *var = new Var();
                auto *varEntry = new SymbolTableEntry(ident, var, ident->getToken()->lineNumber, false);
                currentTable->addEntry(*varEntry->getName(), varEntry);
                icTranslator->translate_VarDef(icItem, IS_GLOBAL, varEntry, hasInitVal);
                return;
            }
            case 1: {
                auto *array1 = new Array1(arrayDimensions[0]);
                auto *array1Entry = new SymbolTableEntry(ident, array1,
                                                         ident->getToken()->lineNumber, false);
                currentTable->addEntry(*array1Entry->getName(), array1Entry);
                icTranslator->translate_ArrayDef(icItem, IS_GLOBAL, array1Entry,
                                                 hasInitVal, arrayDimensions[0]);
                return;
            }
            default:
                auto *array2 = new Array2(arrayDimensions[0], arrayDimensions[1]);
                auto *array2Entry = new SymbolTableEntry(ident, array2,
                                                         ident->getToken()->lineNumber, false);
                currentTable->addEntry(*array2Entry->getName(), array2Entry);
                icTranslator->translate_ArrayDef(icItem, IS_GLOBAL, array2Entry,
                                                 hasInitVal, arrayDimensions[0] * arrayDimensions[1]);
                return;
        }
    }
}

// ConstDef → Ident { '[' ConstExp ']' } '=' ConstInitVal
// FIXME: b => ErrorType::IdentRedefined
// FIXME: k => ErrorType::MissingRBRACK
void ErrorHandler::check_ConstDef(Node *node) {
    Node *ident = node->getFirstChild();
    if (currentTable->nameExistedInCurrentTable(ident)) {
        errorLog.insert({ident->getToken()->lineNumber,
                         errorType2string.find(ErrorType::IdentRedefined)->second});
    } else {
        int i = 1, length = node->getChildrenNum();
        std::vector<int> arrayDimensions;  // size() = 0 || 1 || 2
        while (i < length - 2 && node->getChildAt(i)->is(Symbol::LBRACK)) {
            arrayDimensions.push_back(this->check_ConstExp(node->getChildAt(i + 1)));
            auto errorNode = dynamic_cast<ErrorNode *>(node->getChildAt(i + 2));
            if (errorNode != nullptr) {
                errorLog.insert({errorNode->lineNum, errorNode->error()});
            }
            i += 3;
        }
        ConstValue *constValue = this->check_ConstInitVal(
                node->getLastChild(), arrayDimensions.size());
        // SymbolTableEntry(Node *node, SymbolTableEntryType type, unsigned int defLineNum);
        switch (constValue->dimension) {
            case 0: {
                auto *varConst = new VarConst(constValue->value.var);
                auto *varConstEntry = new SymbolTableEntry(
                        ident, varConst, ident->getToken()->lineNumber, false);
                currentTable->addEntry(*varConstEntry->getName(), varConstEntry);
                icTranslator->translate_ConstVarDef(IS_GLOBAL, varConstEntry);
                return;
            }
            case 1: {
                int d1 = constValue->d1;
                auto *array1Const = new Array1Const(d1, constValue->value.array1);
                auto *array1ConstEntry = new SymbolTableEntry(
                        ident, array1Const, ident->getToken()->lineNumber, false);
                currentTable->addEntry(*array1ConstEntry->getName(), array1ConstEntry);
                icTranslator->translate_ConstArray1Def(IS_GLOBAL, array1ConstEntry, d1);
                return;
            }
            default:
                int d1 = constValue->d1;
                int d2 = constValue->d2;
                auto *array2Const = new Array2Const(d1, d2, constValue->value.array2);
                auto *array2ConstEntry = new SymbolTableEntry(
                        ident, array2Const, ident->getToken()->lineNumber, false);
                currentTable->addEntry(*array2ConstEntry->getName(), array2ConstEntry);
                icTranslator->translate_ConstArray2Def(IS_GLOBAL, array2ConstEntry, d1, d2);
                return;
        }
    }
}

// ConstExp → AddExp
int ErrorHandler::check_ConstExp(Node *node) {  // 返回值为常量表达式的值
    int constValue;
    this->check_AddExp(node->getFirstChild(), true, &constValue);
    return constValue;
}

// AddExp → MulExp | AddExp ('+' | '−') MulExp
SymbolTableEntry *ErrorHandler::check_AddExp(Node *node, bool fromConstExp, int *constExpValue) {
    SymbolTableEntry *entry1 = nullptr, *entry2 = nullptr;
    int value1 = 0, value2 = 0;
    if (node->getChildrenNum() == 1) {
        return this->check_MulExp(node->getFirstChild(), fromConstExp, constExpValue);
    }
    entry1 = this->check_AddExp(node->getFirstChild(), fromConstExp, &value1);
    entry2 = this->check_MulExp(node->getLastChild(), fromConstExp, &value2);
    if (fromConstExp) {  // 必须为常量
        if (node->getChildAt(1)->is(Symbol::PLUS)) {
            *constExpValue = value1 + value2;
        } else {
            *constExpValue = value1 - value2;
        }
        return nullptr;
    }
    if (entry1 == nullptr || entry2 == nullptr) return nullptr;
    return entry1;  // TODO 生成代码时要计算 entry1 +/- entry2
}

// InitVal → Exp | '{' [ InitVal { ',' InitVal } ] '}'
void ErrorHandler::check_InitVal(Node *node, int d, ICItem *icItem) {
    int temp = 0;
    if (IS_GLOBAL) {  // 全局变量，直接求出InitVal，这期间不用生成中间代码
        switch (d) {
            case 0: {
                this->check_Exp(node->getFirstChild(), true, &temp, icItem);
                ((ICItemImm *) icItem)->value = temp;
                return;
            }
            case 1: {
                auto *icItemArray = (ICItemArray *) icItem;
                const int length = (node->getChildrenNum() - 1) / 2;
                icItemArray->value = new int[length];
                icItemArray->length = length;
                for (int i = 1, j = 0; i < node->getChildrenNum() - 1; i += 2) {
                    Node *exp = node->getChildAt(i)->getFirstChild();
                    this->check_Exp(exp, true, &temp, icItem);
                    icItemArray->value[j++] = temp;
                }
                return;
            }
            default: {
                auto *icItemArray = (ICItemArray *) icItem;
                const int d1 = (node->getChildrenNum() - 1) / 2;
                const int d2 = (node->getFirstChild()->getChildrenNum() - 1) / 2;
                icItemArray->value = new int[d1 * d2];
                icItemArray->length = d1 * d2;
                int k = 0;
                for (int i = 1; i < node->getChildrenNum() - 1; i += 2) {
                    for (int j = 1; j < node->getFirstChild()->getChildrenNum() - 1; j += 2) {
                        Node *exp = node->getChildAt(i)->getChildAt(j)->getFirstChild();
                        this->check_Exp(exp, true, &temp, icItem);
                        icItemArray->value[k++] = temp;
                    }
                }
                return;
            }
        }
    } else {  // 非全局变量初始值，不能求出常数，需要生成相应的中间代码(退出函数，回到varDef后生成)
        switch (d) {
            case 0: {
                this->check_Exp(node->getFirstChild(), false, &temp, icItem);
                return;
            }
            case 1: {
                auto *icItemArray = (ICItemArray *) icItem;
                std::vector<ICItemVar *> *arrayItems = icItemArray->itemsToInitArray;

                for (int i = 1; i < node->getChildrenNum() - 1; i += 2) {
                    Node *exp = node->getChildAt(i)->getFirstChild();
                    auto *itemVar = new ICItemVar(IS_GLOBAL);
                    this->check_Exp(exp, false, &temp, itemVar);
                    arrayItems->push_back(itemVar);
                }
                return;
            }
            default: {
                auto *icItemArray = (ICItemArray *) icItem;
                std::vector<ICItemVar *> *arrayItems = icItemArray->itemsToInitArray;

                for (int i = 1; i < node->getChildrenNum() - 1; i += 2) {
                    for (int j = 1; j < node->getFirstChild()->getChildrenNum() - 1; j += 2) {
                        Node *exp = node->getChildAt(i)->getChildAt(j)->getFirstChild();
                        auto *itemVar = new ICItemVar(IS_GLOBAL);
                        this->check_Exp(exp, false, &temp, itemVar);
                        arrayItems->push_back(itemVar);
                    }
                }
                return;
            }
        }
    }
}

// ConstInitVal → ConstExp | '{' [ ConstInitVal { ',' ConstInitVal } ] '}'
ConstValue *ErrorHandler::check_ConstInitVal(Node *node, int d) {
    auto *constValue = new ConstValue();
    constValue->dimension = d;
    switch (d) {
        case 0: {
            constValue->value.var = this->check_ConstExp(node->getFirstChild());
            return constValue;
        }
        case 1: {  // ConstInitVal → '{' ConstInitVal { ',' ConstInitVal }  '}'
            const int d1 = (node->getChildrenNum() - 1) / 2;
            int *initArray = new int[d1];
            for (int i = 1, j = 0; i < node->getChildrenNum() - 1; i += 2) {
                Node *constExp = node->getChildAt(i)->getFirstChild();
                initArray[j++] = (this->check_ConstExp(constExp));
            }
            constValue->value.array1 = initArray;
            return constValue;
        }
        default: // ConstInitVal → '{' ConstInitVal { ',' ConstInitVal }  '}'
            const int d1 = (node->getChildrenNum() - 1) / 2;
            int **initArray2 = new int *[d1];
            for (int i = 1, j = 0; i < node->getChildrenNum() - 1; i += 2, ++j) {
                ConstValue *retArray1 = this->check_ConstInitVal(node->getChildAt(i), 1);
                initArray2[j] = new int[retArray1->d1];
//                initArray2[j] = retArray1->value.array1;

                for (int k = 0; k < retArray1->d1; ++k) {
                    initArray2[j][k] = retArray1->value.array1[k];
                }
            }
            constValue->value.array2 = initArray2;
            return constValue;
    }
}

// MulExp → UnaryExp | MulExp ('*' | '/' | '%') UnaryExp
SymbolTableEntry *ErrorHandler::check_MulExp(Node *node, bool fromConstExp, int *constExpValue) {
    SymbolTableEntry *entry1 = nullptr, *entry2 = nullptr;
    int value1 = 0, value2 = 1;
    if (node->getChildrenNum() == 1) {
        return this->check_UnaryExp(node->getFirstChild(), fromConstExp, constExpValue);
    }
    entry1 = this->check_MulExp(node->getFirstChild(), fromConstExp, &value1);
    entry2 = this->check_UnaryExp(node->getLastChild(), fromConstExp, &value2);
    if (fromConstExp) {  // 必须为常量
        if (node->getChildAt(1)->is(Symbol::MULT)) {
            *constExpValue = value1 * value2;
        } else if (node->getChildAt(1)->is(Symbol::DIV)) {
            *constExpValue = value1 / value2;
        } else {
            *constExpValue = value1 % value2;
        }
        return nullptr;
    }
    return entry1;  // TODO 生成代码时要计算 entry1 * / % entry2
}

// UnaryExp → PrimaryExp | Ident '(' [FuncRParams] ')' | UnaryOp UnaryExp
// FIXME: c => ErrorType::IdentUndefined
// FIXME: length => ErrorType::ParamNumNotMatch
// FIXME: e => ErrorType::ParamTypeNotMatch
// FIXME: j => ErrorType::MissingRPARENT )
// 全是 UnaryExp → Ident '(' [FuncRParams] ')' 的错
SymbolTableEntry *ErrorHandler::check_UnaryExp(Node *node, bool fromConstExp, int *constExpValue) {
    Node *firstChild = node->getFirstChild();
    if (firstChild->is(GrammarItem::PrimaryExp))
        return this->check_PrimaryExp(firstChild, fromConstExp, constExpValue);
    if (firstChild->is(GrammarItem::UnaryOp)) {
        int op = 1;
        if (!firstChild->getFirstChild()->is(Symbol::PLUS)) op = -1;
        SymbolTableEntry *ret = this->check_UnaryExp(node->getChildAt(1), fromConstExp, constExpValue);
        *constExpValue = (*constExpValue * op);
        return ret;
    }
    if (!this->currentTable->nameExistedInAllTables(firstChild)) {
        errorLog.insert({firstChild->getToken()->lineNumber,
                         errorType2string.find(ErrorType::IdentUndefined)->second});
        *constExpValue = 0;
        return nullptr;
    } else {
        SymbolTableEntry *definedEntry = currentTable->getEntryByNameFromAllTables(firstChild);
        if (!node->getChildAt(2)->is(GrammarItem::FuncRParams)) {  // 没有参数
            if (definedEntry->funcParamsNum() > 0) {
                errorLog.insert({firstChild->getToken()->lineNumber,
                                 errorType2string.find(ErrorType::ParamNumNotMatch)->second});
                *constExpValue = 0;
                return nullptr;  // 出错了
            }
        } else {  // 有实参
            std::vector<SymbolTableEntry *> *calledEntry
                    = this->check_FuncRParams(node->getChildAt(2), definedEntry);
            if (this->findParamError(definedEntry, calledEntry, firstChild->getToken()->lineNumber)) {
                *constExpValue = 0;
                return nullptr;  // 出错了
            }
        }
        auto *errorNode = dynamic_cast<ErrorNode *>(node->getLastChild());
        if (errorNode != nullptr) {
            errorLog.insert({errorNode->lineNum, errorNode->error()});
        }
        return definedEntry;  // TODO
    }
}

// PrimaryExp → '(' Exp ')' | LVal | Number
SymbolTableEntry *ErrorHandler::check_PrimaryExp(Node *node, bool fromConstExp, int *constExpValue) {
    SymbolTableEntry *ret = nullptr;
    if (node->getChildrenNum() > 1) {
#ifdef ERROR_HANDLER_DEBUG
        std::cout << "\nChecking PrimaryExp -> '(' Exp ')':\nline: "
                  << node->getFirstChild()->getToken()->lineNumber << std::endl;
#endif
        ret = this->check_Exp(node->getChildAt(1), fromConstExp, constExpValue);
        auto *errorNode = dynamic_cast<ErrorNode *>(node->getLastChild());
        if (errorNode != nullptr) {
            errorLog.insert({errorNode->lineNum, errorNode->error()});
        }
        return ret;
    }
    if (node->getFirstChild()->is(GrammarItem::LVal)) {
        return this->check_LVal(node->getFirstChild(), fromConstExp, constExpValue);
    }
    return this->check_Number(node->getFirstChild(), fromConstExp, constExpValue);
}

// FuncRParams → Exp { ',' Exp }
std::vector<SymbolTableEntry *> *ErrorHandler::check_FuncRParams(
        Node *node,
        SymbolTableEntry *entry) {
    SymbolTableEntry *retEntry = nullptr;
    auto *funcRParams = new std::vector<SymbolTableEntry *>();
    std::vector<Node *> *children = node->getAllChildren();
    int temp = 0;
    for (auto i = 0; i < children->size(); i += 2) {
        retEntry = this->check_Exp(node->getChildAt(i), false, &temp);
        if (retEntry != nullptr) {
            funcRParams->push_back(retEntry);
        } else {  // 函数实参为常数
            auto *constParam = new SymbolTableEntry(temp);
            funcRParams->push_back(constParam);
        }
    }
    return funcRParams;
}

// 检查函数定义 definedEntry 和 函数调用 calledEntry 中的下面两个错：
// FIXME: length => ErrorType::ParamNumNotMatch
// FIXME: e => ErrorType::ParamTypeNotMatch
// 返回值表示是否有错
bool ErrorHandler::findParamError(SymbolTableEntry *definedEntry,
                                  std::vector<SymbolTableEntry *> *calledEntry,
                                  int lineNum) {
    auto size = calledEntry->size();
    if (definedEntry->funcParamsNum() != size) {
        errorLog.insert({lineNum, errorType2string.find(
                ErrorType::ParamNumNotMatch)->second});
        return true;
    }
    std::vector<FuncParam *> *definedFuncParams = definedEntry->getFuncParams();
    for (auto i = 0; i < size; ++i) {
        // hasSameType(SymbolTableEntry *realParam, FuncParam *funcParam)
        bool typeSame = SymbolTableEntry::hasSameType((*calledEntry)[i],
                                                      (*definedFuncParams)[i]);
        if (!typeSame) {
            errorLog.insert({lineNum, errorType2string.find(
                    ErrorType::ParamTypeNotMatch)->second});
            return true;
        }
    }
    return false;
}

SymbolTableEntry *ErrorHandler::check_Exp(Node *node, bool fromConstExp,
                                          int *constExpValue, ICItem *icItem) {
#ifdef ERROR_HANDLER_DEBUG
    currentTable->printAllNames();
#endif
    return this->check_AddExp(node->getFirstChild(), fromConstExp, constExpValue);
}

// LVal → Ident {'[' Exp ']'} // c k
// FIXME: ErrorType::IdentUndefined  c
// FIXME: ErrorType::MissingRBRACK  k ]
SymbolTableEntry *ErrorHandler::check_LVal(Node *node, bool fromConstExp, int *constExpValue) {
    Node *ident = node->getFirstChild();
    if (!this->currentTable->nameExistedInAllTables(ident)) {
        errorLog.insert({ident->getToken()->lineNumber,
                         errorType2string.find(ErrorType::IdentUndefined)->second});
        *constExpValue = 0;
        return nullptr;
    }
    const auto size = node->getChildrenNum();  // 1 or 4 or 7
    SymbolTableEntry *definedEntry = this->currentTable->getEntryByNameFromAllTables(ident);
    switch (size) {
        case 1: {  // ident
            if (fromConstExp) {
//                *constExpValue = 0;
                assert(definedEntry->isConst());
                *constExpValue = definedEntry->varGet();  // TODO: 没有求值
            }
            return definedEntry;
        }
        case 4: // ident[exp]
        case 7:  // ident[exp][exp]
            Node *exp1 = node->getChildAt(2);
            int v1 = 0, v2 = 0;
            SymbolTableEntry *exp1Entry = this->check_Exp(exp1, fromConstExp, &v1);
            auto *errorNode = dynamic_cast<ErrorNode *>(node->getChildAt(3));
            if (errorNode != nullptr) {
                errorLog.insert({errorNode->lineNum, errorNode->error()});
            }
            // ---- 计算函数实参 actualType -----
            // 如有数组 int arr[2][3], 则 引用 arr[0] 的actualType 为 array1
            SymbolTableEntryType definedType = definedEntry->getActualType();
            SymbolTableEntryType actualType;
            SymbolTableEntry *referencedEntry = nullptr;
            switch (size) {
                case 4: {
                    if (definedType == SymbolTableEntryType::Array1 ||
                        definedType == SymbolTableEntryType::Array1Const) {
                        // 实际传进去的是 var
                        referencedEntry = new SymbolTableEntry(definedEntry,
                                                               SymbolTableEntryType::Var, v1);
                        if (fromConstExp) {
                            *constExpValue = referencedEntry->getValueFromReferencedArray1(v1);
                            return nullptr;
                        } else {
                            return referencedEntry;
                        }
                    } else {  // definedType 是二维数组，实际传进去的是一维数组
                        referencedEntry = new SymbolTableEntry(definedEntry,
                                                               SymbolTableEntryType::Array1, v1);
//                        assert(fromConstExp == false);  FIXME: TODO: 当没看到
                        return referencedEntry;
                    }
                }
                default: {
                    // 引用时为 arr[x][y], 故定义时 arr肯定是二维数组， 实际传入函数的类型为 var
                    referencedEntry = new SymbolTableEntry(definedEntry,
                                                           SymbolTableEntryType::Var, v1, v2);
                    Node *exp2 = node->getChildAt(5);
                    SymbolTableEntry *exp2Entry = this->check_Exp(exp2, fromConstExp, &v2);
                    errorNode = dynamic_cast<ErrorNode *>(node->getChildAt(6));
                    if (errorNode != nullptr) {
                        errorLog.insert({errorNode->lineNum, errorNode->error()});
                    }
                    if (fromConstExp) {
                        *constExpValue = referencedEntry->getValueFromReferencedArray2(v1, v2);
                        return nullptr;
                    } else {
                        return referencedEntry;
                    }
                }
            }
    }
#ifdef ERROR_HANDLER_DEBUG
    std::cout << "\nErrorHandler::check_LVal:\n" << ident->getToken()->value
              << ", line " << ident->getToken()->lineNumber << "\n"
              << "size: " << size << std::endl;
#endif
    throw std::runtime_error("Reach line 481 in ErrorHandler.cpp");
}

// Number → IntConst
// IntConst -> 数字
SymbolTableEntry *ErrorHandler::check_Number(Node *node, bool fromConstExp, int *constExpValue) {
#ifdef ERROR_HANDLER_DEBUG
    std::cout << "\nChecking Number -> IntConst:\nline: "
              << node->getFirstChild()->getToken()->lineNumber << ", number: "
              << node->getFirstChild()->getToken()->value << std::endl;
#endif
    *constExpValue = std::stoi(
            node->getFirstChild()->getToken()->value);
    return nullptr;
}

//  FuncFParams → FuncFParam { ',' FuncFParam }
void ErrorHandler::check_FuncFParams(Node *funcFParams, SymbolTableEntry *funcIdentEntry) {
    SymbolTableEntry *funcFParam;
    // 获取实际类型，如有变量定义为 int arr[1][2], 其作为实参时为 arr[1]
    // 此时实参arr[1]的actualType为 array1
    SymbolTableEntryType retType = funcIdentEntry->getActualType();
    for (int i = 0; i < funcFParams->getChildrenNum(); i += 2) {
        funcFParam = this->check_FuncFParam(funcFParams->getChildAt(i), retType);
        if (currentTable->nameExistedInCurrentTable(funcFParam->node)) {
            errorLog.insert({funcFParam->node->getToken()->lineNumber,
                             errorType2string.find(ErrorType::IdentRedefined)->second});
        } else {
            currentTable->addEntry(*(funcFParam->getName()), funcFParam);
        }
        funcIdentEntry->addParamForFuncEntry(funcFParam);
    }
}

// FuncFParam → BType Ident ['[' ']' { '[' ConstExp ']' }]  //   b k
// FIXME: b => ErrorType::IdentRedefined
// FIXME: k => ErrorType::MissingRBRACK  ]
SymbolTableEntry *ErrorHandler::check_FuncFParam(Node *funcFParam, SymbolTableEntryType retType) {
    Node *ident = funcFParam->getChildAt(1);
#ifdef ERROR_HANDLER_DEBUG
    std::cout << "\nChecking FuncFParam:\nline: "
              << ident->getToken()->lineNumber << ", number: "
              << ident->getToken()->value << std::endl;
#endif
    if (currentTable->nameExistedInCurrentTable(ident)) {
        errorLog.insert({ident->getToken()->lineNumber,
                         errorType2string.find(ErrorType::IdentRedefined)->second});
    }

    if (funcFParam->getChildrenNum() == 2) {
        auto *var = new Var();
        auto *varParam = new SymbolTableEntry(ident, var, ident->getToken()->lineNumber, true);
        return varParam;
    }
    if (funcFParam->getChildrenNum() == 4) {
        auto *array1 = new Array1(-1);
        auto *arrayParam = new SymbolTableEntry(
                ident, array1, ident->getToken()->lineNumber, true);
        auto *errorNode = dynamic_cast<ErrorNode *>(funcFParam->getChildAt(3));
        if (errorNode != nullptr) {
            errorLog.insert({errorNode->lineNum, errorNode->error()});
        }
        return arrayParam;
    }
    const int d2 = this->check_ConstExp(funcFParam->getChildAt(5));
    auto *array2 = new Array2(-1, 2);
    auto *array2Param = new SymbolTableEntry(ident, array2, ident->getToken()->lineNumber, true);
    auto *errorNode = dynamic_cast<ErrorNode *>(funcFParam->getChildAt(3));
    if (errorNode != nullptr) {
        errorLog.insert({errorNode->lineNum, errorNode->error()});
    }
    errorNode = dynamic_cast<ErrorNode *>(funcFParam->getChildAt(6));
    if (errorNode != nullptr) {
        errorLog.insert({errorNode->lineNum, errorNode->error()});
    }
    return array2Param;
}

// Block → '{' { BlockItem } '}'
// 需在调用前新建符号表!!!!!
void ErrorHandler::check_Block(Node *block, bool inFuncBlock) {
    assert(createSymbolTableBeforeEnterBlock);
    createSymbolTableBeforeEnterBlock = false;
    for (int i = 1; i < block->getChildrenNum() - 1; ++i) {
        this->check_BlockItem(block->getChildAt(i), inFuncBlock);
    }
    funcEndLineNum = block->getLastChild()->getToken()->lineNumber;  // 结尾分号的行号
}

// BlockItem → Decl | Stmt
void ErrorHandler::check_BlockItem(Node *blockItem, bool inFuncBlock) {
    if (blockItem->getFirstChild()->is(GrammarItem::Decl)) {
        this->check_Decl(blockItem->getFirstChild());
    } else {
        this->check_Stmt(blockItem->getFirstChild(), inFuncBlock);
    }
}

/*
 * Stmt → LVal '=' Exp ';' FIXME: h => ErrorType::ConstantModification; i => ErrorType::MissingSEMICN
 *      | [Exp] ';' FIXME: i => ErrorType::MissingSEMICN
 *      | Block
 *      | 'if' '(' Cond ')' Stmt [ 'else' Stmt ] FIXME: j => ErrorType::MissingRPARENT )
 *      | 'while' '(' Cond ')' Stmt FIXME: j => ErrorType::MissingRPARENT )
 *      | 'break' ';' FIXME: m => ErrorType::RedundantBreakContinue; i => ErrorType::MissingSEMICN
 *      | 'continue' ';' FIXME: m => ErrorType::RedundantBreakContinue; i => ErrorType::MissingSEMICN
 *      | 'return' [Exp] ';' FIXME: f => ErrorType::ReturnRedundant; i => ErrorType::MissingSEMICN
 *      | LVal '=' 'getint''('')'';'  FIXME: h => ErrorType::ConstantModification;
 *                  FIXME: i => ErrorType::MissingSEMICN; j => ErrorType::MissingRPARENT )
 *      | 'printf''('FormatString{','Exp}')'';' FIXME: i => ErrorType::MissingSEMICN;
 *                  FIXME: j => ErrorType::MissingRPARENT ); l => ErrorType::FormatStrNumNotMatch
 */
void ErrorHandler::check_Stmt(Node *stmt, bool inFuncBlock) {
    Node *first = stmt->getFirstChild();
    Node *last = stmt->getLastChild();
    int temp;
    if (first->is(GrammarItem::LVal)) {
        // Stmt → LVal '=' Exp ';'
        // Stmt → LVal '=' 'getint''('')'';'
        SymbolTableEntry *firstEntry = this->check_LVal(first, false, &temp);
        if (firstEntry == nullptr) return;  // 名字未定义
        if (firstEntry->isConst()) {
            // 行号： LVal → Ident {'[' Exp ']'}
            errorLog.insert({first->getFirstChild()->getToken()->lineNumber,
                             errorType2string.find(ErrorType::ConstantModification)->second});
        }
        if (stmt->getChildAt(2)->is(GrammarItem::Exp)) {  // LVal '=' Exp ';'
            this->check_Exp(stmt->getChildAt(-2), false, &temp);
        } else {  // LVal '=' 'getint''('')'';'
            auto *errorNode = dynamic_cast<ErrorNode *>(stmt->getChildAt(4));  // ErrorType::MissingRPARENT
            if (errorNode != nullptr) {
                errorLog.insert({errorNode->lineNum, errorNode->error()});
            }
        }
        auto *errorNode = dynamic_cast<ErrorNode *>(last);
        if (errorNode != nullptr) {
            errorLog.insert({errorNode->lineNum, errorNode->error()});
        }
    } else if (first->is(GrammarItem::Block)) {  // Stmt → Block
        auto *childTable = new SymbolTable(currentTable, false);
        currentTable = childTable;
        createSymbolTableBeforeEnterBlock = true;
        this->check_Block(first);
        currentTable = currentTable->parent;
    } else if (first->is(GrammarItem::Exp) || stmt->getChildrenNum() == 1) {  // Stmt → [Exp] ';'
        if (first->is(GrammarItem::Exp)) {
            this->check_Exp(first, false, &temp);
        }
        auto *errorNode = dynamic_cast<ErrorNode *>(last);
        if (errorNode != nullptr) {
            errorLog.insert({errorNode->lineNum, errorNode->error()});
        }
    } else if (first->is(Symbol::IFTK)) {  // Stmt → 'if' '(' Cond ')' Stmt [ 'else' Stmt ]
        this->check_Cond(stmt->getChildAt(2));
        auto *errorNode = dynamic_cast<ErrorNode *>(stmt->getChildAt(3));  // ErrorType::MissingRPARENT
        if (errorNode != nullptr) {
            errorLog.insert({errorNode->lineNum, errorNode->error()});
        }
        this->check_Stmt(stmt->getChildAt(4));
        if (stmt->getChildrenNum() > 5) {
            this->check_Stmt(stmt->getChildAt(6));
        }
    } else if (first->is(Symbol::WHILETK)) {  // Stmt → 'while' '(' Cond ')' Stmt
        this->check_Cond(stmt->getChildAt(2));
        auto *errorNode = dynamic_cast<ErrorNode *>(stmt->getChildAt(3));  // ErrorType::MissingRPARENT
        if (errorNode != nullptr) {
            errorLog.insert({errorNode->lineNum, errorNode->error()});
        }
        inWhile++;
        this->check_Stmt(stmt->getChildAt(4));
        inWhile--;
    } else if (first->is(Symbol::BREAKTK) || first->is(Symbol::CONTINUETK)) {
        if (!inWhile) {
            errorLog.insert(
                    {first->getToken()->lineNumber,
                     errorType2string.find(ErrorType::RedundantBreakContinue)->second});
        }
        auto *errorNode = dynamic_cast<ErrorNode *>(last);  // ErrorType::MissingSEMICN
        if (errorNode != nullptr) {
            errorLog.insert({errorNode->lineNum, errorNode->error()});
        }
    } else if (first->is(Symbol::RETURNTK)) {  // 'return' [Exp] ';'
        receiveReturn = inFuncBlock;
        if ((currentFunction != Func::IntFunc && currentFunction != Func::MainFunc)
            && stmt->getChildAt(1)->is(GrammarItem::Exp)) {
            errorLog.insert({first->getToken()->lineNumber,
                             errorType2string.find(ErrorType::ReturnRedundant)->second});
        }
        if (stmt->getChildAt(1)->is(GrammarItem::Exp)) {
            this->check_Exp(stmt->getChildAt(1), false, &temp);
        }
        auto *errorNode = dynamic_cast<ErrorNode *>(last);  // ErrorType::MissingSEMICN
        if (errorNode != nullptr) {
            errorLog.insert({errorNode->lineNum, errorNode->error()});
        }
    } else {  // 'printf''('FormatString{','Exp}')'';'
        // ErrorType::FormatStrNumNotMatch
        int leftNum = 0, rightNum = 0;
        Node *formatStr = stmt->getChildAt(2);
        bool hasIllegalChar = ErrorHandler::check_FormatString(formatStr, &leftNum);
        if (hasIllegalChar) {
            errorLog.insert({formatStr->getToken()->lineNumber,
                             errorType2string.find(ErrorType::IllegalChar)->second});
        }
        for (int i = 4; i < stmt->getChildrenNum() - 2; i += 2) {
            ++rightNum;
            this->check_Exp(stmt->getChildAt(i), false, &temp);
        }
        if (leftNum != rightNum) {
            errorLog.insert({formatStr->getToken()->lineNumber,
                             errorType2string.find(ErrorType::FormatStrNumNotMatch)->second});
        }
        auto *errorNode = dynamic_cast<ErrorNode *>(stmt->getChildAt(-2));  // ErrorType::MissingRPARENT
        if (errorNode != nullptr) {
            errorLog.insert({errorNode->lineNum, errorNode->error()});
        }
        errorNode = dynamic_cast<ErrorNode *>(stmt->getLastChild());  // ErrorType::MissingSEMICN
        if (errorNode != nullptr) {
            errorLog.insert({errorNode->lineNum, errorNode->error()});
        }
    }

}

// Cond → LOrExp
void ErrorHandler::check_Cond(Node *node) {
    this->check_LOrExp(node->getFirstChild());
}

bool ErrorHandler::check_FormatString(Node *node, int *formatNum) {
    *formatNum = 0;
    std::string s = node->getToken()->value;
    bool hasIllegalChar = false;
    for (int i = 1; i < s.size() - 1; ++i) {
        // check <FormatChar>
        auto now = s[i];
        auto next = s[i + 1];  // i < length - 1保证了不越界
        if (now == '%' && (i >= s.size() - 2 || next != 'd')) {
            hasIllegalChar = true;
        } else if (now == '%' && next == 'd') {
            *formatNum = *formatNum + 1;
            ++i;  // 从后两个开始判断
        } else if (now == '\\' && next == 'n') {
            ++i;
        } else if (now == '\\' && next != 'n') {
            hasIllegalChar = true;
        } else {
            int ascii = (int) ((unsigned char) now);
            if (!(ascii == 32 || ascii == 33 || (ascii >= 40 && ascii <= 126))) {
                hasIllegalChar = true;
            }
        }
    }
    return hasIllegalChar;
}

// LOrExp → LAndExp | LOrExp '||' LAndExp
void ErrorHandler::check_LOrExp(Node *node) {
    if (node->getChildrenNum() == 1) {
        this->check_LAndExp(node->getFirstChild());
    } else {
        this->check_LOrExp(node->getFirstChild());
        this->check_LAndExp(node->getLastChild());
    }
}

// LAndExp → EqExp | LAndExp '&&' EqExp
void ErrorHandler::check_LAndExp(Node *node) {
    if (node->getChildrenNum() == 1) {
        this->check_EqExp(node->getFirstChild());
    } else {
        this->check_LAndExp(node->getFirstChild());
        this->check_EqExp(node->getLastChild());
    }
}

// EqExp → RelExp | EqExp ('==' | '!=') RelExp
void ErrorHandler::check_EqExp(Node *node) {
    if (node->getChildrenNum() == 1) {
        this->check_RelExp(node->getFirstChild());
    } else {
        this->check_EqExp(node->getFirstChild());
        this->check_RelExp(node->getLastChild());
    }
}

// RelExp → AddExp | RelExp ('<' | '>' | '<=' | '>=') AddExp
void ErrorHandler::check_RelExp(Node *node) {
    int temp = 0;
    if (node->getChildrenNum() == 1) {
        this->check_AddExp(node->getFirstChild(), false, &temp);
    } else {
        this->check_RelExp(node->getFirstChild());
        this->check_AddExp(node->getLastChild(), false, &temp);
    }
}