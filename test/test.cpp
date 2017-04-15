#include <stdio.h>
#include "Queue.h"

int main() {
    Queue<int> queue;
    QueueNode<int> node0(0);
    QueueNode<int> node1(1);
    QueueNode<int> node2(2);
    QueueNode<int> node3(3);
    queue.push(&node0);
    queue.push(&node1);
    queue.push(&node2);
    queue.push(&node3);

    QueueNode<int>* ptr = NULL;
    while(queue.dequeue(ptr)) {
        printf("val %d\n", ptr->getVal());
    }
    return 0;
}
