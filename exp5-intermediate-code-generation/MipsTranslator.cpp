#include "MipsTranslator.h"

#include <fstream>
#include "_debug.h"
#include <string>

extern std::ofstream mipsOutput;

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
            case ICEntryType::ArrayDefine:
                break;
            case ICEntryType::ConstArrayDefine:
                break;
                // case ICEntryType::FuncDefine:  // 非法，不应该出现这个
            case ICEntryType::FuncCall: {
                auto *calledFunc = (const ICItemFunc *) (entry->calledFunc);
                pushTempReg();
                int paramNum = 0;
                if (entry->params != nullptr && !entry->params->empty()) {
                    paramNum = pushParams(entry->params);
                }
                jal(calledFunc);
                if (paramNum > 0) {
                    addi(Reg::$sp, Reg::$sp, paramNum * 4);
                }
                popTempReg();
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
                    divu(Reg::$t0, Reg::$t1);
                    mflo(Reg::$t2);
                    sw(Reg::$t2, dst);
                } else {
                    lw(Reg::$t0, r1);
                    lw(Reg::$t1, r2);
                    divu(Reg::$t0, Reg::$t1);
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
                    divu(Reg::$t0, Reg::$t1);
                    mfhi(Reg::$t2);
                    sw(Reg::$t2, dst);
                } else {
                    lw(Reg::$t0, r1);
                    lw(Reg::$t1, r2);
                    divu(Reg::$t0, Reg::$t1);
                    mfhi(Reg::$t2);
                    sw(Reg::$t2, dst);
                }
                break;
            }
            case ICEntryType::Not:
                break;
            case ICEntryType::Neg: {
                auto *dst = (ICItemVar *) op1, *src = (ICItemVar *) op2;
                if (src->isConst) {
                    li(Reg::$t0, src->value);
                    sw(Reg::$t0, dst);
                } else {
                    lw(Reg::$t0, src);
                    subu(Reg::$t0, Reg::$zero, Reg::$t0);
                    sw(Reg::$t0, dst);
                }
                break;
            }
            case ICEntryType::ArrayGet:
                break;
        }
        i++;
    }
    mipsOutput << "\n\n\n\n" << "# self defined functions\n\n";
    for (const auto &item: *name2icItemFunc) {
        ICItemFunc *func = item.second;
        mipsOutput << func->funcLabel->toString() << ":\n\n";
        translate_FuncDef(func);
        mipsOutput << "\n\n\n";
    }
    mipsOutput << std::flush;
}

void MipsTranslator::translate_FuncDef(ICItemFunc *func) {
    funcFParamId2offset.clear();
    std::vector<ICItem *> *params = func->params;
    const int num = params->size();

    int offset = 0;
    for (const auto *param: *params) {
        auto *var = ((ICItemVar *) param);
        funcFParamId2offset.insert({var->varId, offset});
        offset += 4;
    }

    std::vector<ICEntry *> *funcEntries = func->entries;
    int i = 0;
    bool findReturn = false;
    while (i < funcEntries->size()) {
        ICEntry *entry = funcEntries->at(i);
        ICItem *op1 = entry->operator1, *op2 = entry->operator2, *op3 = entry->operator3;
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
            case ICEntryType::ArrayDefine:
                break;
            case ICEntryType::ConstArrayDefine:
                break;
                // case ICEntryType::FuncDefine:  // 非法，不应该出现这个
            case ICEntryType::FuncCall: {
                auto *calledFunc = (const ICItemFunc *) (entry->calledFunc);
                pushTempReg();
                int paramNum = 0;
                if (entry->params != nullptr && !entry->params->empty()) {
                    paramNum = pushParams(entry->params);
                }
                jal(calledFunc);
                if (paramNum > 0) {
                    addi(Reg::$sp, Reg::$sp, paramNum * 4);
                }
                popTempReg();
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
                    divu(Reg::$t0, Reg::$t1);
                    mflo(Reg::$t2);
                    sw(Reg::$t2, dst);
                } else {
                    lw(Reg::$t0, r1);
                    lw(Reg::$t1, r2);
                    divu(Reg::$t0, Reg::$t1);
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
                    divu(Reg::$t0, Reg::$t1);
                    mfhi(Reg::$t2);
                    sw(Reg::$t2, dst);
                } else {
                    lw(Reg::$t0, r1);
                    lw(Reg::$t1, r2);
                    divu(Reg::$t0, Reg::$t1);
                    mfhi(Reg::$t2);
                    sw(Reg::$t2, dst);
                }
                break;
            }
            case ICEntryType::Not:
                break;
            case ICEntryType::Neg: {
                auto *dst = (ICItemVar *) op1, *src = (ICItemVar *) op2;
                if (src->isConst) {
                    li(Reg::$t0, src->value);
                    sw(Reg::$t0, dst);
                } else {
                    lw(Reg::$t0, src);
                    subu(Reg::$t0, Reg::$zero, Reg::$t0);
                    sw(Reg::$t0, dst);
                }
                break;
            }
            case ICEntryType::ArrayGet:
                break;
        }
        i++;
    }
    if (!findReturn) jr();
    funcFParamId2offset.clear();
}

// 返回params个数
int MipsTranslator::pushParams(const std::vector<ICItem *> *params) {
    mipsOutput << "\n\n# store funcrtion params\n";
    const int num = params->size();
    addi(Reg::$sp, Reg::$sp, -num * 4);
    int offset = 0;
    for (const auto *param: *params) {
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
                lw(Reg::$t0, var);
            }
        }
        sw(Reg::$t0, offset, Reg::$sp);
        offset += 4;
    }

    mipsOutput << "\n\n";
    return num;
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
    }
}

bool MipsTranslator::isFuncFParam(ICItemVar *var) {
    return funcFParamId2offset.find(var->varId) != funcFParamId2offset.end();
}

void MipsTranslator::lw(Reg reg, ICItemVar *var) {
    int addr;
    ICItem *item = var;
    if (item->reference != nullptr) {
        while (item->reference != nullptr) {
            item = item->reference;
        }
        if (item->type == ICItemType::Imm) {
            li(reg, ((ICItemImm *) item)->value);
            return;
        } else {
            var = (ICItemVar *) item;
        }
    }
    if (var->isGlobal) {
        la(reg, var->toString());
        lw(reg, 0, reg);
        return;
    }
    if (isFuncFParam(var)) {
        addr = funcFParamId2offset.find(var->varId)->second;
        mipsOutput << "lw " << reg2s.find(reg)->second << ", " << addr << "($sp)\n";
        return;
    }
    if (var->isConst) {
        li(reg, var->value);
        return;
    }
    if (var->isTemp) {
        addr = tempVarId2mem.find(var->tempVarId)->second;
    } else {
        addr = localVarId2mem.find(var->varId)->second;
    }
    mipsOutput << "lw " << reg2s.find(reg)->second << ", " << addr << "($0)\n";
}

void MipsTranslator::lw(Reg dst, int offset, Reg base) {
    mipsOutput << "lw " << reg2s.find(dst)->second << ", " <<
               std::to_string(offset) << "(" << reg2s.find(base)->second << ")\n";
}

void MipsTranslator::sw(Reg reg, ICItemVar *var) {
    int addr;
    ICItem *item = var;
    if (item->reference != nullptr) {
        while (item->reference != nullptr) {
            item = item->reference;
        }
//        sw(reg, (ICItemVar *) item);
//        return;
        var = (ICItemVar *) item;
    }
    if (var->isGlobal) {
        la(Reg::$t9, var->toString());
        sw(reg, 0, Reg::$t9);
        return;
    }
    if (isFuncFParam(var)) {
        addr = funcFParamId2offset.find(var->varId)->second;
        mipsOutput << "sw " << reg2s.find(reg)->second << ", " << addr << "($sp)\n";
        return;
    }
    if (var->isTemp) {
        if (tempVarId2mem.find(var->tempVarId) == tempVarId2mem.end()) {
            tempVarId2mem.insert({var->tempVarId, tempStackAddressTop});
            addr = tempStackAddressTop;
            tempStackAddressTop += 4;
        } else {
            addr = tempVarId2mem.find(var->tempVarId)->second;
        }
    } else {
        if (localVarId2mem.find(var->varId) == localVarId2mem.end()) {
            localVarId2mem.insert({var->varId, tempStackAddressTop});
            addr = tempStackAddressTop;
            tempStackAddressTop += 4;
        } else {
            addr = localVarId2mem.find(var->varId)->second;
        }
    }
    mipsOutput << "sw " << reg2s.find(reg)->second << ", " << addr << "($0)\n";
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
    mipsOutput << "\n\n# store temp regs\n";
    addi(Reg::$sp, Reg::$sp, -44);
    int offset = 0;
    for (const Reg reg: tempRegs) {
        sw(reg, offset, Reg::$sp);
        offset += 4;
    }
    sw(Reg::$ra, offset, Reg::$sp);
}

void MipsTranslator::popTempReg() {
    mipsOutput << "\n\n# recover temp regs\n";
    int offset = 0;
    for (const Reg reg: tempRegs) {
        lw(reg, offset, Reg::$sp);
        offset += 4;
    }
    lw(Reg::$ra, offset, Reg::$sp);
    addi(Reg::$sp, Reg::$sp, 44);
    mipsOutput << "\n\n";
}

void MipsTranslator::addi(Reg dst, Reg src, int i) {
    mipsOutput << "addi " << reg2s.find(dst)->second << ", "
               << reg2s.find(src)->second << ", " << std::to_string(i) << "\n";
}

void MipsTranslator::jal(const ICItemFunc *calledFunc) {
    mipsOutput << "jal " << calledFunc->funcLabel->toString() << "\n";
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

void MipsTranslator::divu(Reg rs, Reg rt) {
    mipsOutput << "divu " + reg2s.find(rs)->second <<
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






