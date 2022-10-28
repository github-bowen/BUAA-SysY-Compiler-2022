#ifndef THIS_PROJECT_ERROR_TYPE_H
#define THIS_PROJECT_ERROR_TYPE_H

#include <map>

enum class ErrorType {
    IllegalChar,  // 字符串中非法字符 a
    FormatStrNumNotMatch,  // printf中格式字符与表达式个数不匹配 l

    IdentRedefined,  // 函数名或者变量名在当前作用域下重复定义 b
    IdentUndefined,  // 标识符未定义 c

    ParamNumNotMatch,  // 函数参数个数不匹配 d
    ParamTypeNotMatch,  // 函数参数类型不匹配 e

    ReturnRedundant,  // 无返回值的函数存在不匹配的 return 语句 (注：可以为 "return;" ) f
    ReturnMissing,  // 有返回值的函数缺少 return 语句 g

    ConstantModification,  // 不能改变常量的值 h

    MissingSEMICN,  // 缺少分号 i
    MissingRPARENT,  // 缺少右小括号’)’ j
    MissingRBRACK,  // 缺少右中括号’]’ k

    RedundantBreakContinue  // 在非循环块中使用 break 和 continue 语句 m
};

const std::map<ErrorType, std::string> errorType2string = {
        {ErrorType::IllegalChar,          "a"},
        {ErrorType::IdentRedefined,       "b"},
        {ErrorType::IdentUndefined,       "c"},
        {ErrorType::ParamNumNotMatch,     "d"},
        {ErrorType::ParamTypeNotMatch,    "e"},
        {ErrorType::ReturnRedundant,      "f"},
        {ErrorType::ReturnMissing,        "g"},
        {ErrorType::ConstantModification, "h"},
        {ErrorType::MissingSEMICN,        "i"},
        {ErrorType::MissingRPARENT,       "j"},
        {ErrorType::MissingRBRACK,        "k"},
        {ErrorType::FormatStrNumNotMatch, "l"},
        {ErrorType::RedundantBreakContinue, "m"}
};


#endif //THIS_PROJECT_ERROR_TYPE_H
