#pragma once
#ifndef STOLI_H
#define STOLI_H
#include <iostream>
#include <cstring>
#include <string>
#include <cmath>

long int stoli(const std::string &__str)
{
    long int returnn = 0;
    for (int i = 0; i < __str.size(); i++) { returnn += stoi(__str.substr(i, 1)) * pow(10,(__str.size()-1)-i); }
    return returnn;
}

#endif