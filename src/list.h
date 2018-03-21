#pragma once

#include <cstdlib>
#include "marie.h"

struct SymbolListNode
{
    Symbol data;
    SymbolListNode *prev;
};

struct SymbolList
{
    SymbolListNode *head;
    u32 count;
};

inline void
Push(SymbolList *list, const Symbol &a)
{
    auto *newHead = static_cast<SymbolListNode*>(malloc(sizeof(SymbolListNode)));
    newHead->data = a;
    newHead->prev = list->head;

    list->head = newHead;
    list->count += 1;
}

inline Symbol
Pop(SymbolList *list)
{
    auto result = list->head->data;
    auto oldHead = list->head;

    list->head = oldHead->prev;
    list->count -= 1;

    free(oldHead);
    return result;
}

inline Symbol*
PeakAt(SymbolList *list, u32 index)
{
    auto current = list->head;
    for (u32 i = 0; i < index; ++i)
    {
        current = current->prev;
    }

    return &current->data;
}

inline void
Free(SymbolList *list)
{
    while (list->count)
    {
        Pop(list);
    }
}

