#ifndef RESERVED_WORD_H
#define RESERVED_WORD_H

#include <map>
#include "Symbol.h"

enum class ReservedWord {  // 保留字码
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
    VOIDTK // void
};

const std::map<std::string, ReservedWord> string2reservedWord = {
        {"main",     ReservedWord::MAINTK},
        {"const",    ReservedWord::CONSTTK},
        {"int",      ReservedWord::INTTK},
        {"break",    ReservedWord::BREAKTK},
        {"if",       ReservedWord::IFTK},
        {"continue", ReservedWord::CONTINUETK},
        {"else",     ReservedWord::ELSETK},
        {"while",    ReservedWord::WHILETK},
        {"getint",   ReservedWord::GETINTTK},
        {"printf",   ReservedWord::PRINTFTK},
        {"return",   ReservedWord::RETURNTK},
        {"void",     ReservedWord::VOIDTK}
};

const std::map<ReservedWord, Symbol> reservedWord2symbol = {
        {ReservedWord::MAINTK,     Symbol::MAINTK},
        {ReservedWord::CONSTTK,    Symbol::CONSTTK},
        {ReservedWord::INTTK,      Symbol::INTTK},
        {ReservedWord::IFTK,       Symbol::IFTK},
        {ReservedWord::CONTINUETK, Symbol::CONTINUETK},
        {ReservedWord::ELSETK,     Symbol::ELSETK},
        {ReservedWord::WHILETK,    Symbol::WHILETK},
        {ReservedWord::GETINTTK,   Symbol::GETINTTK},
        {ReservedWord::PRINTFTK,   Symbol::PRINTFTK},
        {ReservedWord::RETURNTK,   Symbol::RETURNTK},
        {ReservedWord::VOIDTK,     Symbol::VOIDTK},
        {ReservedWord::BREAKTK,    Symbol::BREAKTK}
};

#endif //RESERVED_WORD_H
