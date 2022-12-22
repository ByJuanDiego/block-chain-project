#pragma once
#ifndef DOUBLELIST_H
#define DOUBLELIST_H

#include "nodes.h"
#include <type_traits>

template<typename T>
class list {
public:
    list() = default;

    virtual ~list() = default;

    virtual T FRONT() = 0;

    virtual T BACK() = 0;

    virtual void push_front(T) = 0;

    virtual void push_back(T) = 0;

    virtual void pop_front() = 0;

    virtual void pop_back() = 0;

    [[nodiscard]] virtual bool is_empty() const = 0;

    virtual void clear() = 0;

    virtual nl::node<T> *begin() const = 0;

    virtual nl::node<T> *end() const = 0;

    virtual void insert(T, int) = 0;

    /*remove section*/
    virtual T &operator[](int) const = 0;

    [[nodiscard]] virtual size_t SIZE() const = 0;
};

template<typename T>
class DoubleList : public list<T> {
private:
    nl::node<T> *front{};
    nl::node<T> *back{};
    size_t size{};

public:
    DoubleList();

    DoubleList(DoubleList<T> &other) : size(0) {
        for (int i = 0; i < other.size; ++i) {
            push_back(other[i]);
        }
    }

    ~DoubleList();

    T FRONT();

    T BACK();

    void push_front(T data);

    void push_back(T data);

    void pop_front();

    void pop_back();

    [[nodiscard]] bool is_empty() const;

    void clear();

    nl::node<T> *begin() const;

    nl::node<T> *end() const;

    void insert(T data, int pos);

    /*remove section*/
    T &operator[](int pos) const;

    [[nodiscard]] size_t SIZE() const;
};

template<typename T>
DoubleList<T>::DoubleList() {
    this->front = nullptr;
    this->back = nullptr;
    this->size = 0;
}

template<typename T>
DoubleList<T>::~DoubleList() { clear(); }

template<typename T>
T DoubleList<T>::FRONT() {
    if (!is_empty()) { return front->data; }
    else { throw std::runtime_error("empty"); }
}

template<typename T>
T DoubleList<T>::BACK() {
    if (!is_empty()) { return back->data; }
    else { throw std::runtime_error("empty"); }
}

template<typename T>
void DoubleList<T>::push_front(T data) {
    if (is_empty()) {
        front = new nl::node<T>(data);
        back = front;
    } else {
        auto *newN = new nl::node<T>(data, front);
        front->prev = newN;
        front = newN;
    }
    size++;
}

template<typename T>
void DoubleList<T>::push_back(T data) {
    if (is_empty()) { push_front(data); }
    else {
        back->next = new nl::node<T>(data, nullptr, back);
        back = back->next;
        size++;
    }
}

template<typename T>
void DoubleList<T>::pop_front() {
    if (!is_empty()) {
        if (size == 1) {
            delete front;
            front = nullptr;
            back = nullptr;
        } else {
            nl::node<T> *temp = front->next;
            delete front;
            front = temp;
            front->prev = nullptr;
        }
        size--;
    } else { throw std::runtime_error("empty"); }
}

template<typename T>
void DoubleList<T>::pop_back() {
    if (!is_empty()) {
        if (size == 1) { pop_front(); }
        else {
            nl::node<T> *temp = back->prev;
            delete back;
            back = temp;
            back->next = nullptr;
            size--;
        }
    } else { throw std::runtime_error("empty"); }
}

template<typename T>
bool DoubleList<T>::is_empty() const { return size == 0; }

template<typename T>
void DoubleList<T>::clear() {
    nl::node<T> *iterator = front;
    while (iterator != nullptr) {
        nl::node<T> *next = iterator->next;
        delete iterator;
        iterator = next;
    }
    front = nullptr;
    back = nullptr;
    size = 0;
}

template<typename T>
nl::node<T> *DoubleList<T>::begin() const { return front; }

template<typename T>
nl::node<T> *DoubleList<T>::end() const { return back; }

template<typename T>
void DoubleList<T>::insert(T data, int pos) {
    if (!is_empty() && pos < size) {
        nl::node<T> *iterator = front;
        for (int i = 0; i <= pos - 1; i++) { iterator = iterator->next; }
        auto *newN = new nl::node<T>(data, iterator->next, iterator);
        iterator->next = newN;
        newN->next->prev = newN;
        size++;
    } else { throw std::runtime_error("error"); }
}

/*remove section*/

template<typename T>
T &DoubleList<T>::operator[](int pos) const {
    if (!is_empty() && pos < size) {
        nl::node<T> *iterator = front;
        for (int i = 0; i <= pos - 1; i++) { iterator = iterator->next; }
        return iterator->data;
    } else { throw std::runtime_error("error"); }
}

template<typename T>
size_t DoubleList<T>::SIZE() const { return size; }

#endif
