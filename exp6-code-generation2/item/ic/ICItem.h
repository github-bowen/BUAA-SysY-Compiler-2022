#ifndef IC_ITEM_H
#define IC_ITEM_H

#include <string>
#include "ICItemType.h"

enum class ReferenceType {
    Unset,
    Var,
    Array1,
    Array2,
    Array1_Var,  // 对一维数组 arr arr[x]
    Array2_Array1,  // 对二维数组 arr arr[x]
    Array2_Var  // 对二维数组 arr arr[x][y]
};

/**
 * Intermediate Code Item
 */
class ICItem {
public:
    ICItemType type;
    const std::string newName;
    bool typeChanged;
    ICItem *newICItem;

    // ----------------------------------------------- LVal 引用 -----------------------------------------
    ICItem *lValReference;
    ReferenceType referenceType;
    // 该 LVal 是否是函数实参
    bool isFuncRParam;
    // for Array1_Var
    ICItem *array1_var_index{nullptr};
    // for Array2_Array1
    ICItem *array2_array1_index{nullptr};
    // for Array2_Var
    ICItem *array2_var_index1{nullptr};
    ICItem *array2_var_index2{nullptr};

    explicit ICItem(ICItemType type = ICItemType::Reference);

//    virtual std::string *toString() = 0;

    virtual ~ICItem();
};

#endif //IC_ITEM_H
