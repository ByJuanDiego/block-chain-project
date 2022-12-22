#include "gui.h"

int main(int argc, char const *argv[]) {
    auto* blockchain = new Blockchain("./assets/users/users.txt", "./assets/users/transactions.txt");
    auto* gui = new Gui;
    if (gui->initialize(blockchain)) {
        gui->runLoop();
    }
    delete gui;
    return 0;
}
