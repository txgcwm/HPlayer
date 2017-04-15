#ifndef __QUEUE__H__
#define __QUEUE__H__

#include <iostream>

#include "QueueNode.h"


template<class T>
class Queue
{
public:
    Queue();
    bool push(QueueNode<T> *item);
    bool dequeue(QueueNode<T>* &item);
    int getSize();

private:
    QueueNode<T> *head;
    QueueNode<T> *tail;
    int size;
};


template<class T>
Queue<T>::Queue()
: head(NULL)
, tail(NULL)
, size(0)
{
}

template<class T>
bool Queue<T>::push(QueueNode<T> *item)
{
    if(head == NULL) {
        head = tail = item;
        tail->setNext(NULL);
        size = 1;
        return true;
    }

    if(tail != NULL) {
        tail->setNext(item);
        item->setNext(NULL);
        tail = item;
        size++;
        return true;
    }

    return false;
}

template<class T>
bool Queue<T>::dequeue(QueueNode<T>* &item)
{
    if(head == NULL || size <= 0) {
        return false;
    }

    if(head->getNext() == NULL) {
        item = head;
        head = tail = NULL;
        size--;

        return true;
    }

    item = head;
    head = head->getNext();
    size--;

    return true;
}

template<class T>
int Queue<T>::getSize()
{
    return size;
}

#endif
