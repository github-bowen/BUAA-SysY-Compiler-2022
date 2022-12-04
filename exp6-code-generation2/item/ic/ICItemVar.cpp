#include "ICItemVar.h"

/**
 *
 * @param originalName
 * @param symbolTableEntry
 * @param isConst
 * @param isGlobal
 * @param value
 */
ICItemVar::ICItemVar(const std::string *originalName, SymbolTableEntry *symbolTableEntry,
                     bool isConst, bool isGlobal, int value)
        : ICItem(ICItemType::Var),
          isGlobal(isGlobal),
          isConst(isConst),
          varId(generateVarId()),
          tempVarId(0),
          value(value),
          symbolTableEntry(symbolTableEntry),
          originalName(originalName),
          isTemp(false) {}

/**
 *
 * @param isGlobal
 * @param value
 */
ICItemVar::ICItemVar(bool isGlobal, int value)
        : ICItem(ICItemType::Var),
          originalName(nullptr),
          symbolTableEntry(nullptr),
          isConst(false),
          isGlobal(isGlobal),
          value(value),
          varId(0),
          tempVarId(generateTempVarId()),
          isTemp(true) {}

int ICItemVar::generateVarId() {
    static int i = 0;
    return --i;
}

int ICItemVar::generateTempVarId() {
    static int i = 0;
    return ++i;
}

std::string ICItemVar::toString() const{
    if (isTemp) {
        return "temp_var" + std::to_string(abs(tempVarId));
    } else {
        if (isGlobal) {
            if (isConst) {
                return "global_const_var" + std::to_string(abs(varId));
            } else {
                return "global_var" + std::to_string(abs(varId));
            }
        } else {
            if (isConst) {
                return "local_const_var" + std::to_string(abs(varId));
            } else {
                return "local_var" + std::to_string(abs(varId));
            }
        }
    }
}
