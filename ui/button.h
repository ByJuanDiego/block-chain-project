//
// Created by Juan Diego on 11/14/2022.
//

#ifndef SFML_BASICS_BUTTON_H
#define SFML_BASICS_BUTTON_H

#include <SFML/Graphics.hpp>
#include "word.h"
#include <iostream>
#include <utility>

struct Button {
protected:
    sf::RectangleShape* rectangle;
    Position position;
    Word* word;
    float width;
    float height;

public:
    explicit Button(sf::Color color, float x, float y, float h, float w, const std::string& text, const float& textSize, sf::Color textColor = sf::Color::Yellow, const std::string& path = "./assets/fonts/Raleway-Bold.ttf")
    : width(w), height(h), position(x, y) {
        rectangle = new sf::RectangleShape;
        rectangle->setFillColor(color);
        rectangle->setPosition(x, y);
        rectangle->setSize(sf::Vector2(w, h));
        word = new Word(text, x, y, textSize, textColor, path);
        this->centerWord();
    }

    void draw(sf::RenderWindow* &window) {
        window->draw(*rectangle);
        word->draw(window);
    }

    [[nodiscard]] bool isOnBound(int x, int y) const {
        return (static_cast<float>(x) > position.x && static_cast<float>(x) < position.x + width) && (static_cast<float>(y) > position.y && static_cast<float>(y) < position.y + height);
    }

    void centerWord() {
        const sf::FloatRect bounds(word->texto.getLocalBounds());
        const auto box(rectangle->getSize());
        word->texto.setOrigin((bounds.width - box.x) / 2 + bounds.left, (bounds.height - box.y) / 2 + bounds.top);
    }

    void mouseEnterEvent() {
        sf::Color temp = rectangle->getFillColor();
        rectangle->setFillColor(word->texto.getFillColor());
        word->texto.setFillColor(temp);
    }

    void mouseLeaveEvent() {
        sf::Color temp = rectangle->getFillColor();
        rectangle->setFillColor(word->texto.getFillColor());
        word->texto.setFillColor(temp);
    }

    Word* &getWord() {
        return this->word;
    }
};


#endif //SFML_BASICS_BUTTON_H
