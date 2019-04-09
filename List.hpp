#ifndef LIST_H
#define LIST_H

#include "ListNode.hpp"
#include <cstdlib>
#include <iostream>

template <class T>
class List {
private:
    ListNode<T>* root;
    int size;

public:
    List() {
        this->root = NULL;
        this->size = 0;
    }

    ~List() {
        ListNode<T>* current = this->root;
        ListNode<T>* toDelete;
        while (current != NULL) {
            toDelete = current;
            current = current->getNext();
            delete toDelete;
        }
    }

    void push(T value) {
        if (root == NULL) {
            root = new ListNode<T>(value);
            this->size++;
            return;
        }
        ListNode<T>* current = root;
        while (current->getNext() != NULL) current = current->getNext();
        current->addNext(value);
        this->size++;
    }

    T get(unsigned int index) {
        unsigned int current = 0;
        ListNode<T>* node = this->root;
        while(current < index) {
            node = node->getNext();
            current++;
        }
        return node->getValue();
    }

    void remove(int index) {
        if (index == 0) {
            ListNode<T>* toDelete = this->root;
            this->root = this->root->getNext();
            delete toDelete;
            this->size--;
            return;
        }
        int current = 0;
        ListNode<T>* node = this->root;
        ListNode<T>* prev;
        while(current < index) {
            prev = node;
            node = node->getNext();
            current++;
        }
        prev->setNext(node->getNext());
        delete node;
        this->size--;
    }

    void set(unsigned int index, T value) {
        unsigned int current = 0;
        ListNode<T>* node = this->root;
        while(current < index) {
            node = node->getNext();
            current++;
        }
        node->setValue(value);
    }

    void clearValues() {
        ListNode<T>* current = this->root;
        ListNode<T>* toDelete;
        while(current != NULL) {
            toDelete = current;
            current = current->getNext();
            delete toDelete->getValue();
        }
    }

    int findIndex(T value) {
        ListNode<T>* current = this->root;
        int index = -1;
        int currentIndex = 0;
        while (current != NULL) {
            if (current->getValue() == value) {
                index = currentIndex;
                break;
            } else {
                currentIndex++;
                current = current->getNext();
            }
        }
        return index;
    }

    void concat(List<T>* list) {
        ListNode<T>* current = list->root;

        while (current != NULL) {
            this->push(current->getValue());
        }
    }

    unsigned int getSize() {
        return this->size;
    }

    T operator[] (unsigned int index) {
        return this->get(index);
    }
};

#endif
