#ifndef REFERENCED_ENTRY
#define REFERENCED_ENTRY

#include "SymbolTableEntryType.h"

class ReferencedEntry {
public:
    const SymbolTableEntryType actualType;
    const SymbolTableEntryType referencedType;

    int index1;  // ident[index1]
    int index2;  // ident[index1][index2]
    bool valueUncertained;

    ReferencedEntry(SymbolTableEntryType actualType,
                    SymbolTableEntryType referencedType,
                    int index1,
                    int index2 = -1) : actualType(actualType),
                                 referencedType(referencedType),
                                 index1(index1),
                                 index2(index2),
                                 valueUncertained(false) {}

    ReferencedEntry(SymbolTableEntryType actualType,
                    SymbolTableEntryType referencedType) : actualType(actualType),
                                                     referencedType(referencedType),
                                                     valueUncertained(true) {}
};

#endif //REFERENCED_ENTRY
