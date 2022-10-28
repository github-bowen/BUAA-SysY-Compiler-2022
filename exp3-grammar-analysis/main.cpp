#include <fstream>
#include "Lexer.h"
#include "Parser.h"
#include "tree/Node.h"
#include "exceptions/FileIOError.h"

#define STAGE_GRAMMAR_ANALYSIS

std::ifstream input("../testfile.txt");  // TODO: 修改路径 ../
std::ofstream output("../output.txt");  // TODO: 修改路径 ../

void outputAll(Node *cur) {
    if (!cur->isLeaf) {
        for (auto node: cur->getAllChildren()) {
            outputAll(node);
        }
    }
    if (cur->needOutput()) {
        output << cur->toString() << "\n";
    }
}

int main() {
    if (!input.is_open())
        throw FileIOError("ERROR IN OPENING FILE 'testfile.txt'!");
    if (!output.is_open())
        throw FileIOError("ERROR IN OPENING FILE 'printAll.txt'");

    Lexer lexer(input, output);
    std::vector<Token *> & tokens = lexer.parse();
    Parser parser(tokens);
    Node *root = parser.parse();
#ifdef STAGE_GRAMMAR_ANALYSIS
    outputAll(root);
    output << std::flush;
#endif

    input.close();
    output.close();

    return 0;
}