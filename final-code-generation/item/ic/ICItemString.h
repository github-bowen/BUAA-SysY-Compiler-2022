#ifndef IC_ITEM_STRING_H
#define IC_ITEM_STRING_H

#include <map>
#include <vector>
#include "ICItem.h"
#include "ICItemVar.h"

class ICItemString : public ICItem {
public:
    class StrItem {
    public:
        const bool isString;
        const std::string *stringItem;
        const ICItem *intItem;
        const int pureStringId;

        static int generatePureStringId(bool isString) {
            static int pureStringId = 0;
            return ++pureStringId;
        }

        StrItem(const bool isString, const std::string *stringItem, const ICItem *intItem)
                : isString(isString), stringItem(stringItem),
                  intItem(intItem), pureStringId(generatePureStringId(isString)) {}

        ~StrItem() {
            delete stringItem;
            delete intItem;
        }
    };

    std::vector<StrItem *> *stringItems;
    std::map<int, std::string *> *id2pureString;

    ICItemString() : ICItem(ICItemType::String) {
        stringItems = new std::vector<StrItem *>;
        id2pureString = new std::map<int, std::string *>;
    }

    ~ICItemString() override {
        for (const auto *pItem: *stringItems) delete pItem;
        delete stringItems;
        for (const auto &pItem: *id2pureString) delete pItem.second;
        delete id2pureString;
    }

    void addIntItem(ICItem *intItem) const {
        auto *intType = new StrItem(false, nullptr, intItem);
        stringItems->push_back(intType);
    }

    /**
     * @param s
     * @return pureStringId
     */
    int addStringItem(std::string *s) const {
        auto *stringItem = new StrItem(true, s, nullptr);
        stringItems->push_back(stringItem);
        id2pureString->insert({stringItem->pureStringId, s});
        return stringItem->pureStringId;
    }

};

#endif //IC_ITEM_STRING_H
