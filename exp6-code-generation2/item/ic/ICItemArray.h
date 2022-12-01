#ifndef IC_ITEM_ARRAY_H
#define IC_ITEM_ARRAY_H

#include <vector>
#include "ICItem.h"
#include "ICItemLabel.h"
#include "ICItemVar.h"
#include "item/symbolTable/SymbolTableEntry.h"

struct OriginType {
    int d;
    int length1;
    int length2;

    OriginType() = default;

    OriginType(int d, int length1, int length2)
            : d(d), length1(length1), length2(length2) {}
};

class ICItemArray : public ICItem {
private:
    static int generateArrayId();

    static int generateTempArrayId();

public:
    /* 用于存储临时变量 */
    const bool isTemp;
    const int tempArrayId;
    std::vector<ICItemVar *> *itemsToInitArray;  // 只有给数组赋值时会用到
    /* 用于非临时变量 */
    const std::string *originalName;
    const SymbolTableEntry *symbolTableEntry;
    const bool isGlobal;
    const bool isConst;
    const int arrayId;
    OriginType originType;
    int *value;  // const 特有
    int length;  // 一维数组的长度  TODO: 注意，二维数组全部转成一维！

    ICItemArray(const std::string *originalName, SymbolTableEntry *symbolTableEntry,
                bool isGlobal, bool isConst, int length = 0, int *value = nullptr);

    explicit ICItemArray(bool isGlobal, int *value = nullptr);

    ~ICItemArray() override;

    std::string toString() const;

    void setOriginType(int d, int length1, int length2);
};

#endif //IC_ITEM_ARRAY_H
