#ifndef THIS_PROJECT_ERROR_NODE_H
#define THIS_PROJECT_ERROR_NODE_H

#include <string>
#include "Node.h"
#include "item/ErrorType.h"

class ErrorNode : public Node {
public:
    const ErrorType errorType;
    const int lineNum;  // 这个行号为要求输出的行号

    ErrorNode(ErrorType errorType, int lineNum)
            : Node(), errorType(errorType), lineNum(lineNum) {}

    std::string error() const {
        return errorType2string.find(this->errorType)->second;
    }
};

#endif //THIS_PROJECT_ERROR_NODE_H
