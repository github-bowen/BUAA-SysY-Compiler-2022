#ifndef TREE_NODE_H
#define TREE_NODE_H


#include <vector>
#include <string>
#include <fstream>
#include "item/Symbol.h"
#include "item/GrammarItem.h"
#include "item/Token.h"

class Node {
protected:
    std::vector<Node *> children;
    Node *parent;
    int depth;
    GrammarItem grammarItem;  // for non-leaf nodes
    Token *token;  // for leaf nodes

public:
    bool isToken() const;

    bool isGrammarItem() const;

    bool isLeaf;

    bool isErrorNode{false};

    Node();

    Node(GrammarItem grammarItem, Node *parent, int depth);

    Node(GrammarItem grammarItem, int depth);

    Node(Token *token, int depth);

    Node(Token *token, Node *parent, int depth);

    virtual void addChild(Node *child);

    virtual Node *getParent();

    virtual Token *getToken();

    virtual unsigned long long getChildrenNum() const;

    virtual void setParent(Node *parent);

    virtual std::vector<Node *> *getAllChildren();

    virtual Node *getFirstChild();

    virtual Node *getChildAt(int);

    virtual std::string toString();

    virtual bool needOutput() const;

    virtual bool is(GrammarItem) const;

    virtual bool is(Symbol) const;

    Node *getLastChild();
};


#endif //TREE_NODE_H
