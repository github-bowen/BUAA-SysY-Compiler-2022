#include <fstream>
#include "Lexer.h"

int main() {

    std::ifstream input("testfile.txt");  // TODO: 修改路径 ../
    if (!input.is_open())
        throw std::runtime_error("ERROR IN OPENING FILE 'testfile.txt'!");
    std::ofstream output("output.txt");  // TODO: 修改路径 ../
    if (!output.is_open())
        throw std::runtime_error("ERROR IN OPENING FILE 'output.txt'");

    Lexer lexer(input, output);
    lexer.parse();

    input.close();
    output.close();

    return 0;
}