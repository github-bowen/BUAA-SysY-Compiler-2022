#ifndef MIPS_TRANSLATOR_H
#define MIPS_TRANSLATOR_H


#include "ICTranslator.h"
#include "item/reg.h"

class MipsTranslator {
public:
    const int tempStackAddressBase = 268500992;  // 0x10010000
    int tempStackAddressTop = 268500992;

    std::map<int, int> localVarId2mem;  // id 为负

    std::map<int, int> tempVarId2mem;  // id 为正

    std::map<Reg, bool> regUsage;

    std::map<Reg, int> reg2id;

    ICTranslator *icTranslator;

    explicit MipsTranslator(ICTranslator *icTranslator);

    void translate();

    void translate_GlobalVarOrArrayDef(ICEntry *);

    void pushParams(const std::vector<ICItem *> *params);

    void lw(Reg reg, ICItemVar *var);

    void sw(Reg reg, ICItemVar *var);

    void sw(Reg src, int offset, Reg base);

    void li(Reg reg, int imm);

    void move(Reg dst, Reg src);

    void pushTempReg();

    void addi(Reg dst, Reg src, int i);

    void jal(const ICItemFunc *calledFunc);

    void exit();

    void getint(ICItemVar *dst);

    void printStr(int strId);

    void la(Reg reg, const std::string &label);

    static std::string strId2label(int strId);

    void syscall();

    void printInt(ICItemVar *var);

    void addu(Reg rd, Reg rs, Reg rt);

    void subu(Reg rd, Reg rs, Reg rt);

    void mul(Reg rd, Reg rs, Reg rt);

    void divu(Reg rs, Reg rt);

    void mfhi(Reg reg);

    void mflo(Reg reg);

};


#endif //MIPS_TRANSLATOR_H
