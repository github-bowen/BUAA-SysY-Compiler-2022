#include <fstream>
#include <string>
#include "Lexer.h"
#include "Parser.h"
#include "tree/Node.h"
#include "exceptions/FileIOError.h"
#include "ErrorHandler.h"

// #define STAGE_GRAMMAR_ANALYSIS
#define STAGE_ERROR_HANDLING

std::ifstream input("../testfile.txt");  // TODO: 修改路径 ../
std::ofstream normalOutput("../output.txt");  // TODO: 修改路径 ../
std::ofstream errorOutput("../error.txt");  // TODO: 修改路径 ../
std::map<int, std::string> errorLog;

static bool cmp(const std::pair<int, std::string>& a, const std::pair<int ,std::string> &b) {
    return a.first < b.first;
}

void outputAll(Node *cur) {
    if (!cur->isLeaf) {
        for (auto node: *(cur->getAllChildren())) {
            outputAll(node);
        }
    }
    if (cur->needOutput()) {
        normalOutput << cur->toString() << "\n";
    }
}

int main() {
    if (!input.is_open())
        throw FileIOError("ERROR IN OPENING FILE 'testfile.txt'!");
    if (!normalOutput.is_open())
        throw FileIOError("ERROR IN OPENING FILE 'printAll.txt'");

    Lexer lexer;
    std::vector<Token *> &tokens = lexer.parse();
    Parser parser(tokens);
    Node *root = parser.parse();
#ifdef STAGE_GRAMMAR_ANALYSIS
    outputAll(root);
    normalOutput << std::flush;
#endif
    ErrorHandler errorHandler(root);
    errorHandler.check();
#ifdef STAGE_ERROR_HANDLING
    auto it = errorLog.begin();
    while (it != errorLog.end()) {
        errorOutput << it->first << " " << it->second << "\n";
        ++it;
    }
    errorOutput << std::flush;
#endif
    input.close();
    normalOutput.close();
    errorOutput.close();

    return 0;
}