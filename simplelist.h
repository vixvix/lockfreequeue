#ifndef __SIMPLELIST_H__
#define __SIMPLELIST_H__

#include "enforce.h"

template<typename T>
struct SimpleList
{
    struct Node
    {
        Node(const T& t) : pNext(0), obj(t)
        {
        }
        Node* pNext;
        T obj;
    };
    struct iterator
    {
        Node* pNode;
        iterator& operator ++()
        {
            pNode = (*ENFORCE(pNode)).pNext;
            return *this;
        }
        T& operator *()
        {
            return (*ENFORCE(pNode)).obj;
        }
        bool operator !=(iterator i)
        {
            return pNode != i.pNode;
        }
    };
    SimpleList()
    {
        front.pNode = 0;
        back.pNode = 0;
    }
    ~SimpleList()
    {        
        erase_begin(end());
    }
    iterator begin()
    {
        return front;
    }

    iterator end()
    {
        iterator i;
        i.pNode = 0;
        return i;
    }
    void push_back(const T& t)
    {
      Node* pNode = new Node(t);
      if (back.pNode)
      {
          ENFORCE(!back.pNode->pNext);
          back.pNode->pNext = pNode;
          back.pNode = pNode;
      }
      else
      {
          ENFORCE(!front.pNode);
          front.pNode = pNode;
          back.pNode = pNode;
      }
    }
    std::size_t erase_begin(iterator last)
    {
        std::size_t i = 0;
        for (; front != last; ++i)
        {
            ENFORCE(front.pNode);
            Node* pSecond = front.pNode->pNext;
            delete front.pNode;
            front.pNode = pSecond;
        }
        if (!front.pNode)
        {
            back.pNode = 0;
        }
        return i;
    }
private:
    iterator front;
    iterator back;
};

#endif
