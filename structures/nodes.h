#pragma once
#ifndef NODES_H
#define NODES_H

#include <iostream>
#include <functional>

namespace nl {
    template<typename T>
    struct node {
        T data;
        node<T> *next;
        node<T> *prev;

        node() = default;

        ~node() = default;

        explicit node(T data, node<T> *next = nullptr, node<T> *prev = nullptr);
    };

    template<typename T>
    node<T>::node(T data, node<T> *next, node<T> *prev) {
        this->data = data;
        this->next = next;
        this->prev = prev;
    }
}

namespace nh {
    template<typename TK, typename TV>
    struct node {
        TV value;
        TK key;

        node() = default;

        ~node() = default;

        node(TK key, TV value);
    };

    template<typename TK, typename TV>
    node<TK, TV>::node(TK key, TV value) {
        this->key = key;
        this->value = value;
    }
}

namespace nbpt {
    template<typename T>
    struct node {
        bool is_leaf{};
        size_t maxchildren{};
        size_t size{};
        T *data;
        node<T> **children;
        node<T> *parent;

        node() = default;

        ~node() = default;

        explicit node(size_t maxchildren);
    };

    template<typename T>
    node<T>::node(size_t maxchildren) {
        this->is_leaf = false;
        this->maxchildren = maxchildren;
        this->size = 0;
        this->parent = nullptr;
        this->data = new T[maxchildren - 1];
        this->children = new node<T> *[maxchildren];
        for (int i = 0; i < maxchildren; i++) { this->children[i] = nullptr; }
    }
}

#endif