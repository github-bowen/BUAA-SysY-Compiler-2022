#ifndef SYMBOL_H
#define SYMBOL_H

#include <map>

enum class Symbol {  // 类别码：包括包括保留字码
    /* 保留字 */
    MAINTK, // main
    CONSTTK, // item
    INTTK, // int
    BREAKTK, // break
    CONTINUETK, // continue
    IFTK, // if
    ELSETK, // else
    WHILETK, // while
    GETINTTK, // getint
    PRINTFTK, // printf
    RETURNTK, // return
    VOIDTK, // void

    BITANDTK,

    /* 其他 */
    IDENFR, // Ident
    INTCON, // IntConst
    STRCON, // FormatString
    NOT, // !
    AND, // &&
    OR, // ||
    PLUS, // +
    MINU, // -
    MULT, // *
    DIV, // /
    MOD, // %
    LSS, // <
    LEQ, // <=
    GRE, // >
    GEQ, // >=
    EQL, // ==
    NEQ, // !=
    ASSIGN, // =
    SEMICN, // ;
    COMMA, // ,
    LPARENT, // (
    RPARENT, // )
    LBRACK, // [
    RBRACK, // ]
    LBRACE, // {
    RBRACE,  // }
    SINGLE_LINE_COMMENT,  // FIXME: 注意，这是自己添加的
    MULTI_LINE_COMMENT,  // FIXME: 注意，这是自己添加的
    EMPTY_LINE  // FIXME: 注意，这是自己添加的
};

const std::map<Symbol, std::string> symbol2outputString = {  // 用于输出
        {Symbol::MAINTK,     "MAINTK"},
        {Symbol::CONSTTK,    "CONSTTK"},
        {Symbol::INTTK,      "INTTK"},
        {Symbol::BREAKTK,    "BREAKTK"},
        {Symbol::CONTINUETK, "CONTINUETK"},
        {Symbol::IFTK,       "IFTK"},
        {Symbol::ELSETK,     "ELSETK"},
        {Symbol::WHILETK,    "WHILETK"},
        {Symbol::GETINTTK,   "GETINTTK"},
        {Symbol::PRINTFTK,   "PRINTFTK"},
        {Symbol::RETURNTK,   "RETURNTK"},
        {Symbol::VOIDTK,     "VOIDTK"},
        {Symbol::IDENFR,     "IDENFR"},
        {Symbol::INTCON,     "INTCON"},
        {Symbol::STRCON,     "STRCON"},
        {Symbol::NOT,        "NOT"},
        {Symbol::AND,        "AND"},
        {Symbol::OR,         "OR"},
        {Symbol::PLUS,       "PLUS"},
        {Symbol::MINU,       "MINU"},
        {Symbol::MULT,       "MULT"},
        {Symbol::DIV,        "DIV"},
        {Symbol::MOD,        "MOD"},
        {Symbol::LSS,        "LSS"},
        {Symbol::LEQ,        "LEQ"},
        {Symbol::GRE,        "GRE"},
        {Symbol::GEQ,        "GEQ"},
        {Symbol::EQL,        "EQL"},
        {Symbol::NEQ,        "NEQ"},
        {Symbol::ASSIGN,     "ASSIGN"},
        {Symbol::SEMICN,     "SEMICN"},
        {Symbol::COMMA,      "COMMA"},
        {Symbol::LPARENT,    "LPARENT"},
        {Symbol::RPARENT,    "RPARENT"},
        {Symbol::LBRACK,     "LBRACK"},
        {Symbol::RBRACK,     "RBRACK"},
        {Symbol::LBRACE,     "LBRACE"},
        {Symbol::RBRACE,     "RBRACE"},

        {Symbol::BITANDTK,   "BITANDTK"}
};

#endif //SYMBOL_H
