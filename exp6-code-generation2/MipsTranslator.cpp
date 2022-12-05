#include "MipsTranslator.h"

#include <fstream>
#include "_debug.h"
#include <string>

extern std::ofstream mipsOutput;
static bool inSelfDefinedFunc = false;

MipsTranslator::MipsTranslator(ICTranslator *icTranslator)
        : icTranslator(icTranslator) {
    regUsage = {{Reg::$zero, false},
                {Reg::$at,   false},
                {Reg::$v0,   false},
                {Reg::$v1,   false},
                {Reg::$a0,   false},
                {Reg::$a1,   false},
                {Reg::$a2,   false},
                {Reg::$a3,   false},
                {Reg::$t0,   false},
                {Reg::$t1,   false},
                {Reg::$t2,   false},
                {Reg::$t3,   false},
                {Reg::$t4,   false},
                {Reg::$t5,   false},
                {Reg::$t6,   false},
                {Reg::$t7,   false},
                {Reg::$t8,   false},
                {Reg::$t9,   false},
                {Reg::$s0,   false},
                {Reg::$s1,   false},
                {Reg::$s2,   false},
                {Reg::$s3,   false},
                {Reg::$s4,   false},
                {Reg::$s5,   false},
                {Reg::$s6,   false},
                {Reg::$s7,   false},
                {Reg::$k0,   false},
                {Reg::$k1,   false},
                {Reg::$gp,   false},
                {Reg::$sp,   false},
                {Reg::$fp,   false},
                {Reg::$ra,   false}
    };
}

void MipsTranslator::translate() {
    std::vector<ICEntry *> *mainStream = icTranslator->mainEntries;
    std::map<int, std::string *> *id2allPureString = icTranslator->id2allPureString;
    std::map<std::string *, ICItemFunc *> *name2icItemFunc = icTranslator->name2icItemFunc;

    int i = 0;
    const auto mainEntryNum = mainStream->size();

    // .data起始地址 0x10010000 (16进制)     268500992 (10进制)
    mipsOutput << ".data 0x10010000\n";

    mipsOutput << "temp:  .space  160000\n\n";  // 临时内存区，起始地址为0x10010000 (16) or 268500992 (10)

    // 全局变量、常量
    while (mainStream->at(i)->entryType != ICEntryType::MainFuncStart) {
        ICEntry *defEntry = mainStream->at(i);
//        assert(defEntry->isGlobalVarOrConstDef());
        translate_GlobalVarOrArrayDef(defEntry);
        i++;
    }
    // 字符串片段(纯字符串部分)定义
    mipsOutput << "\n# string tokens: \n";
    for (const auto &item: *id2allPureString) {
        const int id = item.first;
        const std::string *str = item.second;
        mipsOutput << strId2label(id) << ":  .asciiz   \"" << *str << "\"\n";
    }
    // 主函数部分
    assert(mainStream->at(i)->entryType == ICEntryType::MainFuncStart);
    i++;
    mipsOutput << "\n\n.text 0x00400000\n\n# main function\n";
    while (i < mainEntryNum) {
#ifdef MIPS_DEBUG
        mipsOutput << std::flush;
#endif
        ICEntry *entry = mainStream->at(i);
        ICItem *op1 = entry->operator1, *op2 = entry->operator2, *op3 = entry->operator3;
        const int opNum = entry->opNum;
        switch (entry->entryType) {
            case ICEntryType::VarDefine: {  // 局部变量
                const bool hasInitValue = op2 != nullptr;
                auto *var = (ICItemVar *) op1;
                localVarId2mem.insert({var->varId, tempStackAddressTop});
                tempStackAddressTop += 4;
                if (hasInitValue) {
                    assert(op2->type == ICItemType::Var);
                    auto *rightValue = (ICItemVar *) op2;
                    lw(Reg::$t0, rightValue);
                    sw(Reg::$t0, var);
                }
                break;
            }
            case ICEntryType::ConstVarDefine: {  // 局部常量
                auto *constVar = (ICItemVar *) op1;
                localVarId2mem.insert({constVar->varId, tempStackAddressTop});
                tempStackAddressTop += 4;
                const int initValue = constVar->value;
                li(Reg::$t0, initValue);
                sw(Reg::$t0, constVar);
                break;
            }
            case ICEntryType::ArrayDefine: {  // 局部数组
                const bool hasInitValue = op2 != nullptr;
                auto *array = (ICItemArray *) op1;
                const int firstAddress = tempStackAddressTop;
                localArrayId2mem.insert({array->arrayId, tempStackAddressTop});
                tempStackAddressTop += 4 * array->length;
                if (hasInitValue) {
                    li(Reg::$t1, firstAddress);  // 数组基地址
                    for (int j = 0; j < array->length; ++j) {
                        ICItemVar *rightValue = ((ICItemArray *) op2)->itemsToInitArray->at(j);
                        lw(Reg::$t0, rightValue);
                        sw(Reg::$t0, j * 4, Reg::$t1);
                    }
                }
                break;
            }
            case ICEntryType::ConstArrayDefine: {
                auto *array = (ICItemArray *) op1;
                const int firstAddress = tempStackAddressTop;
                localArrayId2mem.insert({array->arrayId, tempStackAddressTop});
                tempStackAddressTop += 4 * array->length;

                li(Reg::$t1, firstAddress);  // 数组基地址
                for (int j = 0; j < array->length; ++j) {
                    const int rightValue = array->value[j];
                    li(Reg::$t0, rightValue);
                    sw(Reg::$t0, j * 4, Reg::$t1);
                }
                break;
            }
            case ICEntryType::FuncCall: {
                auto *calledFunc = (const ICItemFunc *) (entry->calledFunc);
                pushTempReg();
                pushParams(entry->params);
                mipsOutput << "# Call function!\n";
                jal(calledFunc);
                lw(Reg::$ra, 0, Reg::$sp);
                mipsOutput << "# Pop params\n";
                addiu(Reg::$sp, Reg::$sp, 1000);
                popTempReg();
                mipsOutput << "\n";
                break;
            }
            case ICEntryType::FuncReturnWithValue:  // 主函数结束
            case ICEntryType::FuncReturn:
            case ICEntryType::MainFuncEnd:
                exit();
                break;
            case ICEntryType::Getint: {
                auto *dst = ((ICItemVar *) op1);
                getint(dst);
                break;
            }
            case ICEntryType::Print: {
                auto *itemString = ((ICItemString *) op1);
                for (const auto *strItem: *(itemString->stringItems)) {
                    if (strItem->isString) {
                        const int strId = strItem->pureStringId;
                        printStr(strId);
                    } else {
                        const ICItem *icItem = strItem->intItem;
                        assert(icItem->type == ICItemType::Var);
                        printInt((ICItemVar *) icItem);
                    }
                }
                break;
            }
            case ICEntryType::Assign: {
                auto *left = (ICItemVar *) op1, *right = (ICItemVar *) op2;
                if (right == nullptr) {  // 从函数返回后赋值
                    sw(Reg::$v0, left);
                } else {
                    lw(Reg::$t0, right);
                    sw(Reg::$t0, left);
                }
                break;
            }
            case ICEntryType::Add: {
                auto *dst = (ICItemVar *) op1, *r1 = (ICItemVar *) op2, *r2 = (ICItemVar *) op3;
                if (r1->isConst && r2->isConst) {
                    const int right = r1->value + r2->value;
                    li(Reg::$t0, right);
                    sw(Reg::$t0, dst);
                } else if (r1->isConst) {
                    li(Reg::$t0, r1->value);
                    lw(Reg::$t1, r2);
                    addu(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                } else {
                    lw(Reg::$t0, r1);
                    lw(Reg::$t1, r2);
                    addu(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                }
                break;
            }
            case ICEntryType::Sub: {
                auto *dst = (ICItemVar *) op1, *r1 = (ICItemVar *) op2, *r2 = (ICItemVar *) op3;
                if (r1->isConst && r2->isConst) {
                    const int right = r1->value - r2->value;
                    li(Reg::$t0, right);
                    sw(Reg::$t0, dst);
                } else if (r1->isConst) {
                    li(Reg::$t0, r1->value);
                    lw(Reg::$t1, r2);
                    subu(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                } else {
                    lw(Reg::$t0, r1);
                    lw(Reg::$t1, r2);
                    subu(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                }
                break;
            }
            case ICEntryType::Mul: {
                auto *dst = (ICItemVar *) op1, *r1 = (ICItemVar *) op2, *r2 = (ICItemVar *) op3;
                if (r1->isConst && r2->isConst) {
                    const int right = r1->value * r2->value;
                    li(Reg::$t0, right);
                    sw(Reg::$t0, dst);
                } else if (r1->isConst) {
                    li(Reg::$t0, r1->value);
                    lw(Reg::$t1, r2);
                    mul(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                } else {
                    lw(Reg::$t0, r1);
                    lw(Reg::$t1, r2);
                    mul(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                }
                break;
            }
            case ICEntryType::Div: {
                auto *dst = (ICItemVar *) op1, *r1 = (ICItemVar *) op2, *r2 = (ICItemVar *) op3;
                if (r1->isConst && r2->isConst) {
                    const int right = r1->value / r2->value;
                    li(Reg::$t0, right);
                    sw(Reg::$t0, dst);
                } else if (r1->isConst) {
                    li(Reg::$t0, r1->value);
                    lw(Reg::$t1, r2);
                    div(Reg::$t0, Reg::$t1);
                    mflo(Reg::$t2);
                    sw(Reg::$t2, dst);
                } else {
                    lw(Reg::$t0, r1);
                    lw(Reg::$t1, r2);
                    div(Reg::$t0, Reg::$t1);
                    mflo(Reg::$t2);
                    sw(Reg::$t2, dst);
                }
                break;
            }
            case ICEntryType::Mod: {
                auto *dst = (ICItemVar *) op1, *r1 = (ICItemVar *) op2, *r2 = (ICItemVar *) op3;
                if (r1->isConst && r2->isConst) {
                    const int right = r1->value % r2->value;
                    li(Reg::$t0, right);
                    sw(Reg::$t0, dst);
                } else if (r1->isConst) {
                    li(Reg::$t0, r1->value);
                    lw(Reg::$t1, r2);
                    div(Reg::$t0, Reg::$t1);
                    mfhi(Reg::$t2);
                    sw(Reg::$t2, dst);
                } else {
                    lw(Reg::$t0, r1);
                    lw(Reg::$t1, r2);
                    div(Reg::$t0, Reg::$t1);
                    mfhi(Reg::$t2);
                    sw(Reg::$t2, dst);
                }
                break;
            }
            case ICEntryType::Neg: {
                auto *dst = (ICItemVar *) op1, *src = (ICItemVar *) op2;
                if (src->isConst) {
                    li(Reg::$t0, -src->value);
                    sw(Reg::$t0, dst);
                } else {
                    lw(Reg::$t0, src);
                    subu(Reg::$t0, Reg::$zero, Reg::$t0);
                    sw(Reg::$t0, dst);
                }
                break;
            }
            case ICEntryType::Not: {
                auto *dst = (ICItemVar *) op1, *src = (ICItemVar *) op2;
                if (src->isConst) {
                    assert(op2->type == ICItemType::Var);
                    li(Reg::$t0, !src->value);
                    sw(Reg::$t0, dst);
                } else {
                    lw(Reg::$t0, src);
                    seq(Reg::$t0, Reg::$zero, Reg::$t0);
                    sw(Reg::$t0, dst);
                }
                break;
            }
            case ICEntryType::Or: {
                auto *dst = (ICItemVar *) op1, *r1 = (ICItemVar *) op2, *r2 = (ICItemVar *) op3;
                if (r1->isConst && r2->isConst) {
                    const int right = r1->value || r2->value;
                    li(Reg::$t0, right);
                    sw(Reg::$t0, dst);
                } else if (r1->isConst) {
                    li(Reg::$t0, r1->value);
                    lw(Reg::$t1, r2);
                    _or(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                } else {
                    lw(Reg::$t0, r1);
                    lw(Reg::$t1, r2);
                    _or(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                }
                break;
            }
            case ICEntryType::And: {
                auto *dst = (ICItemVar *) op1, *r1 = (ICItemVar *) op2, *r2 = (ICItemVar *) op3;
                if (r1->isConst && r2->isConst) {
                    const int right = r1->value && r2->value;
                    li(Reg::$t0, right);
                    sw(Reg::$t0, dst);
                } else if (r1->isConst) {
                    li(Reg::$t0, r1->value);
                    lw(Reg::$t1, r2);
                    _and(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                } else {
                    lw(Reg::$t0, r1);
                    lw(Reg::$t1, r2);
                    _and(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                }
                break;
            }
            case ICEntryType::Equal: {
                auto *dst = (ICItemVar *) op1, *r1 = (ICItemVar *) op2, *r2 = (ICItemVar *) op3;
                if (r1->isConst && r2->isConst) {
                    const int right = r1->value == r2->value;
                    li(Reg::$t0, right);
                    sw(Reg::$t0, dst);
                } else if (r1->isConst) {
                    li(Reg::$t0, r1->value);
                    lw(Reg::$t1, r2);
                    seq(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                } else {
                    lw(Reg::$t0, r1);
                    lw(Reg::$t1, r2);
                    seq(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                }
                break;
            }
            case ICEntryType::NotEqual: {
                auto *dst = (ICItemVar *) op1, *r1 = (ICItemVar *) op2, *r2 = (ICItemVar *) op3;
                if (r1->isConst && r2->isConst) {
                    const int right = r1->value != r2->value;
                    li(Reg::$t0, right);
                    sw(Reg::$t0, dst);
                } else if (r1->isConst) {
                    li(Reg::$t0, r1->value);
                    lw(Reg::$t1, r2);
                    sne(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                } else {
                    lw(Reg::$t0, r1);
                    lw(Reg::$t1, r2);
                    sne(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                }
                break;
            }
            case ICEntryType::LessEqual: {
                auto *dst = (ICItemVar *) op1, *r1 = (ICItemVar *) op2, *r2 = (ICItemVar *) op3;
                if (r1->isConst && r2->isConst) {
                    const int right = r1->value <= r2->value;
                    li(Reg::$t0, right);
                    sw(Reg::$t0, dst);
                } else if (r1->isConst) {
                    li(Reg::$t0, r1->value);
                    lw(Reg::$t1, r2);
                    sle(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                } else {
                    lw(Reg::$t0, r1);
                    lw(Reg::$t1, r2);
                    sle(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                }
                break;
            }
            case ICEntryType::LessThan: {
                auto *dst = (ICItemVar *) op1, *r1 = (ICItemVar *) op2, *r2 = (ICItemVar *) op3;
                if (r1->isConst && r2->isConst) {
                    const int right = r1->value < r2->value;
                    li(Reg::$t0, right);
                    sw(Reg::$t0, dst);
                } else if (r1->isConst) {
                    li(Reg::$t0, r1->value);
                    lw(Reg::$t1, r2);
                    slt(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                } else {
                    lw(Reg::$t0, r1);
                    lw(Reg::$t1, r2);
                    slt(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                }
                break;
            }
            case ICEntryType::GreaterThan: {
                auto *dst = (ICItemVar *) op1, *r1 = (ICItemVar *) op2, *r2 = (ICItemVar *) op3;
                if (r1->isConst && r2->isConst) {
                    const int right = r1->value > r2->value;
                    li(Reg::$t0, right);
                    sw(Reg::$t0, dst);
                } else if (r1->isConst) {
                    li(Reg::$t0, r1->value);
                    lw(Reg::$t1, r2);
                    sgt(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                } else {
                    lw(Reg::$t0, r1);
                    lw(Reg::$t1, r2);
                    sgt(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                }
                break;
            }
            case ICEntryType::GreaterEqual: {
                auto *dst = (ICItemVar *) op1, *r1 = (ICItemVar *) op2, *r2 = (ICItemVar *) op3;
                if (r1->isConst && r2->isConst) {
                    const int right = r1->value >= r2->value;
                    li(Reg::$t0, right);
                    sw(Reg::$t0, dst);
                } else if (r1->isConst) {
                    li(Reg::$t0, r1->value);
                    lw(Reg::$t1, r2);
                    sge(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                } else {
                    lw(Reg::$t0, r1);
                    lw(Reg::$t1, r2);
                    sge(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                }
                break;
            }
            case ICEntryType::Beqz: {
                auto *condition = (ICItemVar *) op1;
                auto *label = (ICItemLabel *) op2;
                if (condition->isConst) {
                    if (condition->value == 0) {
                        j(label);
                    }
                } else {
                    lw(Reg::$t0, condition);
                    beqz(Reg::$t0, label);
                }
                break;
            }
            case ICEntryType::JumpLabel: {
                auto *label = (ICItemLabel *) op1;
                j(label);
                break;
            }
            case ICEntryType::InsertLabel:
                insertLabel((ICItemLabel *) op1);
                break;
//            case ICEntryType::FuncDefine:
//            case ICEntryType::MainFuncStart:
        }
        i++;
    }
    inSelfDefinedFunc = true;
    mipsOutput << "\n\n\n\n" << "# self defined functions\n\n";
    for (const auto &item: *name2icItemFunc) {
        ICItemFunc *func = item.second;
        mipsOutput << "# ---------------- FUCNTION: " << *func->originName << " ------------------\n";
        mipsOutput << func->funcLabel->toString() << ":\n\n";
        translate_FuncDef(func);
        mipsOutput << "\n\n\n";
    }
#ifdef MIPS_DEBUG
    mipsOutput << std::flush;
#endif
}

void MipsTranslator::translate_FuncDef(ICItemFunc *func) {
    inSelfDefinedFunc = true;
    clearLocalAndTempInFunc();
    funcFArrayParamId2offset.clear();
    funcFVarParamId2offset.clear();
    std::vector<ICItem *> *params = func->params;
    const int num = params->size();

    int offset = 4;  // offset = 0处放 ra
    for (const ICItem *param: *params) {
        if (param->type == ICItemType::Var) {
            auto *var = ((ICItemVar *) param);
            funcFVarParamId2offset.insert({var->varId, offset});  // varId 为负数
            offset += 4;
        } else {
            auto *array = (ICItemArray *) param;
            funcFArrayParamId2offset.insert({array->arrayId, offset});  // arrayId 为正数
            offset += 4;
        }
    }
    tempFuncStackOffsetTop = tempFuncStackOffsetBase + offset;

    std::vector<ICEntry *> *funcEntries = func->entries;
    int i = 0;
    bool findReturn = false;
    while (i < funcEntries->size()) {
#ifdef MIPS_DEBUG
        mipsOutput << std::flush;
#endif
        ICEntry *entry = funcEntries->at(i);
        ICItem *op1 = entry->operator1, *op2 = entry->operator2, *op3 = entry->operator3;
        switch (entry->entryType) {
            case ICEntryType::VarDefine: {  // 局部变量
                const bool hasInitValue = op2 != nullptr;
                auto *var = (ICItemVar *) op1;
                localVarId2offset.insert({var->varId, tempFuncStackOffsetTop});
                tempFuncStackOffsetTop += 4;
                if (hasInitValue) {
                    assert(op2->type == ICItemType::Var);
                    auto *rightValue = (ICItemVar *) op2;
                    lw(Reg::$t0, rightValue);
                    sw(Reg::$t0, var);
                }
                break;
            }
            case ICEntryType::ConstVarDefine: {  // 局部常量
                auto *constVar = (ICItemVar *) op1;
                localVarId2offset.insert({constVar->varId, tempFuncStackOffsetTop});
                tempFuncStackOffsetTop += 4;
                const int initValue = constVar->value;
                li(Reg::$t0, initValue);
                sw(Reg::$t0, constVar);
                break;
            }
            case ICEntryType::ArrayDefine: {  // 局部数组
                const bool hasInitValue = op2 != nullptr;
                auto *array = (ICItemArray *) op1;
                const int firstAddress = tempFuncStackOffsetTop;
                localArrayId2offset.insert({array->arrayId, tempFuncStackOffsetTop});
                tempFuncStackOffsetTop += 4 * array->length;
                if (hasInitValue) {
                    li(Reg::$t1, firstAddress);  // 数组基地址
                    for (int j = 0; j < array->length; ++j) {
                        ICItemVar *rightValue = ((ICItemArray *) op2)->itemsToInitArray->at(j);
                        lw(Reg::$t0, rightValue);
                        sw(Reg::$t0, j * 4, Reg::$t1);
                    }
                }
                break;
            }
            case ICEntryType::ConstArrayDefine: {
                auto *array = (ICItemArray *) op1;
                const int firstAddress = tempFuncStackOffsetTop;
                localArrayId2offset.insert({array->arrayId, tempFuncStackOffsetTop});
                tempFuncStackOffsetTop += 4 * array->length;

                li(Reg::$t1, firstAddress);  // 数组基地址
                for (int j = 0; j < array->length; ++j) {
                    const int rightValue = array->value[j];
                    li(Reg::$t0, rightValue);
                    sw(Reg::$t0, j * 4, Reg::$t1);
                }
                break;
            }
                // case ICEntryType::FuncDefine:  // 非法，不应该出现这个
            case ICEntryType::FuncCall: {
                auto *calledFunc = (const ICItemFunc *) (entry->calledFunc);
                pushTempReg();
                pushParams(entry->params);
                mipsOutput << "# Call function!\n";
                jal(calledFunc);
                lw(Reg::$ra, 0, Reg::$sp);
                mipsOutput << "# Pop params\n";
                addiu(Reg::$sp, Reg::$sp, 1000);
                popTempReg();
                mipsOutput << "\n";
                break;
            }
            case ICEntryType::FuncReturnWithValue: {
                findReturn = true;
                if (op1->type == ICItemType::Imm) {
                    li(Reg::$v0, ((ICItemImm *) op1)->value);
                } else {
                    lw(Reg::$v0, ((ICItemVar *) op1));
                }
                jr();
                break;
            }
            case ICEntryType::FuncReturn:
                findReturn = true;
                // case ICEntryType::MainFuncEnd:
                jr();
                break;
            case ICEntryType::Getint: {
                auto *dst = ((ICItemVar *) op1);
                getint(dst);
                break;
            }
            case ICEntryType::Print: {
                auto *itemString = ((ICItemString *) op1);
                for (const auto *strItem: *(itemString->stringItems)) {
                    if (strItem->isString) {
                        const int strId = strItem->pureStringId;
                        printStr(strId);
                    } else {
                        const ICItem *icItem = strItem->intItem;
                        assert(icItem->type == ICItemType::Var);
                        printInt((ICItemVar *) icItem);
                    }
                }
                break;
            }
            case ICEntryType::Assign: {
                auto *left = (ICItemVar *) op1, *right = (ICItemVar *) op2;
                if (right == nullptr) {
                    // 从函数返回后赋值
                    sw(Reg::$v0, left);
                } else {
                    lw(Reg::$t0, right);
                    sw(Reg::$t0, left);
                }
                break;
            }
            case ICEntryType::Add: {
                auto *dst = (ICItemVar *) op1, *r1 = (ICItemVar *) op2, *r2 = (ICItemVar *) op3;
                if (r1->isConst && r2->isConst) {
                    const int right = r1->value + r2->value;
                    li(Reg::$t0, right);
                    sw(Reg::$t0, dst);
                } else if (r1->isConst) {
                    li(Reg::$t0, r1->value);
                    lw(Reg::$t1, r2);
                    addu(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                } else {
                    lw(Reg::$t0, r1);
                    lw(Reg::$t1, r2);
                    addu(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                }
                break;
            }
            case ICEntryType::Sub: {
                auto *dst = (ICItemVar *) op1, *r1 = (ICItemVar *) op2, *r2 = (ICItemVar *) op3;
                if (r1->isConst && r2->isConst) {
                    const int right = r1->value - r2->value;
                    li(Reg::$t0, right);
                    sw(Reg::$t0, dst);
                } else if (r1->isConst) {
                    li(Reg::$t0, r1->value);
                    lw(Reg::$t1, r2);
                    subu(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                } else {
                    lw(Reg::$t0, r1);
                    lw(Reg::$t1, r2);
                    subu(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                }
                break;
            }
            case ICEntryType::Mul: {
                auto *dst = (ICItemVar *) op1, *r1 = (ICItemVar *) op2, *r2 = (ICItemVar *) op3;
                if (r1->isConst && r2->isConst) {
                    const int right = r1->value * r2->value;
                    li(Reg::$t0, right);
                    sw(Reg::$t0, dst);
                } else if (r1->isConst) {
                    li(Reg::$t0, r1->value);
                    lw(Reg::$t1, r2);
                    mul(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                } else {
                    lw(Reg::$t0, r1);
                    lw(Reg::$t1, r2);
                    mul(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                }
                break;
            }
            case ICEntryType::Div: {
                auto *dst = (ICItemVar *) op1, *r1 = (ICItemVar *) op2, *r2 = (ICItemVar *) op3;
                if (r1->isConst && r2->isConst) {
                    const int right = r1->value / r2->value;
                    li(Reg::$t0, right);
                    sw(Reg::$t0, dst);
                } else if (r1->isConst) {
                    li(Reg::$t0, r1->value);
                    lw(Reg::$t1, r2);
                    div(Reg::$t0, Reg::$t1);
                    mflo(Reg::$t2);
                    sw(Reg::$t2, dst);
                } else {
                    lw(Reg::$t0, r1);
                    lw(Reg::$t1, r2);
                    div(Reg::$t0, Reg::$t1);
                    mflo(Reg::$t2);
                    sw(Reg::$t2, dst);
                }
                break;
            }
            case ICEntryType::Mod: {
                auto *dst = (ICItemVar *) op1, *r1 = (ICItemVar *) op2, *r2 = (ICItemVar *) op3;
                if (r1->isConst && r2->isConst) {
                    const int right = r1->value % r2->value;
                    li(Reg::$t0, right);
                    sw(Reg::$t0, dst);
                } else if (r1->isConst) {
                    li(Reg::$t0, r1->value);
                    lw(Reg::$t1, r2);
                    div(Reg::$t0, Reg::$t1);
                    mfhi(Reg::$t2);
                    sw(Reg::$t2, dst);
                } else {
                    lw(Reg::$t0, r1);
                    lw(Reg::$t1, r2);
                    div(Reg::$t0, Reg::$t1);
                    mfhi(Reg::$t2);
                    sw(Reg::$t2, dst);
                }
                break;
            }
            case ICEntryType::Not: {
                auto *dst = (ICItemVar *) op1, *src = (ICItemVar *) op2;
                if (src->isConst) {
                    assert(op2->type == ICItemType::Var);
                    li(Reg::$t0, !src->value);
                    sw(Reg::$t0, dst);
                } else {
                    lw(Reg::$t0, src);
                    seq(Reg::$t0, Reg::$zero, Reg::$t0);
                    sw(Reg::$t0, dst);
                }
                break;
            }
            case ICEntryType::Neg: {
                auto *dst = (ICItemVar *) op1, *src = (ICItemVar *) op2;
                if (src->isConst) {
                    li(Reg::$t0, -src->value);
                    sw(Reg::$t0, dst);
                } else {
                    lw(Reg::$t0, src);
                    subu(Reg::$t0, Reg::$zero, Reg::$t0);
                    sw(Reg::$t0, dst);
                }
                break;
            }
//            case ICEntryType::FuncDefine:
//                break;
//            case ICEntryType::MainFuncStart:
//                break;
//            case ICEntryType::MainFuncEnd:
//                break;
            case ICEntryType::Or: {
                auto *dst = (ICItemVar *) op1, *r1 = (ICItemVar *) op2, *r2 = (ICItemVar *) op3;
                if (r1->isConst && r2->isConst) {
                    const int right = r1->value || r2->value;
                    li(Reg::$t0, right);
                    sw(Reg::$t0, dst);
                } else if (r1->isConst) {
                    li(Reg::$t0, r1->value);
                    lw(Reg::$t1, r2);
                    _or(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                } else {
                    lw(Reg::$t0, r1);
                    lw(Reg::$t1, r2);
                    _or(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                }
                break;
            }
            case ICEntryType::And: {
                auto *dst = (ICItemVar *) op1, *r1 = (ICItemVar *) op2, *r2 = (ICItemVar *) op3;
                if (r1->isConst && r2->isConst) {
                    const int right = r1->value && r2->value;
                    li(Reg::$t0, right);
                    sw(Reg::$t0, dst);
                } else if (r1->isConst) {
                    li(Reg::$t0, r1->value);
                    lw(Reg::$t1, r2);
                    _and(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                } else {
                    lw(Reg::$t0, r1);
                    lw(Reg::$t1, r2);
                    _and(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                }
                break;
            }
            case ICEntryType::Equal: {
                auto *dst = (ICItemVar *) op1, *r1 = (ICItemVar *) op2, *r2 = (ICItemVar *) op3;
                if (r1->isConst && r2->isConst) {
                    const int right = r1->value == r2->value;
                    li(Reg::$t0, right);
                    sw(Reg::$t0, dst);
                } else if (r1->isConst) {
                    li(Reg::$t0, r1->value);
                    lw(Reg::$t1, r2);
                    seq(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                } else {
                    lw(Reg::$t0, r1);
                    lw(Reg::$t1, r2);
#ifdef MIPS_DEBUG
                    mipsOutput << std::flush;
#endif
                    seq(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                }
                break;
            }
            case ICEntryType::NotEqual: {
                auto *dst = (ICItemVar *) op1, *r1 = (ICItemVar *) op2, *r2 = (ICItemVar *) op3;
                if (r1->isConst && r2->isConst) {
                    const int right = r1->value != r2->value;
                    li(Reg::$t0, right);
                    sw(Reg::$t0, dst);
                } else if (r1->isConst) {
                    li(Reg::$t0, r1->value);
                    lw(Reg::$t1, r2);
                    sne(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                } else {
                    lw(Reg::$t0, r1);
                    lw(Reg::$t1, r2);
                    sne(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                }
                break;
            }
            case ICEntryType::LessEqual: {
                auto *dst = (ICItemVar *) op1, *r1 = (ICItemVar *) op2, *r2 = (ICItemVar *) op3;
                if (r1->isConst && r2->isConst) {
                    const int right = r1->value <= r2->value;
                    li(Reg::$t0, right);
                    sw(Reg::$t0, dst);
                } else if (r1->isConst) {
                    li(Reg::$t0, r1->value);
                    lw(Reg::$t1, r2);
                    sle(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                } else {
                    lw(Reg::$t0, r1);
                    lw(Reg::$t1, r2);
                    sle(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                }
                break;
            }
            case ICEntryType::LessThan: {
                auto *dst = (ICItemVar *) op1, *r1 = (ICItemVar *) op2, *r2 = (ICItemVar *) op3;
                if (r1->isConst && r2->isConst) {
                    const int right = r1->value < r2->value;
                    li(Reg::$t0, right);
                    sw(Reg::$t0, dst);
                } else if (r1->isConst) {
                    li(Reg::$t0, r1->value);
                    lw(Reg::$t1, r2);
                    slt(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                } else {
                    lw(Reg::$t0, r1);
                    lw(Reg::$t1, r2);
                    slt(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                }
                break;
            }
            case ICEntryType::GreaterThan: {
                auto *dst = (ICItemVar *) op1, *r1 = (ICItemVar *) op2, *r2 = (ICItemVar *) op3;
                if (r1->isConst && r2->isConst) {
                    const int right = r1->value > r2->value;
                    li(Reg::$t0, right);
                    sw(Reg::$t0, dst);
                } else if (r1->isConst) {
                    li(Reg::$t0, r1->value);
                    lw(Reg::$t1, r2);
                    sgt(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                } else {
                    lw(Reg::$t0, r1);
                    lw(Reg::$t1, r2);
                    sgt(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                }
                break;
            }
            case ICEntryType::GreaterEqual: {
                auto *dst = (ICItemVar *) op1, *r1 = (ICItemVar *) op2, *r2 = (ICItemVar *) op3;
                if (r1->isConst && r2->isConst) {
                    const int right = r1->value >= r2->value;
                    li(Reg::$t0, right);
                    sw(Reg::$t0, dst);
                } else if (r1->isConst) {
                    li(Reg::$t0, r1->value);
                    lw(Reg::$t1, r2);
                    sge(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                } else {
                    lw(Reg::$t0, r1);
                    lw(Reg::$t1, r2);
                    sge(Reg::$t2, Reg::$t0, Reg::$t1);
                    sw(Reg::$t2, dst);
                }
                break;
            }
            case ICEntryType::Beqz: {
                auto *condition = (ICItemVar *) op1;
                auto *label = (ICItemLabel *) op2;
                if (condition->isConst) {
                    if (condition->value == 0) {
                        j(label);
                    }
                } else {
                    lw(Reg::$t0, condition);
                    beqz(Reg::$t0, label);
                }
                break;
            }
            case ICEntryType::JumpLabel: {
                auto *label = (ICItemLabel *) op1;
                j(label);
                break;
            }
            case ICEntryType::InsertLabel:
                insertLabel((ICItemLabel *) op1);
                break;
        }
        i++;
    }
    if (!findReturn) jr();
    funcFArrayParamId2offset.clear();
    funcFVarParamId2offset.clear();
    clearLocalAndTempInFunc();
}

// 返回params个数
void MipsTranslator::pushParams(const std::vector<ICItem *> *params) {
#ifdef MIPS_DEBUG
    mipsOutput << std::flush;
#endif
    mipsOutput << "\n\n# Pushing Function Real Params:\n";
//    const int num = params->size();
    addiu(Reg::$sp, Reg::$sp, -1000);
    sw(Reg::$ra, 0, Reg::$sp);
    int offset = 4;
    if (params == nullptr || params->empty()) return;
    for (const ICItem *param: *params) {
        if (param->type == ICItemType::Imm) {
            auto *imm = (ICItemImm *) param;
            li(Reg::$t0, imm->value);
        } else if (param->type == ICItemType::Var) {
            auto *var = (ICItemVar *) param;
            if (var->isConst) {
                li(Reg::$t0, var->value);
            } else if (var->isGlobal) {
                la(Reg::$t0, var->toString());
                lw(Reg::$t0, 0, Reg::$t0);
            } else {
                lw(Reg::$t0, var, inSelfDefinedFunc);
            }
        } else if (param->type == ICItemType::Array) {
            // 会在 lw 中找到 param的lValReference，传入数组首地址
            lw(Reg::$t0, (ICItemVar *) param, inSelfDefinedFunc);
        }
        sw(Reg::$t0, offset, Reg::$sp);
        offset += 4;
    }

    mipsOutput << "# Finished Pushing Params!\n";
}


void MipsTranslator::translate_GlobalVarOrArrayDef(ICEntry *defEntry) {
    if (defEntry->entryType == ICEntryType::ConstVarDefine) {
        // 全局常量定义
        auto *constVar = (ICItemVar *) (defEntry->operator1);
        const int initValue = constVar->value;
        mipsOutput << "\n# " << (*constVar->originalName) << "\n";
        mipsOutput << constVar->toString() << ":  .word  " << initValue << "\n";
    } else if (defEntry->entryType == ICEntryType::ConstArrayDefine) {
        // 全局常量数组定义
        auto *constArray = ((ICItemArray *) (defEntry->operator1));
        const int d = constArray->originType.d,
                length1 = constArray->originType.length1,
                length2 = constArray->originType.length2;
        const int *initValues = constArray->value;
        const int length = constArray->length;
        if (d == 1) {
            mipsOutput << "\n# " << (*constArray->originalName) << "[" << length1 << "]" << "\n";
        } else {
            mipsOutput << "\n# " << (*constArray->originalName) << "[" << length1 << "]"
                       << "[" << length2 << "]" << "\n";
        }
        mipsOutput << constArray->toString() << ":  .word  ";
        for (int i = 0; i < length - 1; ++i) {
            mipsOutput << initValues[i] << ", ";
        }
        mipsOutput << initValues[length - 1] << "\n";
    } else if (defEntry->entryType == ICEntryType::VarDefine) {
        // 全局变量定义
        auto *var = ((ICItemVar *) (defEntry->operator1));
        const int initValue = var->value;
        mipsOutput << "\n#" << (*var->originalName) << "\n";
        mipsOutput << var->toString() << ": .word  " << initValue << "\n";
    } else if (defEntry->entryType == ICEntryType::ArrayDefine) {
        // 全局变量数组定义
        auto *array = ((ICItemArray *) (defEntry->operator1));
        const int d = array->originType.d,
                length1 = array->originType.length1,
                length2 = array->originType.length2;
        const int *initValues = array->value;
        const int length = array->length;
        if (d == 1) {
            mipsOutput << "\n# " << (*array->originalName) << "[" << length1 << "]" << "\n";
        } else {
            mipsOutput << "\n# " << (*array->originalName) << "[" << length1 << "]"
                       << "[" << length2 << "]" << "\n";
        }
        mipsOutput << array->toString() << ":  .word  ";
        for (int i = 0; i < length - 1; ++i) {
            mipsOutput << initValues[i] << ", ";
        }
        mipsOutput << initValues[length - 1] << "\n";
#ifdef MIPS_DEBUG
        mipsOutput << std::flush;
#endif
    }
}

bool MipsTranslator::isFuncFParam(ICItemVar *var) {
    assert(var->type == ICItemType::Var);
    return funcFVarParamId2offset.find(var->varId) != funcFVarParamId2offset.end();
}

bool MipsTranslator::isFuncFParam(ICItemArray *array) {
    array = (ICItemArray *) array;
    return funcFArrayParamId2offset.find(array->arrayId) != funcFArrayParamId2offset.end();
}

void MipsTranslator::lw(Reg reg, ICItemVar *var, bool whenPushingParamsRecursively) {
    int addr;
    ICItem *referenceItem = var;
#ifdef MIPS_DEBUG
    mipsOutput << std::flush;
#endif
    assert(var != nullptr);
    if (referenceItem->lValReference != nullptr) {  // var 为 LVal
        ReferenceType referenceType = referenceItem->referenceType;
        referenceItem = referenceItem->lValReference;
        if (referenceItem->type == ICItemType::Imm) {
            li(reg, ((ICItemImm *) referenceItem)->value);
            return;
        } else {
            switch (referenceType) {
                case ReferenceType::Var: {
                    var = (ICItemVar *) referenceItem;
                    if (var->isGlobal) {
                        la(reg, var->toString());
                        lw(reg, 0, reg);
                    } else if (isFuncFParam(var)) {
                        addr = funcFVarParamId2offset.find(var->varId)->second;
                        if (whenPushingParamsRecursively) {
                            addr += 1000;
                        }
                        lw(reg, addr, Reg::$sp);
                    } else if (var->isConst) {
                        li(reg, var->value);
                    } else {
                        if (inSelfDefinedFunc) {
                            if (var->isTemp) {
                                addr = tempVarId2offset.find(var->tempVarId)->second;
                            } else {
                                addr = localVarId2offset.find(var->varId)->second;
                            }
                            if (whenPushingParamsRecursively) {
                                addr += 1000;
                            }
                            lw(reg, addr, Reg::$sp);
                        } else {
                            if (var->isTemp) {
                                addr = tempVarId2mem.find(var->tempVarId)->second;
                            } else {
                                addr = localVarId2mem.find(var->varId)->second;
                            }
                            lw(reg, addr, Reg::$zero);
                        }
                    }
                    return;
                }
                case ReferenceType::Array1_Var: {
                    ICItem *offsetItem = var->array1_var_index;
                    lw(Reg::$t9, (ICItemVar *) offsetItem, whenPushingParamsRecursively);  // $t9 = 数组下标
                    sll(Reg::$t9, Reg::$t9, 2);  // t9 = t9 * 4
                    auto array1Item = (ICItemArray *) referenceItem;
                    if (array1Item->isGlobal) {
                        la(reg, array1Item->toString());
                        addu(reg, reg, Reg::$t9);  // reg = reg + t9 = 基地址 + 偏移
                        lw(reg, 0, reg);
                    } else if (isFuncFParam(array1Item)) {
                        addr = funcFArrayParamId2offset.find(array1Item->arrayId)->second;
                        if (whenPushingParamsRecursively) {
                            addr += 1000;
                        }
                        lw(reg, addr, Reg::$sp);
                        addu(reg, reg, Reg::$t9);
                        lw(reg, 0, reg);
                    } else {
                        if (inSelfDefinedFunc) {
                            if (array1Item->isTemp) {
                                addr = tempVarId2offset.find(array1Item->tempArrayId)->second;
                            } else {
                                addr = localVarId2offset.find(array1Item->arrayId)->second;
                            }
                            if (whenPushingParamsRecursively) {
                                addr += 1000;
                            }
                            lw(reg, addr, Reg::$sp);
                            addu(reg, reg, Reg::$t9);
                            lw(reg, 0, reg);
                        } else {
                            if (array1Item->isTemp) {
                                addr = tempArrayId2mem.find(array1Item->tempArrayId)->second;
                            } else {
                                addr = localArrayId2mem.find(array1Item->arrayId)->second;
                            }
                            addiu(reg, Reg::$t9, addr);
                            lw(reg, 0, reg);
                        }
                    }
                    return;
                }
                case ReferenceType::Array2_Var: {
                    ICItem *offsetItem1 = var->array2_var_index1;
                    ICItem *offsetItem2 = var->array2_var_index2;
                    // $t8 = 数组下标 1
                    lw(Reg::$t8, (ICItemVar *) offsetItem1, whenPushingParamsRecursively);
                    // $t9 = 数组下标 2
                    lw(Reg::$t9, (ICItemVar *) offsetItem2, whenPushingParamsRecursively);
                    mul(Reg::$t9, Reg::$t9, Reg::$t8);
                    sll(Reg::$t9, Reg::$t9, 2);  // t9 = t9 * 4
                    auto array2Item = (ICItemArray *) referenceItem;
                    if (array2Item->isGlobal) {
                        la(reg, array2Item->toString());
                        addu(reg, reg, Reg::$t9);  // reg = reg + t9 = 基地址 + 偏移
                        lw(reg, 0, reg);
                    } else if (isFuncFParam(array2Item)) {
                        addr = funcFArrayParamId2offset.find(array2Item->arrayId)->second;
                        if (whenPushingParamsRecursively) {
                            addr += 1000;
                        }
                        lw(reg, addr, Reg::$sp);
                        addu(reg, reg, Reg::$t9);
                        lw(reg, 0, reg);
                    } else {
                        if (inSelfDefinedFunc) {
                            if (array2Item->isTemp) {
                                addr = tempVarId2offset.find(array2Item->tempArrayId)->second;
                            } else {
                                addr = localVarId2offset.find(array2Item->arrayId)->second;
                            }
                            if (whenPushingParamsRecursively) {
                                addr += 1000;
                            }
                            lw(reg, addr, Reg::$sp);
                            addu(reg, reg, Reg::$t9);
                            lw(reg, 0, reg);
                        } else {
                            if (array2Item->isTemp) {
                                addr = tempArrayId2mem.find(array2Item->tempArrayId)->second;
                            } else {
                                addr = localArrayId2mem.find(array2Item->arrayId)->second;
                            }
                            addiu(reg, Reg::$t9, addr);
                            lw(reg, 0, reg);
                        }
                    }
                    return;
                }
                case ReferenceType::Array1:
                case ReferenceType::Array2: {
                    auto arrayItem = (ICItemArray *) referenceItem;
//                    mipsOutput << "# 实参类型 Array2\n";
                    if (arrayItem->isGlobal) {
                        la(reg, arrayItem->toString());
                    } else if (isFuncFParam(arrayItem)) {
                        addr = funcFArrayParamId2offset.find(arrayItem->arrayId)->second;
                        if (whenPushingParamsRecursively) {
                            addr += 1000;
                        }
                        lw(reg, addr, Reg::$sp);
                    } else {
                        if (inSelfDefinedFunc) {
                            if (arrayItem->isTemp) {
                                addr = tempVarId2offset.find(arrayItem->tempArrayId)->second;
                            } else {
                                addr = localVarId2offset.find(arrayItem->arrayId)->second;
                            }
                            if (whenPushingParamsRecursively) {
                                addr += 1000;
                            }
                            lw(reg, addr, Reg::$sp);
                        } else {
                            if (arrayItem->isTemp) {
                                addr = tempArrayId2mem.find(arrayItem->tempArrayId)->second;
                            } else {
                                addr = localArrayId2mem.find(arrayItem->arrayId)->second;
                            }
                            li(reg, addr);
                        }
                    }
                    return;
                }
                case ReferenceType::Array2_Array1: {  // 传新数组的首地址
                    ICItem *offsetItem = var->array2_array1_index;
                    auto *array2Item = (ICItemArray *) referenceItem;
                    const int refArrayD2 = array2Item->originType.length2;  // 所引用数组的第二维长度
//                    mipsOutput << "# 实参类型：Array2_Array1，传入开始\n";
                    // $t9 = 数组下标
                    lw(Reg::$t9, (ICItemVar *) offsetItem, whenPushingParamsRecursively);
                    mul(Reg::$t9, Reg::$t9, refArrayD2);  // 实际上传入的数组参数的首地址相对于原数组首地址的偏移
                    sll(Reg::$t9, Reg::$t9, 2);  // t9 = t9 * 4
                    if (array2Item->isGlobal) {
                        la(reg, array2Item->toString());
                        addu(reg, reg, Reg::$t9);
                    } else if (isFuncFParam(array2Item)) {
                        addr = funcFArrayParamId2offset.find(array2Item->arrayId)->second;
                        if (whenPushingParamsRecursively) {
                            addr += 1000;
                        }
                        lw(reg, addr, Reg::$sp);  // reg 为原数组首地址
                        addu(reg, Reg::$t9, reg);
                    } else {
                        if (inSelfDefinedFunc) {
                            if (array2Item->isTemp) {
                                addr = tempVarId2offset.find(array2Item->tempArrayId)->second;
                            } else {
                                addr = localVarId2offset.find(array2Item->arrayId)->second;
                            }
                            if (whenPushingParamsRecursively) {
                                addr += 1000;
                            }
                            lw(reg, addr, Reg::$sp);
                            addu(reg, reg, Reg::$t9);
                        } else {
                            if (array2Item->isTemp) {
                                addr = tempArrayId2mem.find(array2Item->tempArrayId)->second;
                            } else {
                                addr = localArrayId2mem.find(array2Item->arrayId)->second;
                            }
                            addiu(reg, Reg::$t9, addr);
                        }
                    }
//                    mipsOutput << "# 实参类型：Array2_Array1，传入结束\n";
                    return;
                }
                case ReferenceType::Unset:
                    throw std::runtime_error("\nError in MipsTranslator.cpp, line " +
                                             std::to_string(__LINE__) +
                                             ": reaching illegal cases\n");
            }
        }
        return;  // 处理完所有 LVal的情况
    }

    // 非 LVal
    if (var->isGlobal) {
        la(reg, var->toString());
        lw(reg, 0, reg);
        return;
    }
    if (var->isConst) {
        li(reg, var->value);
        return;
    }
    if (isFuncFParam(var)) {
        addr = funcFVarParamId2offset.find(var->varId)->second;
        if (whenPushingParamsRecursively) {
            addr += 1000;
        }
        lw(reg, addr, Reg::$sp);
        return;
    }
    if (inSelfDefinedFunc) {
        if (var->isTemp) {
            addr = tempVarId2offset.find(var->tempVarId)->second;
        } else {
            addr = localVarId2offset.find(var->varId)->second;
        }
        if (whenPushingParamsRecursively) {
            addr += 1000;
        }
        lw(reg, addr, Reg::$sp);
    } else {
        if (var->isTemp) {
            addr = tempVarId2mem.find(var->tempVarId)->second;
        } else {
            addr = localVarId2mem.find(var->varId)->second;
        }
        lw(reg, addr, Reg::$zero);
    }
}

void MipsTranslator::lw(Reg dst, int offset, Reg base) {
    mipsOutput << "lw " << reg2s.find(dst)->second << ", " <<
               offset << "(" << reg2s.find(base)->second << ")\n";
}

void MipsTranslator::sw(Reg reg, ICItemVar *dst) {
    int addr;
    ICItem *dstLVal = dst;
    // [Exp];
    // 例： 1-1;
    if (dst == nullptr) return;
    if (dstLVal->lValReference != nullptr) {
        // dst 是 LVal
        ReferenceType referenceType = dstLVal->referenceType;
        dstLVal = dstLVal->lValReference;

        switch (referenceType) {
            case ReferenceType::Var: {
                dst = (ICItemVar *) dstLVal;
                if (dst->isGlobal) {
                    la(Reg::$t9, dst->toString());
                    sw(reg, 0, Reg::$t9);
                    return;
                }
                if (isFuncFParam(dst)) {
                    addr = funcFVarParamId2offset.find(dst->varId)->second;
                    sw(reg, addr, Reg::$sp);
                    return;
                }
                if (dst->isTemp) {
                    if (inSelfDefinedFunc) {
                        if (tempVarId2offset.find(dst->tempVarId) == tempVarId2offset.end()) {
                            tempVarId2offset.insert({dst->tempVarId, tempFuncStackOffsetTop});
                            addr = tempFuncStackOffsetTop;
                            tempFuncStackOffsetTop += 4;
                        } else {
                            addr = tempVarId2offset.find(dst->tempVarId)->second;
                        }
                        sw(reg, addr, Reg::$sp);
                    } else {
                        if (tempVarId2mem.find(dst->tempVarId) == tempVarId2mem.end()) {
                            tempVarId2mem.insert({dst->tempVarId, tempStackAddressTop});
                            addr = tempStackAddressTop;
                            tempStackAddressTop += 4;
                        } else {
                            addr = tempVarId2mem.find(dst->tempVarId)->second;
                        }
                        sw(reg, addr, Reg::$zero);
                    }
                } else {
                    if (inSelfDefinedFunc) {
                        if (localVarId2offset.find(dst->varId) == localVarId2offset.end()) {
                            localVarId2offset.insert({dst->varId, tempFuncStackOffsetTop});
                            addr = tempFuncStackOffsetTop;
                            tempFuncStackOffsetTop += 4;
                        } else {
                            addr = localVarId2offset.find(dst->varId)->second;
                        }
                        sw(reg, addr, Reg::$sp);
                    } else {
                        if (localVarId2mem.find(dst->varId) == localVarId2mem.end()) {
                            localVarId2mem.insert({dst->varId, tempStackAddressTop});
                            addr = tempStackAddressTop;
                            tempStackAddressTop += 4;
                        } else {
                            addr = localVarId2mem.find(dst->varId)->second;
                        }
                        sw(reg, addr, Reg::$zero);
                    }
                }
                return;
            }
            case ReferenceType::Array1_Var: {  // 存到一维数组的某个元素里
                ICItem *offsetItem = dst->array1_var_index;
                lw(Reg::$t9, (ICItemVar *) offsetItem);  // $t9 = 数组下标
                sll(Reg::$t9, Reg::$t9, 2);  // t9 = t9 * 4 偏移量
                auto array1Item = (ICItemArray *) dstLVal;
                if (array1Item->isGlobal) {
                    la(Reg::$t8, array1Item->toString());  // t8 = 基地址
                    addu(Reg::$t9, Reg::$t8, Reg::$t9);  // t9 = t8 + t9 = 基地址 + 偏移
                    sw(reg, 0, Reg::$t9);
                } else if (isFuncFParam(array1Item)) {
                    addr = funcFArrayParamId2offset.find(array1Item->arrayId)->second;
                    addiu(Reg::$t9, Reg::$t9, addr);  // t9:偏移, addr:基地址
                    sw(reg, addr, Reg::$sp);
                } else {
                    if (inSelfDefinedFunc) {
                        if (array1Item->isTemp) {
                            addr = tempArrayId2offset.find(array1Item->tempArrayId)->second;
                        } else {
                            addr = localArrayId2offset.find(array1Item->arrayId)->second;
                        }
                        lw(Reg::$t8, addr, Reg::$sp);  // t8 = 数组首地址
                        addu(Reg::$t9, Reg::$t9, Reg::$t8);
                        sw(reg, 0, Reg::$t9);
                    } else {
                        if (array1Item->isTemp) {
                            addr = tempArrayId2mem.find(array1Item->tempArrayId)->second;
                        } else {
                            addr = localArrayId2mem.find(array1Item->arrayId)->second;
                        }
                        addiu(Reg::$t9, Reg::$t9, addr);  // t9:偏移, addr:基地址
                        sw(reg, addr, Reg::$zero);
                    }
                }
                return;
            }
            case ReferenceType::Array2_Var: {
                ICItem *offsetItem1 = dst->array2_var_index1;
                ICItem *offsetItem2 = dst->array2_var_index2;
                lw(Reg::$t8, (ICItemVar *) offsetItem1);  // $t8 = 数组下标 1
                lw(Reg::$t9, (ICItemVar *) offsetItem2);  // $t9 = 数组下标 2
                mul(Reg::$t9, Reg::$t9, Reg::$t8);
                sll(Reg::$t9, Reg::$t9, 2);  // t9 = t9 * 4  总的偏移量
                auto array2Item = (ICItemArray *) dstLVal;
                if (array2Item->isGlobal) {
                    la(Reg::$t8, array2Item->toString());  // t8 基地址
                    addu(Reg::$t9, Reg::$t8, Reg::$t9);  // t9 = t8 + t9 = 基地址 + 偏移
                    sw(reg, 0, Reg::$t9);
                } else if (isFuncFParam(array2Item)) {
                    addr = funcFArrayParamId2offset.find(array2Item->arrayId)->second;
                    addiu(Reg::$t9, Reg::$t9, addr);  // t9:偏移, addr:基地址
                    sw(reg, addr, Reg::$sp);
                } else {
                    if (inSelfDefinedFunc) {
                        if (array2Item->isTemp) {
                            addr = tempArrayId2offset.find(array2Item->tempArrayId)->second;
                        } else {
                            addr = localArrayId2offset.find(array2Item->arrayId)->second;
                        }
                        lw(Reg::$t8, addr, Reg::$sp);  // t8 = 数组首地址
                        addu(Reg::$t9, Reg::$t9, Reg::$t8);
                        sw(reg, 0, Reg::$t9);
                    } else {
                        if (array2Item->isTemp) {
                            addr = tempArrayId2mem.find(array2Item->tempArrayId)->second;
                        } else {
                            addr = localArrayId2mem.find(array2Item->arrayId)->second;
                        }
                        addiu(Reg::$t9, Reg::$t9, addr);  // t9:偏移, addr:基地址
                        sw(reg, addr, Reg::$zero);
                    }
                }
                return;
            }
            case ReferenceType::Array1:
            case ReferenceType::Array2:
            case ReferenceType::Array2_Array1:
            case ReferenceType::Unset:
                throw std::runtime_error("\nError in MipsTranslator.cpp, line " +
                                         std::to_string(__LINE__) +
                                         ": reaching illegal cases\n");

        }
        return;  // 处理完所有 LVal的情况
    }
    if (dst->isGlobal) {
        la(Reg::$t9, dst->toString());
        sw(reg, 0, Reg::$t9);
        return;
    }
    if (isFuncFParam(dst)) {  // TODO 形参为数组
        addr = funcFVarParamId2offset.find(dst->varId)->second;
        sw(reg, addr, Reg::$sp);
        return;
    }
    if (dst->isTemp) {
        if (inSelfDefinedFunc) {
            if (tempVarId2offset.find(dst->tempVarId) == tempVarId2offset.end()) {
                tempVarId2offset.insert({dst->tempVarId, tempFuncStackOffsetTop});
                addr = tempFuncStackOffsetTop;
                tempFuncStackOffsetTop += 4;
            } else {
                addr = tempVarId2offset.find(dst->tempVarId)->second;
            }
            sw(reg, addr, Reg::$sp);
        } else {
            if (tempVarId2mem.find(dst->tempVarId) == tempVarId2mem.end()) {
                tempVarId2mem.insert({dst->tempVarId, tempStackAddressTop});
                addr = tempStackAddressTop;
                tempStackAddressTop += 4;
            } else {
                addr = tempVarId2mem.find(dst->tempVarId)->second;
            }
            sw(reg, addr, Reg::$zero);
        }
    } else {
        if (inSelfDefinedFunc) {
            if (localVarId2offset.find(dst->varId) == localVarId2offset.end()) {
                localVarId2offset.insert({dst->varId, tempFuncStackOffsetTop});
                addr = tempFuncStackOffsetTop;
                tempFuncStackOffsetTop += 4;
            } else {
                addr = localVarId2offset.find(dst->varId)->second;
            }
            sw(reg, addr, Reg::$sp);
        } else {
            if (localVarId2mem.find(dst->varId) == localVarId2mem.end()) {
                localVarId2mem.insert({dst->varId, tempStackAddressTop});
                addr = tempStackAddressTop;
                tempStackAddressTop += 4;
            } else {
                addr = localVarId2mem.find(dst->varId)->second;
            }
            sw(reg, addr, Reg::$zero);
        }
    }
}

void MipsTranslator::sw(Reg src, int offset, Reg base) {
    mipsOutput << "sw " << reg2s.find(src)->second << ", " <<
               std::to_string(offset) << "(" << reg2s.find(base)->second << ")\n";
}

void MipsTranslator::li(Reg reg, int imm) {
    mipsOutput << "li " << reg2s.find(reg)->second << ", " << std::to_string(imm) << "\n";
}

void MipsTranslator::move(Reg dst, Reg src) {
    mipsOutput << "move " << reg2s.find(dst)->second << ", " << reg2s.find(src)->second << "\n";
}

void MipsTranslator::pushTempReg() {  // t0 - t9, ra
    return;
//    mipsOutput << "\n\n# store temp regs\n";
//    addiu(Reg::$sp, Reg::$sp, -44);
//    int offset = 0;
//    for (const Reg reg: tempRegs) {
//        sw(reg, offset, Reg::$sp);
//        offset += 4;
//    }
//    sw(Reg::$ra, offset, Reg::$sp);
}

void MipsTranslator::popTempReg() {
    return;
//    mipsOutput << "\n\n# recover temp regs\n";
//    int offset = 0;
//    for (const Reg reg: tempRegs) {
//        lw(reg, offset, Reg::$sp);
//        offset += 4;
//    }
//    lw(Reg::$ra, offset, Reg::$sp);
//    addiu(Reg::$sp, Reg::$sp, 44);
//    mipsOutput << "\n\n";
}

void MipsTranslator::addiu(Reg dst, Reg src, int i) {
    mipsOutput << "addiu " << reg2s.find(dst)->second << ", "
               << reg2s.find(src)->second << ", " << std::to_string(i) << "\n";
}

void MipsTranslator::jal(const ICItemFunc *calledFunc) {
    mipsOutput << "jal " << calledFunc->funcLabel->toString() << "\n";
}

void MipsTranslator::j(ICItemLabel *label) {
    mipsOutput << "j " << label->toString() << "\n";
}

void MipsTranslator::beqz(Reg cond, ICItemLabel *label) {
    mipsOutput << "beqz " << reg2s.find(cond)->second << ", " << label->toString() << "\n";
}

void MipsTranslator::exit() {
    mipsOutput << "\nli $v0, 10\n";
    syscall();
}

void MipsTranslator::getint(ICItemVar *dst) {
    mipsOutput << "\nli $v0, 5\n";
    syscall();
    sw(Reg::$v0, dst);
}

void MipsTranslator::printStr(int strId) {
    la(Reg::$a0, strId2label(strId));
    li(Reg::$v0, 4);
    syscall();
}

void MipsTranslator::la(Reg reg, const std::string &label) {
    mipsOutput << "la " << reg2s.find(reg)->second << ", " + label << "\n";
}

std::string MipsTranslator::strId2label(int strId) {
    return "str_" + std::to_string(strId);
}

void MipsTranslator::syscall() {
    mipsOutput << "syscall\n";
}

void MipsTranslator::printInt(ICItemVar *var) {
    lw(Reg::$a0, var);
    li(Reg::$v0, 1);
    syscall();
}

void MipsTranslator::addu(Reg rd, Reg rs, Reg rt) {
    mipsOutput << "addu " + reg2s.find(rd)->second <<
               ", " << reg2s.find(rs)->second <<
               ", " << reg2s.find(rt)->second << "\n";
}

void MipsTranslator::subu(Reg rd, Reg rs, Reg rt) {
    mipsOutput << "subu " + reg2s.find(rd)->second <<
               ", " << reg2s.find(rs)->second <<
               ", " << reg2s.find(rt)->second << "\n";
}

void MipsTranslator::mul(Reg rd, Reg rs, Reg rt) {
    mipsOutput << "mul " + reg2s.find(rd)->second <<
               ", " << reg2s.find(rs)->second <<
               ", " << reg2s.find(rt)->second << "\n";
}

void MipsTranslator::mul(Reg dst, Reg srcReg, int srcNum) {
    mipsOutput << "mul " + reg2s.find(dst)->second <<
               ", " << reg2s.find(srcReg)->second <<
               ", " << srcNum << "\n";
}

void MipsTranslator::div(Reg rs, Reg rt) {
    mipsOutput << "div " + reg2s.find(rs)->second <<
               ", " << reg2s.find(rt)->second << "\n";
}

void MipsTranslator::mfhi(Reg reg) {
    mipsOutput << "mfhi " + reg2s.find(reg)->second << "\n";
}

void MipsTranslator::mflo(Reg reg) {
    mipsOutput << "mflo " + reg2s.find(reg)->second << "\n";
}

void MipsTranslator::jr() {
    mipsOutput << "jr $ra\n";
}

void MipsTranslator::sll(Reg dst, Reg src, int bits) {
    mipsOutput << "sll " << reg2s.find(dst)->second << ", "
               << reg2s.find(src)->second << ", " << bits << '\n';
}

void MipsTranslator::sne(Reg rd, Reg rs, Reg rt) {
    mipsOutput << "sne " + reg2s.find(rd)->second <<
               ", " << reg2s.find(rs)->second <<
               ", " << reg2s.find(rt)->second << "\n";
}

void MipsTranslator::seq(Reg rd, Reg rs, Reg rt) {
    mipsOutput << "seq " + reg2s.find(rd)->second <<
               ", " << reg2s.find(rs)->second <<
               ", " << reg2s.find(rt)->second << "\n";
}

void MipsTranslator::sle(Reg rd, Reg rs, Reg rt) {
    mipsOutput << "sle " + reg2s.find(rd)->second <<
               ", " << reg2s.find(rs)->second <<
               ", " << reg2s.find(rt)->second << "\n";
}

void MipsTranslator::slt(Reg rd, Reg rs, Reg rt) {
    mipsOutput << "slt " + reg2s.find(rd)->second <<
               ", " << reg2s.find(rs)->second <<
               ", " << reg2s.find(rt)->second << "\n";
}

void MipsTranslator::sge(Reg rd, Reg rs, Reg rt) {
    mipsOutput << "sge " + reg2s.find(rd)->second <<
               ", " << reg2s.find(rs)->second <<
               ", " << reg2s.find(rt)->second << "\n";
}

void MipsTranslator::sgt(Reg rd, Reg rs, Reg rt) {
    mipsOutput << "sgt " + reg2s.find(rd)->second <<
               ", " << reg2s.find(rs)->second <<
               ", " << reg2s.find(rt)->second << "\n";
}

void MipsTranslator::_or(Reg rd, Reg rs, Reg rt) {
    mipsOutput << "or " + reg2s.find(rd)->second <<
               ", " << reg2s.find(rs)->second <<
               ", " << reg2s.find(rt)->second << "\n";
}

void MipsTranslator::_and(Reg rd, Reg rs, Reg rt) {
    mipsOutput << "and " + reg2s.find(rd)->second <<
               ", " << reg2s.find(rs)->second <<
               ", " << reg2s.find(rt)->second << "\n";
}

void MipsTranslator::insertLabel(ICItemLabel *label) {
    mipsOutput << label->toString() << ":\n";
}

void MipsTranslator::clearLocalAndTempInFunc() {
    localVarId2offset.clear();
    localArrayId2offset.clear();
    tempVarId2offset.clear();
    tempArrayId2offset.clear();
}
