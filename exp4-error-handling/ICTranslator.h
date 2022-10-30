#ifndef INTERMEDIATE_CODE_GENERATOR_H
#define INTERMEDIATE_CODE_GENERATOR_H

#include "tree/Node.h"
#include "item/symbolTable/SymbolTable.h"

/**
 * Intermediate Code Translator 中间代码翻译器
 */
class ICTranslator {
public:
    Node *root;
    SymbolTable *rootTable;

    ICTranslator(Node *, SymbolTable *);

    void translate();
};


#endif //INTERMEDIATE_CODE_GENERATOR_H
