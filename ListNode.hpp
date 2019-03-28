#ifndef LIST_NODE_H
#define LIST_NODE_H

#include <cstdlib>

template<class T>
class ListNode {
private:
    T value;
    ListNode<T>* next;

public:
    ListNode<T>* getNext() {
        return this->next;
    }

    void addNext(T value) {
        this->next = new ListNode<T>(value);
    }

    void setNext(ListNode<T>* next) {
        this->next = next;
    }

    ListNode(T value) {
        this->next = NULL;
        this->value = value;
    }

    T getValue() {
        return this->value;
    }

    void setValue(T value) {
        this->value = value;
    }
};

#endif
