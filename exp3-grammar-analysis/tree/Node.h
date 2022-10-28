#ifndef TREE_NODE_H
#define TREE_NODE_H


#include <vector>
#include <string>
#include <fstream>
#include "item/Symbol.h"
#include "item/GrammarItem.h"
#include "item/Token.h"

class Node {
    std::vector<Node *> children;
    Node *parent;
    int depth;
    GrammarItem grammarItem;  // for non-leaf nodes
    Token *token;  // for leaf nodes

public:
    bool isLeaf;

    Node(GrammarItem grammarItem, Node *parent, int depth);

    Node(GrammarItem grammarItem, int depth);

    Node(Token *token, int depth);

    Node(Token *token, Node *parent, int depth);

    void addChild(Node *child);

    Node *getParent();

    void setParent(Node *parent);

    std::vector<Node *> getAllChildren();

    std::string toString();

    bool needOutput() const;
};


#endif //TREE_NODE_H
