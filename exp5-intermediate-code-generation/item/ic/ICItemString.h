#ifndef IC_ITEM_STRING_H
#define IC_ITEM_STRING_H

#include <map>
#include <vector>
#include "ICItem.h"
#include "ICItemVar.h"

class ICItemString : public ICItem {
private:
    class InnerType {
    public:
        const bool isString;
        const std::string *stringItem;
        const ICItemVar *intItem;
        const int pureStringId;

        static int generatePureStringId(bool isString) {
            static int pureStringId = 0;
            return ++pureStringId;
        }

        InnerType(const bool isString, const std::string *stringItem, const ICItemVar *intItem)
                : isString(isString), stringItem(stringItem),
                  intItem(intItem), pureStringId(generatePureStringId(isString)) {}

        ~InnerType() {
            delete stringItem;
            delete intItem;
        }
    };


public:
    std::vector<InnerType *> *stringItems;
    std::map<int, std::string *> *id2pureString;

    ICItemString() : ICItem(ICItemType::String) {
        stringItems = new std::vector<InnerType *>;
        id2pureString = new std::map<int, std::string *>;
    }

    ~ICItemString() override {
        for (const auto *pItem: *stringItems) delete pItem;
        delete stringItems;
        for (const auto &pItem: *id2pureString) delete pItem.second;
        delete id2pureString;
    }

    void addIntItem(ICItemVar *intItem) {
        auto *intType = new InnerType(false, nullptr, intItem);
        stringItems->push_back(intType);
    }

    /**
     * @param s
     * @return pureStringId
     */
    int addStringItem(std::string *s) {
        auto *stringItem = new InnerType(true, s, nullptr);
        stringItems->push_back(stringItem);
        id2pureString->insert({stringItem->pureStringId, s});
        return stringItem->pureStringId;
    }

};

#endif //IC_ITEM_STRING_H
