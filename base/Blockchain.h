#pragma once
#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <fstream>
#include <string>
#include <sstream>
#include "block.h"

class Blockchain {
private:
    DoubleList<block *> chain {};
    Hash<std::string, nl::node<block *> *> usersHash {};

    BPlusTree<transaction *> allOrderByDate{
            23,
            [](const transaction *first, const transaction *second) { return first->date < second->date; },
            [](const transaction *first, const transaction *second) { return second->date < first->date; },
            [](const transaction *first, const transaction *second) { return second->date == first->date; }
    };
    BPlusTree<transaction *> allOrderByAmount{
            23,
            [](const transaction *first, const transaction *second) { return first->amount < second->amount; },
            [](const transaction *first, const transaction *second) { return second->amount < first->amount; },
            [](const transaction *first, const transaction *second) { return second->amount == first->amount; }
    };

public:

    ~Blockchain() = default;

    Blockchain() = default;

    explicit Blockchain(const std::string &usersPath, const std::string &transactionsPath);

    void createUser(const std::string &username, const std::string &password);

    bool searchUser(const std::string &username, const std::string &password);

    void setTx(const std::string &username, const std::string &password, const std::string &to, float amount,
               const std::string &date);

    bool searchTx(const std::string &username, const std::string &password, const std::string &to, float amount,
                  const std::string &date);

    void myTxs(const std::string &username, const std::string &password);

    void myTxsDate(const std::string &username, const std::string &password);

    void myTxsAmount(const std::string &username, const std::string &password);

    void myTxsDateRange(const std::string &username, const std::string &password, const std::string &start, const std::string& end);

    void myTxsAmountRange(const std::string &username, const std::string &password, float start, float end);

    void myMaxTxD(const std::string &username, const std::string &password);

    void myMinTxD(const std::string &username, const std::string &password);

    void myMaxTxA(const std::string &username, const std::string &password);

    void myMinTxA(const std::string &username, const std::string &password);

    static void loadTxsToFile(DoubleList<transaction*>* doubleList, const std::string& path = "./assets/users/session.txt");

    void mineCascade(const std::string &username, const std::string &password);

    friend std::ostream &operator<<(std::ostream &os, const Blockchain &tx);

};

Blockchain::Blockchain(const std::string &usersPath, const std::string &transactionsPath) {
    std::string line;
    auto *file = new std::ifstream(usersPath);
    std::getline(*file, line, '\n');
    std::string username{}, password{};

    while ((*file) >> username >> password) {
        createUser(username, password);
    }
    file->close();

    file = new std::ifstream(transactionsPath);
    std::getline(*file, line, '\n');
    std::string from, to, date, ammount;

    while ((*file) >> from >> password >> to >> ammount >> date) {
        setTx(from, password, to, std::stof(ammount), date);
    }
    file->close();
}

void Blockchain::createUser(const std::string &username, const std::string &password) {
    if (chain.is_empty()) {
        chain.push_back(new block{chain.SIZE()});
        std::stringstream ss;
        ss << username;
        ss << password;
        usersHash.set(ss.str(), chain.begin());
    } else {
        chain.push_back(new block{chain.SIZE(), chain.end()->data->hash});
        std::stringstream ss;
        ss << username;
        ss << password;
        usersHash.set(ss.str(), chain.end());
    }
}

void Blockchain::myTxs(const std::string &username, const std::string &password) {
    std::stringstream ss;
    ss << username;
    ss << password;
    DoubleList<transaction *> transactions = this->usersHash.get(ss.str())->data->data;
    loadTxsToFile(&transactions);
}

void Blockchain::myTxsDate(const std::string &username, const std::string &password) {
    std::stringstream ss;
    ss << username;
    ss << password;
    DoubleList<transaction *> transactions = usersHash.get(ss.str())->data->ObyDtoList();
    loadTxsToFile(&transactions);
}

void Blockchain::myTxsAmount(const std::string &username, const std::string &password) {
    std::stringstream ss;
    ss << username;
    ss << password;
    DoubleList<transaction *> transactions = usersHash.get(ss.str())->data->ObyAtoList();
    loadTxsToFile(&transactions);
}

void Blockchain::myMaxTxD(const std::string &username, const std::string &password) {
    std::stringstream ss;
    ss << username;
    ss << password;
    DoubleList<transaction *> transactions = usersHash.get(ss.str())->data->maxTxD();
    loadTxsToFile(&transactions);
}

void Blockchain::myMinTxD(const std::string &username, const std::string &password) {
    std::stringstream ss;
    ss << username;
    ss << password;
    DoubleList<transaction *> transactions = usersHash.get(ss.str())->data->minTxD();
    loadTxsToFile(&transactions);
}

void Blockchain::myMaxTxA(const std::string &username, const std::string &password) {
    std::stringstream ss;
    ss << username;
    ss << password;
    DoubleList<transaction *> transactions = usersHash.get(ss.str())->data->maxTxA();
    loadTxsToFile(&transactions);
}

void Blockchain::myMinTxA(const std::string &username, const std::string &password) {
    std::stringstream ss;
    ss << username;
    ss << password;
    DoubleList<transaction *> transactions = usersHash.get(ss.str())->data->minTxA();
    loadTxsToFile(&transactions);
}

void Blockchain::setTx(const std::string &username, const std::string &password, const std::string &to, float amount,
                       const std::string &date) {
    std::stringstream ss;
    ss << username;
    ss << password;
    auto *tx = new transaction(username, to, amount, date);
    usersHash.get(ss.str())->data->insert(tx);
    allOrderByDate.insert(tx);
    allOrderByAmount.insert(tx);
    usersHash.get(ss.str())->data->hash = usersHash.get(ss.str())->data->calculateHash();

    auto iterator = usersHash.get(ss.str());
    iterator = iterator->next;
    while (iterator != nullptr) {
        iterator->data->prevHash = iterator->prev->data->hash;
        iterator->data->hash = iterator->data->calculateHash();
        iterator = iterator->next;
    }
}

bool Blockchain::searchTx(const std::string &username, const std::string &password, const std::string &to, float amount,
                          const std::string &date) {
    std::stringstream ss;
    ss << username;
    ss << password;
    auto *tx = new transaction(username, to, amount, date);
    std::stringstream key;
    key << *(tx);
    return usersHash.get(ss.str())->data->dataHash.search(key.str());
}

bool Blockchain::searchUser(const std::string &username, const std::string &password) {
    std::stringstream ss;
    ss << username;
    ss << password;
    return usersHash.search(ss.str());
}

void Blockchain::mineCascade(const std::string &username, const std::string &password) {
    std::stringstream ss;
    ss << username;
    ss << password;
    usersHash.get(ss.str())->data->mine();
    auto iterator = usersHash.get(ss.str());
    iterator = iterator->next;
    while (iterator != nullptr) {
        iterator->data->prevHash = iterator->prev->data->hash;
        iterator->data->hash = iterator->data->calculateHash();
        iterator = iterator->next;
    }
}

std::ostream &operator<<(std::ostream &os, const Blockchain &tx) {
    auto iterator = tx.chain.begin();
    while (iterator != nullptr) {
        os << *(iterator->data) << std::endl;
        iterator = iterator->next;
    }
    return os;
}

void Blockchain::loadTxsToFile(DoubleList<transaction *> *doubleList, const std::string &path) {
    std::ofstream out(path);
    for (int i=0; i<doubleList->SIZE(); ++i)
    {
        transaction* tx = doubleList->operator[](i);
        out << tx->string1 << " " << tx->string2 << " " << tx->amount << " " << tx->date << std::endl;
    }
    out.close();
}

void Blockchain::myTxsDateRange(const std::string &username, const std::string &password, const std::string& start, const std::string& end) {
    std::stringstream ss;
    ss << username;
    ss << password;
    DoubleList<transaction *> transactions = usersHash.get(ss.str())->data->orderByDate.makeListRange(new transaction{"", "", 0, start}, new transaction{"", "", 0, end});
    loadTxsToFile(&transactions);
}

void Blockchain::myTxsAmountRange(const std::string &username, const std::string &password, float start, float end) {
    std::stringstream ss;
    ss << username;
    ss << password;
    DoubleList<transaction *> transactions = usersHash.get(ss.str())->data->orderByAmount.makeListRange(
            new transaction{"", "", start, ""}, new transaction{"", "", end, ""});
    loadTxsToFile(&transactions);
}

#endif