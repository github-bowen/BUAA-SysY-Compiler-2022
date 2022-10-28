#include "Node.h"


Node::Node(GrammarItem grammarItem, Node *parent, int depth)
        : grammarItem(grammarItem), parent(parent), depth(depth), isLeaf(false) {}

Node::Node(GrammarItem grammarItem, int depth)
        : grammarItem(grammarItem), parent(nullptr), depth(depth), isLeaf(false) {}


Node::Node(Token *token, int depth)
        : token(token), parent(nullptr), depth(depth), isLeaf(true) {}


Node::Node(Token *token, Node *parent, int depth)
        : token(token), parent(parent), depth(depth), isLeaf(true) {}

void Node::addChild(Node *child) {
    this->children.push_back(child);
}

Node *Node::getParent() {
    return this->parent;
}

std::vector<Node *> Node::getAllChildren() {
    return this->children;
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



