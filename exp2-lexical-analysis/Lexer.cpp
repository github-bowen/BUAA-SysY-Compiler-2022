#include <fstream>
#include <cctype>
#include "Lexer.h"
#include "ReservedWord.h"
#include "Symbol.h"

// redundant declarations here:
// extern const std::map<std::string, ReservedWord> string2reservedWord;
// extern const std::map<Symbol, std::string> symbol2outputString;
// extern const std::map<ReservedWord, Symbol> reservedWord2symbol;

static std::string curLine;  // 存放读入的当前行
static int curPos = 0;

Lexer::Lexer(std::ifstream &input, std::ofstream &output) : input(input), output(output) {}

void Lexer::print() {
    this->output << symbol2outputString.find(
            this->symbol)->second << " " << this->token << "\n";
}

void Lexer::parse() {
    int lineNum = 0;
    while (std::getline(this->input, curLine)) {
        this->parseLine(++lineNum);
        curLine.clear();
    }
    this->output << std::flush;
}

void Lexer::parseLine(int lineNum) {
    curPos = 0;
    auto length = curLine.size();
    while (curPos < length) {
        this->symbol = this->getSym(lineNum);
        if (this->symbol == Symbol::SINGLE_LINE_COMMENT || this->symbol == Symbol::EMPTY_LINE)
            return;  // 单行注释，后面的也不用看了
        if (this->symbol != Symbol::MULTI_LINE_COMMENT) {
            this->print();  // TODO: 用于词法分析阶段的输出, this->token, this->symbol
        }
        // this->symbol == Symbol::MULTI_LINE_COMMENT: 多行注释，不输出，但要继续看
    }

}

Symbol Lexer::getSym(int lineNum) {  // TODO: 记录 lineNum
    this->token.clear();
    while (isspace(curLine[curPos]) && curPos < curLine.size()) curPos++;
    if (curPos >= curLine.size()) return Symbol::EMPTY_LINE;

    if (!this->inMultLineComment) {  // 不在注释中
        if (isalpha(curLine[curPos]) || curLine[curPos] == '_') {  // ident or reserved word
            while (isalnum(curLine[curPos]) || curLine[curPos] == '_') {
                this->token.push_back(curLine[curPos++]);
            }
            auto it = string2reservedWord.find(this->token);
            if (it == string2reservedWord.end()) {  // not a reserved word
                return Symbol::IDENFR;
            } else {  // reserved word
                return reservedWord2symbol.find(it->second)->second;
            }
        } else if (curLine[curPos] == '\"') {  // "  "
            do {
                this->token.push_back(curLine[curPos++]);
            } while (curLine[curPos] != '\"');
            this->token.push_back(curLine[curPos++]);
            return Symbol::STRCON;
        } else if (curLine[curPos] == '/' && curLine[curPos + 1] == '/') {  // //
            return Symbol::SINGLE_LINE_COMMENT;  // 这行全是注释
        } else if (curLine[curPos] == '/' && curLine[curPos + 1] == '*') {  // /*
            curPos += 2;
            while (true) {
                while (curPos < curLine.size() && curLine[curPos] != '*') curPos++;
                if (curPos >= curLine.size()) {  // 多行注释不在这行结束
                    this->inMultLineComment = true;
                    return Symbol::MULTI_LINE_COMMENT;
                } else if (curLine[curPos + 1] == '/') {
                    curPos += 2;
                    return Symbol::MULTI_LINE_COMMENT;
                } // else: 单独出现一个*
                curPos++;
            }
        } else if (isdigit(curLine[curPos])) {  // 数字，TODO: 没有检测前导0
            do {
                this->token.push_back(curLine[curPos++]);
            } while (isdigit(curLine[curPos]));
            return Symbol::INTCON;
        } else if (curLine[curPos] == '!') {  // !, !=
            this->token.push_back(curLine[curPos]);
            if (curLine[++curPos] != '=') return Symbol::NOT;
            else {
                this->token.push_back(curLine[curPos]);
                curPos++;
                return Symbol::NEQ;
            }
        } else if (curLine[curPos] == '&' && curLine[curPos + 1] == '&') {  // &&
            this->token.append(curLine.substr(curPos, 2));
            curPos += 2;
            return Symbol::AND;
        } else if (curLine[curPos] == '|' && curLine[curPos + 1] == '|') {  // ||
            this->token.append(curLine.substr(curPos, 2));
            curPos += 2;
            return Symbol::OR;
        } else if (curLine[curPos] == '+') {
            this->token.push_back(curLine[curPos++]);
            return Symbol::PLUS;
        } else if (curLine[curPos] == '-') {
            this->token.push_back(curLine[curPos++]);
            return Symbol::MINU;
        } else if (curLine[curPos] == '*') {
            this->token.push_back(curLine[curPos++]);
            return Symbol::MULT;
        } else if (curLine[curPos] == '/') {  // 前面判断过 // 和 /* 了
            this->token.push_back(curLine[curPos++]);
            return Symbol::DIV;
        } else if (curLine[curPos] == '%') {
            this->token.push_back(curLine[curPos++]);
            return Symbol::MOD;
        } else if (curLine[curPos] == '<') {
            this->token.push_back(curLine[curPos++]);
            if (curLine[curPos] != '=') return Symbol::LSS;
            else {
                this->token.push_back(curLine[curPos++]);
                return Symbol::LEQ;
            }
        } else if (curLine[curPos] == '>') {
            this->token.push_back(curLine[curPos++]);
            if (curLine[curPos] != '=') return Symbol::GRE;
            else {
                this->token.push_back(curLine[curPos++]);
                return Symbol::GEQ;
            }
        } else if (curLine[curPos] == '=') {
            this->token.push_back(curLine[curPos++]);
            if (curLine[curPos] != '=') return Symbol::ASSIGN;
            else {
                this->token.push_back(curLine[curPos++]);
                return Symbol::EQL;
            }
        } else if (curLine[curPos] == ';') {
            this->token.push_back(curLine[curPos++]);
            return Symbol::SEMICN;
        } else if (curLine[curPos] == ',') {
            this->token.push_back(curLine[curPos++]);
            return Symbol::COMMA;
        } else if (curLine[curPos] == '(') {
            this->token.push_back(curLine[curPos++]);
            return Symbol::LPARENT;
        } else if (curLine[curPos] == ')') {
            this->token.push_back(curLine[curPos++]);
            return Symbol::RPARENT;
        } else if (curLine[curPos] == '[') {
            this->token.push_back(curLine[curPos++]);
            return Symbol::LBRACK;
        } else if (curLine[curPos] == ']') {
            this->token.push_back(curLine[curPos++]);
            return Symbol::RBRACK;
        } else if (curLine[curPos] == '{') {
            this->token.push_back(curLine[curPos++]);
            return Symbol::LBRACE;
        } else if (curLine[curPos] == '}') {
            this->token.push_back(curLine[curPos++]);
            return Symbol::RBRACE;
        }
        // TODO: ERROR
    } else {  // 在多行注释中
        while (true) {
            while (curPos < curLine.size() && curLine[curPos] != '*') curPos++;
            if (curPos >= curLine.size()) {  // 多行注释不在这行结束
                return Symbol::MULTI_LINE_COMMENT;
            } else if (curLine[curPos + 1] == '/') {
                this->inMultLineComment = false;
                curPos += 2;
                return Symbol::MULTI_LINE_COMMENT;
            } // else: 单独出现一个*
            curPos++;
        }
    }
    return Symbol::EMPTY_LINE;  // TODO: ERROR
}
