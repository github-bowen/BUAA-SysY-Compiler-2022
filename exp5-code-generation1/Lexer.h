#ifndef LEXER_H
#define LEXER_H

#include <map>
#include <vector>
#include "item/ReservedWord.h"
#include "item/Symbol.h"
#include "item/Token.h"

class Lexer {

    std::string token;
    bool inMultLineComment = false;
    Symbol symbol;
    std::vector<Token*> tokens;

public:
    Lexer() = default;

    std::vector<Token*> &parse();

    Symbol getSym(int lineNum);

    void print();

    void parseLine(int lineNum);
};


#endif //LEXER_H
