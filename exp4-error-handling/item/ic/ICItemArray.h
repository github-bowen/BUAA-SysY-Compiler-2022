#ifndef IC_ITEM_ARRAY_H
#define IC_ITEM_ARRAY_H

#include "ICItem.h"
#include "ICItemLabel.h"

class ICItemArray : public ICItem {
private:
    static int generateArrayId() {
        static int arrayId = 0;
        return ++arrayId;
    }

public:
    const bool isGlobal;
    const bool isConst;
    const int arrayId;
    int *value;  // const 特有
    int d;  // 维数  TODO: 注意，二维数组全部转成一维！

    /**
     *
     * @param isGlobal
     * @param isConst
     * @param d
     * @param value
     */
    ICItemArray(bool isGlobal, bool isConst, int d, int *value = nullptr) :
            ICItem(ICItemType::Array), isConst(isConst), value(value), d(d),
            isGlobal(isGlobal), arrayId(generateArrayId()) {}

    ~ICItemArray() override {
        delete[] value;
    }
};

#endif //IC_ITEM_ARRAY_H
