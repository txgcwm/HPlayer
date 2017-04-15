#ifndef __QUEUENODE__H__
#define __QUEUENODE__H__

#include <iostream>

template<class T>
class QueueNode
{
public:
    QueueNode(T v);
    void setNext(QueueNode *next);
    void setVal(T val);
    QueueNode<T>* getNext();
    T getVal();
    ~QueueNode();

private:
    T val;
    QueueNode<T> *next;
};

template<class T>
QueueNode<T>::QueueNode(T v)
{
    this->val = v;
    this->next = NULL;
}

template<class T>
void QueueNode<T>::setNext(QueueNode<T> *next)
{
    this->next = next;
}

template<class T>
QueueNode<T>* QueueNode<T>::getNext()
{
    return next;
}

template<class T>
void QueueNode<T>::setVal(T v)
{
    this->val = v;
}

template<class T>
T QueueNode<T>::getVal()
{
    return val;
}

template<class T>
QueueNode<T>::~QueueNode()
{
    this->next = NULL;
}

#endif

