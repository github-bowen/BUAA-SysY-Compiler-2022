#ifndef THIS_PROJECT_SYMBOL_TABLE_H
#define THIS_PROJECT_SYMBOL_TABLE_H

#include <map>
#include <vector>
#include "SymbolTableEntry.h"

class SymbolTable {
public:
    SymbolTable(SymbolTable *parent, bool isRoot);

    bool isRoot;
    SymbolTable *parent;
    std::vector<SymbolTable *> children;
    std::map<std::string, SymbolTableEntry *> name2symbolTableEntry;


    bool nameExistedInCurrentTable(Node *node) const;

    bool nameExistedInAllTables(Node *node);

    SymbolTableEntry *getEntryByNameFromAllTables(Node *node);

    void addEntry(const std::string&, SymbolTableEntry *);

    void addChildTable(SymbolTable *child);

    void printAllNames();
};


#endif //THIS_PROJECT_SYMBOL_TABLE_H
