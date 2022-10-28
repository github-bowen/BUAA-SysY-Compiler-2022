#ifndef MY_COMPILER_LEXER_H
#define MY_COMPILER_LEXER_H

#include <map>
#include "ReservedWord.h"
#include "Symbol.h"

class Lexer {

    std::ifstream &input;
    std::ofstream &output;
    std::string token;
    bool inMultLineComment = false;
    Symbol symbol;

public:
    Lexer(std::ifstream &input, std::ofstream &output);

    void parse();

    Symbol getSym(int lineNum);

    void print();

    void parseLine(int lineNum);
};


#endif //MY_COMPILER_LEXER_H
