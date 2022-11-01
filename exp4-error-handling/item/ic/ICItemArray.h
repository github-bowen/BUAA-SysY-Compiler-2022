#ifndef IC_ITEM_ARRAY_H
#define IC_ITEM_ARRAY_H

#include "ICItem.h"
#include "ICItemLabel.h"

class ICItemArray : public ICItem {
private:
    static int generateArrayId() {
        static int i = 0;
        return ++i;
    }

    static int generateTempArrayId() {
        static int i = 0;
        return ++i;
    }

public:
    /* 用于存储临时变量 */
    const bool isTemp;
    const int tempArrayId;
    std::vector<ICItem *> *itemsToInitArray;  // 只有给数组赋值时会用到
    /* 用于非临时变量 */
    const std::string *originalName;
    const SymbolTableEntry *symbolTableEntry;
    const bool isGlobal;
    const bool isConst;
    const int arrayId;
    int *value;  // const 特有
    int length;  // 一维数组的长度  TODO: 注意，二维数组全部转成一维！

    /**
     *
     * @param originalName
     * @param symbolTableEntry
     * @param isGlobal
     * @param isConst
     * @param length
     * @param value
     */
    ICItemArray(const std::string *originalName, SymbolTableEntry *symbolTableEntry,
                bool isGlobal, bool isConst, int length = 0, int *value = nullptr) :
            ICItem(ICItemType::Array),
            isConst(isConst),
            value(value),
            length(length),
            isGlobal(isGlobal),
            arrayId(generateArrayId()),
            tempArrayId(-1),
            symbolTableEntry(symbolTableEntry),
            originalName(originalName),
            isTemp(false),
            itemsToInitArray(nullptr) {}

    /**
     *
     * @param isGlobal
     * @param value
     */
    explicit ICItemArray(bool isGlobal, int *value = nullptr) :
            ICItem(ICItemType::Array),
            isConst(false),
            value(value),
            length(-1),
            isGlobal(isGlobal),
            arrayId(-1),
            tempArrayId(generateTempArrayId()),
            symbolTableEntry(nullptr),
            originalName(nullptr),
            isTemp(true) {
        if (!isGlobal) {
            // 初始化非全局变量的数组才会用到，数组每一个元素都需要通过临时变量来赋值
            itemsToInitArray = new std::vector<ICItem *>;
        } else {
            itemsToInitArray = nullptr;
        }
    }

    ~ICItemArray() override {
        delete[] value;
        for (const auto * pItem : *itemsToInitArray) delete pItem;
        delete itemsToInitArray;
    }
};

#endif //IC_ITEM_ARRAY_H
