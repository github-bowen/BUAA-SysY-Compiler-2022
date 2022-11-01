#ifndef IC_ITEM_H
#define IC_ITEM_H

#include <string>
#include "ICItemType.h"

/**
 * Intermediate Code Item
 */
class ICItem {
public:
    const ICItemType type;
    const std::string newName;

    explicit ICItem(ICItemType type) : type(type) {}

//    virtual std::string *toString() = 0;

    virtual ~ICItem() = 0;
};

#endif //IC_ITEM_H
