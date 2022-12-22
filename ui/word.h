//
// Created by Juan Diego on 11/14/2022.
//

#ifndef SFML_BASICS_WORD_H
#define SFML_BASICS_WORD_H

#include <SFML/Graphics.hpp>
#include <iostream>

struct Position {
    float x, y;
    Position(float x, float y) : x(x), y(y) {}
};

struct Word {
    sf::Font font;
    sf::Text texto;
    std::string mensaje;
    Position position;
    unsigned size;

    explicit Word(const std::string& str, float x, float y, const float &fontSize, sf::Color color = sf::Color::Blue, const std::string &fontPath = "./assets/fonts/Raleway-Bold.ttf")
    : mensaje(str), position(x, y)
    {
        if (!font.loadFromFile(fontPath)) {
            std::cout << "not correctly loaded font: " << fontPath << std::endl;
        }
        size = str.size();
        texto.setFont(font);
        texto.setFillColor(color);
        texto.setString(str);
        texto.setCharacterSize(static_cast<int>(fontSize));
        texto.setPosition(position.x, position.y);
    }

    void setText(const std::string& newText) {
        this->mensaje = newText;
        size = newText.length();
        texto.setString(newText);
    }

    sf::Text &getFont() {
        return texto;
    }

    void draw(sf::RenderWindow* & window) const {
        window->draw(texto);
    }

    void setFillColor(sf::Color nuevo) {
        texto.setFillColor(nuevo);
    }
};


#endif //SFML_BASICS_WORD_H
