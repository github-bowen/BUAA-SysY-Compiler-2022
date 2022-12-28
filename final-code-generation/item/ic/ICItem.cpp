#include "ICItem.h"

ICItem::ICItem(ICItemType type)
        : type(type),
          referenceType(ReferenceType::Unset),
          lValReference(nullptr),
          typeChanged(false),
          newICItem(nullptr) {}

ICItem::~ICItem() {
    delete newICItem;
    delete lValReference;
    delete array1_var_index;
    delete array2_array1_index;
    delete array2_var_index1;
    delete array2_var_index2;
}

