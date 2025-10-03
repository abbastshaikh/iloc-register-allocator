#pragma once

#include <iostream>

template <typename T>
class Node {
public:
    T data;
    Node* prev;
    Node* next;

    Node(T value) : data(value), prev(nullptr), next(nullptr) {}
};

template <typename T>
class LinkedList {
public:
    Node<T>* head;
    Node<T>* tail;

    LinkedList() : head(nullptr), tail(nullptr) {}
    ~LinkedList();

    void append (T value);
    void print ();
};

template <typename T>
LinkedList<T>::~LinkedList() {
    Node<T>* current = head;
    while (current) {
        Node<T>* nextNode = current->next;
        delete current;
        current = nextNode;
    }
    head = tail = nullptr;   
}

template <typename T>
void LinkedList<T>::append(T value) { 
    Node<T>* newNode = new Node<T>(value);
    if (!head) {
        head = tail = newNode;
    } else {
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
    }
}

template <typename T>
void LinkedList<T>::print() { 
    Node<T>* current = head;
    while(current) {
        std::cout << (current -> data).toString() << std::endl;
        current = current -> next;
    }
}