#ifndef WHILE_BLOCK
#define WHILE_BLOCK


#include "item/ic/ICItemLabel.h"

class WhileBlock {
public:
    WhileBlock *parent;
    WhileBlock *child;
    const int depth;

    ICItemLabel *whileStartLabel;
    ICItemLabel *whileEndLabel;

    WhileBlock(WhileBlock *parent, ICItemLabel *start, ICItemLabel *end);

    WhileBlock();  // 最开始的 root，设置 depth = 0，表示不在 while 里

    bool inWhile() const;
};


#endif //WHILE_BLOCK
