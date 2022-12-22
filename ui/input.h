//
// Created by Juan Diego on 11/20/2022.
//

#ifndef BLOCK_CHAIN_INPUT_H
#define BLOCK_CHAIN_INPUT_H

#include "button.h"

class InputField : public Button {
public:
    // Usa el mismo constructor del padre
    using Button::Button;

    void updateText(const std::string& newText) {
        this->word->setText(newText);
        centerWord();
    }

    void deleteLastCharacter() {
        if (word->size < 1) {
            return;
        }
        word->mensaje = word->mensaje.substr(0, --word->size);
        word->texto.setString(word->mensaje);
        centerWord();
    }
};

#endif //BLOCK_CHAIN_INPUT_H
