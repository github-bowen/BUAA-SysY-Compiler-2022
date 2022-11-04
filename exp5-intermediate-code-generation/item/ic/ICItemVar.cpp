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
          tempVarId(-1),
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
          varId(-1),
          tempVarId(generateTempVarId()),
          isTemp(true) {}

int ICItemVar::generateVarId() {
    static int i = 0;
    return ++i;
}

int ICItemVar::generateTempVarId() {
    static int i = 0;
    return ++i;
}
