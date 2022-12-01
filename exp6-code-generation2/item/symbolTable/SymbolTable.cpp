#include <iostream>
#include "SymbolTable.h"
#include "_debug.h"

SymbolTable::SymbolTable(SymbolTable *parent, bool isRoot) : parent(parent), isRoot(isRoot) {}

bool SymbolTable::nameExistedInCurrentTable(Node *node) const {
    return this->name2symbolTableEntry.count(node->getToken()->value) != 0;
}

bool SymbolTable::nameExistedInAllTables(Node *node) {
    SymbolTable *table = this;
    while (table != nullptr) {
        if (table->nameExistedInCurrentTable(node)) return true;
        table = table->parent;
    }
    return false;
}

SymbolTableEntry *SymbolTable::getEntryByNameFromAllTables(Node *node) {
    // 先调用 nameExistedInAllTables 保证有
    SymbolTable *table = this;
    while (table != nullptr) {
        if (table->nameExistedInCurrentTable(node)) {
            return table->name2symbolTableEntry.find(node->getToken()->value)->second;
        }
        table = table->parent;
    }
    return nullptr;
}

ICItem *SymbolTable::getICItemByNameFromAllTables(Node *node) {
    SymbolTable *table = this;
    while (table != nullptr) {
        if (table->nameExistedInCurrentTable(node)) {
            return table->name2icItem.find(node->getToken()->value)->second;
        }
        table = table->parent;
    }
    return nullptr;
}

void SymbolTable::addEntry(const std::string &name, SymbolTableEntry *entry) {
    assert(name2symbolTableEntry.count(name) == 0);
    this->name2symbolTableEntry.insert({name, entry});
}

void SymbolTable::addICItem(const std::string &name, ICItem *icItem) {
    name2icItem.insert({name, icItem});
}

void SymbolTable::addChildTable(SymbolTable *child) {
    this->children.push_back(child);
}

void SymbolTable::printAllNames() {
    static int i = 1;
    std::cout << "\nTimes for printing: " << i << "\n";
    for (auto &it: name2symbolTableEntry) {
        std::cout << it.first << " ";
    }
    i++;
    std::cout << std::endl;
}
