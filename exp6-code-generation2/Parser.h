#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include "item/GrammarItem.h"
#include "tree/Node.h"
#include "exceptions/ParseEndError.h"
#include "item/Token.h"

class Parser {
    std::vector<Token *> &tokens;
    int tokenPos;
    const int tokenLength;
    Token *curToken;

    void nextItem() noexcept(false);

public:
    explicit Parser(std::vector<Token *> &tokens);

    Node *parse();

    Node *parse_CompUnit(int depth);

    Node *parse_Decl(int depth);

    Node *parse_FuncDef(int depth);

    Node *parse_MainFuncDef(int depth);

    Node *parse_ConstDecl(int depth);

    Node *parse_VarDecl(int depth);

    Node *parse_BType(int depth);

    Node *parse_ConstDef(int depth);

    Node *parse_Ident(int depth);

    Node *parse_ConstExp(int depth);

    Node *parse_ConstInitVal(int depth);

    Node *parse_VarDef(int depth);

    Node *parse_InitVal(int depth);

    Node *parse_Exp(int depth);

    Node *parse_FuncType(int depth);

    Node *parse_FuncFParams(int depth);

    Node *parse_Block(int depth);

    Node *parse_FuncFParam(int depth);

    Node *parse_BlockItem(int depth);

    Node *parse_Stmt(int depth);

    Node *parse_FormatString(int depth);

    Node *parse_Cond(int depth);

    bool hasAssignInCurrentLine();

    Node *parse_LVal(int depth);

    Node *parse_AddExp(int depth);

    Node *parse_LOrExp(int depth);

    Node *parse_PrimaryExp(int depth);

    Node *parse_Number(int depth);

    Node *parse_IntConst(int depth);

    Node *parse_UnaryExp(int depth);

    Node *parse_UnaryOp(int depth);

    Node *parse_FuncRParams(int depth);

    Node *parse_MulExp(int depth);

    Node *parse_RelExp(int depth);

    Node *parse_EqExp(int depth);

    Node *parse_LAndExp(int depth);
};


#endif //PARSER_H
