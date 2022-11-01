#ifndef THIS_PROJECT_ERROR_HANDLER_H
#define THIS_PROJECT_ERROR_HANDLER_H

#include "tree/Node.h"
#include "item/symbolTable/SymbolTable.h"
#include "item/symbolTable/ConstValue.h"

#include "item/ic/ICItemType.h"
#include "item/ic/ICItem.h"
#include "item/ic/ICItemArray.h"
#include "item/ic/ICItemConstNum.h"
#include "item/ic/ICItemLabel.h"
#include "item/ic/ICItemString.h"
#include "item/ic/ICItemVar.h"

class ErrorHandler {
    Node *root;
public:
    SymbolTable *currentTable;
    SymbolTable *rootTable;

    explicit ErrorHandler(Node *);

    void check();

    void check_CompUnit(Node *node);

    void check_Decl(Node *node);

    void check_FuncDef(Node *node);

    void check_ConstDecl(Node *node);

    void check_VarDecl(Node *node);

    void check_MainFuncDef(Node *node);

    void check_ConstDef(Node *node);

    int check_ConstExp(Node *node);

    void check_InitVal(Node *node);

    ConstValue *check_ConstInitVal(Node *node, int d);

    SymbolTableEntry *check_AddExp(Node *node, bool fromConstExp, int *constExpValue);

    SymbolTableEntry *check_MulExp(Node *node, bool fromConstExp, int *constExpValue);

    SymbolTableEntry *check_UnaryExp(Node *node, bool fromConstExp, int *constExpValue);

    SymbolTableEntry *check_PrimaryExp(Node *node, bool fromConstExp, int *constExpValue);

    SymbolTableEntry *check_Exp(Node *node, bool fromConstExp, int *constExpValue);

    SymbolTableEntry *check_LVal(Node *node, bool fromConstExp, int *constExpValue);

    SymbolTableEntry *check_Number(Node *node, bool fromConstExp, int *constExpValue);

    std::vector<SymbolTableEntry *> *check_FuncRParams(Node *node,
                                                       SymbolTableEntry *entry);

    void check_FuncFParams(Node *funcFParams,
                           SymbolTableEntry *funcIdentEntry);

    SymbolTableEntry *check_FuncFParam(Node *funcFParam, SymbolTableEntryType retType);

    bool findParamError(SymbolTableEntry *definedEntry,
                        std::vector<SymbolTableEntry *> *calledEntry,
                        int lineNum);

    void check_Block(Node *block, bool inFuncBlock = false);

    void check_BlockItem(Node *blockItem, bool inFuncBlock = false);

    void check_Stmt(Node *stmt, bool inFuncBlock = false);

    void check_Cond(Node *node);

    void check_LOrExp(Node *node);

    void check_LAndExp(Node *node);

    void check_EqExp(Node *node);

    void check_RelExp(Node *node);

    void check_VarDef(Node *node);

    static bool check_FormatString(Node *node, int *formatNum);
};


#endif //THIS_PROJECT_ERROR_HANDLER_H
