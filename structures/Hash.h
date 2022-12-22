#pragma once
#ifndef HASHMAP_H
#define HASHMAP_H

#include "DoubleList.h"

const double maxColision = 3;
const double maxFillFactor = 0.7;

template<typename TK, typename TV>
class Hash {
private:
    DoubleList<nh::node<TK, TV>> *array;
    size_t capacity;
    size_t size;

public:
    Hash();

    ~Hash();

    explicit Hash(size_t capacity);

    void set(TK key, TV value);

    TV get(TK key);

    bool search(TK key);
    /*remove section*/

private:
    int hashFunction(TK key);

    double fillFactor();

    void rehashing();
};

template<typename TK, typename TV>
Hash<TK, TV>::Hash() {
    this->capacity = 23;
    this->size = 0;
    this->array = new DoubleList<nh::node<TK, TV>>[23];
}

template<typename TK, typename TV>
Hash<TK, TV>::~Hash() { delete[] array; }

template<typename TK, typename TV>
Hash<TK, TV>::Hash(size_t capacity) {
    this->capacity = capacity;
    this->size = 0;
    this->array = new DoubleList<nh::node<TK, TV>>[capacity];
}

template<typename TK, typename TV>
void Hash<TK, TV>::set(TK key, TV value) {
    if (fillFactor() >= maxFillFactor) { rehashing(); }
    nh::node<TK, TV> entry(key, value);
    int index = hashFunction(key);
    auto *iterator = array[index].begin();
    while (iterator != nullptr) {
        if (iterator->data.key == key) {
            iterator->data.value = value;
            break;
        }
        iterator = iterator->next;
    }
    if (iterator == nullptr) {
        array[index].push_front(entry);
        size++;
    }
}

template<typename TK, typename TV>
TV Hash<TK, TV>::get(TK key) {
    int index = hashFunction(key);
    auto *iterator = array[index].begin();
    while (iterator != nullptr) {
        if (iterator->data.key == key) { return iterator->data.value; }
        iterator = iterator->next;
    }
    throw std::runtime_error("there is no an value with the key you are searching for");
}

template<typename TK, typename TV>
bool Hash<TK, TV>::search(TK key) {
    try {
        get(key);
        return true;
    }
    catch (const std::runtime_error &rte) { return false; }
}

template<typename TK, typename TV>
int Hash<TK, TV>::hashFunction(TK key) {
    std::hash<TK> ptr_hash;
    return ptr_hash(key) % capacity;
}

template<typename TK, typename TV>
double Hash<TK, TV>::fillFactor() { return (size / (capacity * maxColision)); }

template<typename TK, typename TV>
void Hash<TK, TV>::rehashing() {
    int prevCapacity = capacity;
    capacity *= 2;
    DoubleList<nh::node<TK, TV>> *prevArray = array;
    array = new DoubleList<nh::node<TK, TV>>[capacity];

    for (int i = 0; i < prevCapacity; i++) {
        auto *iterator = prevArray[i].begin();
        while (iterator != nullptr) {
            set(iterator->data.key, iterator->data.value);
            iterator = iterator->next;
        }
    }
    delete[] prevArray;
}

#endif