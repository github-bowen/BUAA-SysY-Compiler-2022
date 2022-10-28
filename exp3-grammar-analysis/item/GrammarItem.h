#ifndef GRAMMAR_ITEM
#define GRAMMAR_ITEM

#include <map>
#include <string>

enum class GrammarItem {
    // 无需输出
    BlockItem,
    BType,
    Decl,
    // 需要输出
    CompUnit,
    ConstDecl,
    VarDecl,
    ConstDef,
    ConstInitVal,
    VarDef,
    InitVal,
    FuncDef,
    MainFuncDef,
    FuncType,
    FuncFParams,
    FuncFParam,
    Block,
    Stmt,
    Exp,
    Cond,
    LVal,
    PrimaryExp,
    Number,
    UnaryExp,
    UnaryOp,
    FuncRParams,
    MulExp,
    AddExp,
    RelExp,
    EqExp,
    LAndExp,
    LOrExp,
    ConstExp
};

const std::map<GrammarItem, std::string> grammarItem2string = {
        {GrammarItem::BlockItem,    "BlockItem"},
        {GrammarItem::BType,        "BType"},
        {GrammarItem::Decl,         "Decl"},
        {GrammarItem::CompUnit,     "CompUnit"},
        {GrammarItem::ConstDecl,    "ConstDecl"},
        {GrammarItem::VarDecl,      "VarDecl"},
        {GrammarItem::ConstDef,     "ConstDef"},
        {GrammarItem::ConstInitVal, "ConstInitVal"},
        {GrammarItem::VarDef,       "VarDef"},
        {GrammarItem::InitVal,      "InitVal"},
        {GrammarItem::FuncDef,      "FuncDef"},
        {GrammarItem::MainFuncDef,  "MainFuncDef"},
        {GrammarItem::FuncType,     "FuncType"},
        {GrammarItem::FuncFParams,  "FuncFParams"},
        {GrammarItem::FuncFParam,   "FuncFParam"},
        {GrammarItem::Block,        "Block"},
        {GrammarItem::Stmt,         "Stmt"},
        {GrammarItem::Exp,          "Exp"},
        {GrammarItem::Cond,         "Cond"},
        {GrammarItem::LVal,         "LVal"},
        {GrammarItem::PrimaryExp,   "PrimaryExp"},
        {GrammarItem::Number,       "Number"},
        {GrammarItem::UnaryExp,     "UnaryExp"},
        {GrammarItem::UnaryOp,      "UnaryOp"},
        {GrammarItem::FuncRParams,  "FuncRParams"},
        {GrammarItem::MulExp,       "MulExp"},
        {GrammarItem::AddExp,       "AddExp"},
        {GrammarItem::RelExp,       "RelExp"},
        {GrammarItem::EqExp,        "EqExp"},
        {GrammarItem::LAndExp,      "LAndExp"},
        {GrammarItem::LOrExp,       "LOrExp"},
        {GrammarItem::ConstExp,     "ConstExp"},
};

#endif //GRAMMAR_ITEM
