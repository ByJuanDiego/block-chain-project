#pragma once
#ifndef SWAP_H
#define SWAP_H
#include <iostream>
#include<functional>

template<typename T>
void swap(T &x, T &y)
{
    T temp = x;
    x = y;
    y = temp;
}

#endif