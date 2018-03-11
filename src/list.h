#pragma once

template <typename T>
struct TListNode
{
    T data;

    TListNode *prev;
};

template <typename T>
struct TList
{
    TListNode<T> *head;

    uint32 count;
};

template <typename T>
inline void
Push(TList<T> *list, const T &a)
{
    TListNode<T> *newHead = static_cast<TListNode<T>*>(malloc(sizeof(TListNode<T>)));
    newHead->data = a;
    newHead->prev = list->head;

    list->head = newHead;
    list->count += 1;
}

template <typename T>
inline T
Pop(TList<T> *list)
{
    T result = list->head->data;

    auto oldHead = list->head;

    list->head = oldHead->prev;
    list->count -= 1;

    free(oldHead);
    return result;
}

template <typename T>
inline T*
At(TList<T> *list, uint32 index)
{
    auto current = list->head;
    for (uint32 i = 0; i < index; ++i)
    {
        current = current->prev;
    }

    return &current->data;
}

