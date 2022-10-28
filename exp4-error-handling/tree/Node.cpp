#include <iostream>
#include "Node.h"
#include "_debug.h"


Node::Node(GrammarItem grammarItem, Node *parent, int depth)
        : grammarItem(grammarItem), parent(parent), depth(depth), isLeaf(false) {}

Node::Node(GrammarItem grammarItem, int depth)
        : grammarItem(grammarItem), parent(nullptr), depth(depth), isLeaf(false) {}


Node::Node(Token *token, int depth)
        : token(token), parent(nullptr), depth(depth), isLeaf(true),
          grammarItem(GrammarItem::NotGrammarItem) {}


Node::Node(Token *token, Node *parent, int depth)
        : token(token), parent(parent), depth(depth), isLeaf(true),
          grammarItem(GrammarItem::NotGrammarItem) {}

void Node::addChild(Node *child) {
    this->children.push_back(child);
}

Node *Node::getParent() {
    return this->parent;
}

std::vector<Node *> *Node::getAllChildren() {
    return &this->children;
}

void Node::setParent(Node *parent) {
    this->parent = parent;
}

std::string Node::toString() {  // TODO: 需先调用needOutput看看要不要输出
    if (this->isLeaf) {
        return symbol2outputString.find(this->token->symbol)->second
               + " " + this->token->value;
    }
    return "<" + grammarItem2string.find(this->grammarItem)->second + ">";
}

bool Node::needOutput() const {
// 无需输出: BlockItem, BType, Decl
    return !(!this->isLeaf && (
            this->grammarItem == GrammarItem::BlockItem ||
            this->grammarItem == GrammarItem::BType ||
            this->grammarItem == GrammarItem::Decl)
    );
}

bool Node::is(GrammarItem grammarItem) const {
    return this->grammarItem == grammarItem;
}

Node *Node::getFirstChild() {
    return this->children.front();
}

Node *Node::getLastChild() {
    return this->children.back();
}

Node *Node::getChildAt(int i) {
    if (i < 0) {
        return this->children[children.size() + i];
    }
    return this->children[i];
}

unsigned long long Node::getChildrenNum() const {
#ifdef NODE_DEBUG
    std::cout << "\nIn Node::getChildrenNum()\nCurrent node: ";
    if (this->isErrorNode) {
        std::cout << "ErrorNode" << "\n";
    } else if (this->isToken()) {
        if (this->token->lineNumber == 31) {
            int a;
        }
        std::cout << "\"" << this->token->value << "\", line: " << this->token->lineNumber << "\n";
    } else {
        std::cout << grammarItem2string.find(this->grammarItem)->second << "\n";
    }
    std::cout << "children: \n\t";
    for (const Node *child: children) {
        std::cout << std::flush;
        if (child->isErrorNode) {
            std::cout << "ErrorNode" << "\n";
        } else if (child->isToken()) {
            if (this->token->lineNumber == 31) {
                int a;
            }
            std::cout << "\"" << child->token->value << "\", line: " << child->token->lineNumber << "\n\t";
        } else {
            std::cout << grammarItem2string.find(child->grammarItem)->second << "\n\t";
        }
    }
    std::cout << std::endl;
#endif
    return this->children.size();
}

Token *Node::getToken() {
    return this->token;
}

bool Node::is(Symbol symbol) const {
    return symbol == this->token->symbol;
}

bool Node::isToken() const {
    if (isErrorNode) return false;
    return grammarItem == GrammarItem::NotGrammarItem;
}

bool Node::isGrammarItem() const {
    if (isErrorNode) return false;
    return token == nullptr;
}

Node::Node() {
    isErrorNode = true;
}



