//
// Created by Juan Diego on 11/20/2022.
//

#ifndef BLOCK_CHAIN_PASSWORD_H
#define BLOCK_CHAIN_PASSWORD_H

#include "button.h"

class PasswordField : public Button {
private:
    bool showingPassword = false;
    std::string passwordCopy {};

    void updateText(const std::string& newText) {
        if (showingPassword) {
            word->setText(passwordCopy);
        } else {
            word->setText(std::string(passwordCopy.length(), '*'));
        }
        centerWord();
    }

public:
    using Button::Button;

    void hidePassword() {
        if (!showingPassword) {
            return;
        }

        passwordCopy = word->mensaje;
        showingPassword = false;
        updateText(std::string(word->size, '*'));
    }

    void showPassword() {
        if (showingPassword) {
            return;
        }

        showingPassword = true;
        updateText(passwordCopy);
    }

    void updateText(char newChar) {
        passwordCopy += newChar;

        if (showingPassword) {
            updateText(passwordCopy);
        } else {
            updateText(std::string(passwordCopy.length(), '*'));
        }
    }

    void deleteLastCharacter() {
        if (word->size < 1) {
            return;
        }

        word->mensaje = word->mensaje.substr(0, --word->size);
        passwordCopy = passwordCopy.substr(0, word->size);
        word->texto.setString(word->mensaje);
        centerWord();
    }
};

#endif //BLOCK_CHAIN_PASSWORD_H
