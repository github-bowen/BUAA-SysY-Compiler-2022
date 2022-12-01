#ifndef IC_ITEM_H
#define IC_ITEM_H

#include <string>
#include "ICItemType.h"

/**
 * Intermediate Code Item
 */
class ICItem {
public:
    ICItemType type;
    const std::string newName;
    ICItem *reference;
    bool typeChanged;
    ICItem *newICItem;

    explicit ICItem(ICItemType type = ICItemType::Reference)
            : type(type),
              reference(nullptr),
              typeChanged(false),
              newICItem(nullptr) {}

//    virtual std::string *toString() = 0;

    virtual ~ICItem() = default;
};

#endif //IC_ITEM_H
