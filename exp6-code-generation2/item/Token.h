#ifndef TOKEN_H
#define TOKEN_H

#include "Symbol.h"

struct Token {
    Token(Symbol symbol, std::string& value, int i) {
        this->symbol = symbol;
        this->value = value;
        this->lineNumber = i;
    }

    Symbol symbol;
    std::string value;
    int lineNumber;
};

#endif //TOKEN_H
