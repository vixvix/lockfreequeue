#ifndef __LOCKFREEQUEUE_H__
#define __LOCKFREEQUEUE_H__

#include "fenced.h"
#include "simplelist.h"

template <typename T>
struct LockFreeQueue
{
    LockFreeQueue()
    {
        list.push_back(T());
        iHead.Set(list.begin());
        iTail.Set(list.end());
    }

    void Produce(const T& t)
    {
        list.push_back(t);
        iTail.Set(list.end());
        list.erase_begin(iHead.Get());
    }

    bool Consume(T& t)
    {
        typename TList::iterator iNext = iHead.Get();
        ++iNext;
        if (iNext != iTail.Get())
        {
            iHead.Set(iNext);
            t = *iHead.Get();
            return true;
        }
        return false;
    }

    T Consume()
    {
        T tmp;
        while (!Consume(tmp))
        {
            ;
        }
        return tmp;
    }

private:
    typedef SimpleList<T> TList;
    TList list;
    Fenced<typename TList::iterator> iHead, iTail;
};

#endif
