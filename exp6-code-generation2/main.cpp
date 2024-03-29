#include <fstream>
#include <string>
#include "Lexer.h"
#include "Parser.h"
#include "tree/Node.h"
#include "exceptions/FileIOError.h"
#include "ErrorHandler.h"
#include "ICTranslator.h"
#include "MipsTranslator.h"

//#define STAGE_GRAMMAR_ANALYSIS
//#define STAGE_ERROR_HANDLING
//#define STAGE_INTERMEDIATE_CODE
#define STAGE_MIPS

static const std::string pre = "./";  // TODO: 修改路径 ../

std::ifstream input(pre + "testfile.txt");
std::ofstream normalOutput(pre + "output.txt");
std::ofstream errorOutput(pre + "error.txt");
// std::ofstream icOutput(pre + "ic_output.txt");
std::ofstream mipsOutput(pre + "mips.txt");

std::map<int, std::string> errorLog;

static void grammarItemOutput(Node *cur) {
    if (!cur->isLeaf) {
        for (auto node: *(cur->getAllChildren())) {
            grammarItemOutput(node);
        }
    }
    if (cur->needOutput()) {
        normalOutput << cur->toString() << "\n";
    }
}

int main() {
    std::string s = "ss";
    std::map<int, int> a;
    if (!input.is_open())
        throw FileIOError("ERROR IN OPENING FILE 'testfile.txt'!");
    if (!normalOutput.is_open())
        throw FileIOError("ERROR IN OPENING FILE 'printAll.txt'");

    // lexical analyzer 词法分析器
    auto *lexer = new Lexer();
    std::vector<Token *> &tokens = lexer->parse();

    // 语法分析器
    auto *parser = new Parser(tokens);
    Node *root = parser->parse();

    delete lexer;
    delete parser;

#ifdef STAGE_GRAMMAR_ANALYSIS
    grammarItemOutput(root);
    normalOutput << std::flush;
#endif

    // error handler 错误处理器
    auto *errorHandler = new ErrorHandler(root);
    errorHandler->check();
    ICTranslator *icTranslator = errorHandler->icTranslator;

#ifdef STAGE_ERROR_HANDLING
    auto it = errorLog.begin();
    while (it != errorLog.end()) {
        errorOutput << it->first << " " << it->second << "\n";
        ++it;
    }
    errorOutput << std::flush;
#endif

#ifdef STAGE_INTERMEDIATE_CODE
    icTranslator->output();
#endif

    auto *mipsTranslator = new MipsTranslator(icTranslator);
#ifdef STAGE_MIPS
    mipsTranslator->translate();
#endif

    input.close();
    normalOutput.close();
    errorOutput.close();

    delete errorHandler;
    delete root;

    return 0;
}