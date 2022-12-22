//
// Created by Juan Diego on 11/14/2022.
//

#ifndef BLOCK_CHAIN_GUI_H
#define BLOCK_CHAIN_GUI_H

#include <SFML/Graphics.hpp>

#include <fstream>
#include <iomanip>
#include <vector>
#include <map>

#include "button.h"
#include "input.h"
#include "password.h"
#include "Blockchain.h"
#include "utils/unix.h"
#include "Hash.h"
#include "DoubleList.h"

enum class options {
    register_,
    login,
    close,
    showTransactions,
    showMenu,
    newTransaction
};

class Gui {
private:
    sf::RenderWindow *window{};
    Blockchain *blockchain{};
    std::string sessionUsername{};
    std::string sessionPassword{};
    Hash<std::string, sf::Color> colors;

public:
    explicit Gui() = default;

    virtual ~Gui() {
        delete window;
    }

    bool initialize(Blockchain *bch) {
        blockchain = bch;
        sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
        window = new sf::RenderWindow(
                sf::VideoMode((int) (desktop.width * 2.0 / 3.0), (int) (desktop.height * 2.0 / 3.0)), "BlockChain");
        window->setPosition(sf::Vector2<int>((int) (desktop.width / 2.0 - window->getSize().x / 2.0),
                                             (int) (desktop.height / 2.0 - window->getSize().y / 2.0)));

        colors.set("red", sf::Color::Red);
        colors.set("black", sf::Color::Black);
        colors.set("blue", sf::Color::Blue);
        colors.set("yellow", sf::Color::Yellow);
        colors.set("white", sf::Color::White);
        colors.set("cyan", sf::Color::Cyan);
        colors.set("green", sf::Color::Green);

        return (window->isOpen() && blockchain);
    }

    void runLoop() {
        options option = this->showMenu();
        while (option != options::close) {
            switch (option) {
                case options::showMenu:
                    option = showMenu();
                    break;
                case options::login:
                    option = loginUser();
                    break;
                case options::register_:
                    option = registerUser();
                    break;
                case options::showTransactions:
                    option = showTransactions();
                    break;
                case options::newTransaction:
                    option = newTransaction();
                    break;
                default:
                    break;
            }
        }
        closeWindow();
    }

    options newTransaction() {
        Word messageUsername("Username to transfer:", 495, 180, 30, sf::Color::Yellow);
        Word messageAmount("Enter the amount:", 520, 330, 30, sf::Color::Yellow);

        InputField Username(sf::Color::White, 500, 230, 50, 300, "", 25, sf::Color::Black);
        InputField Amount(sf::Color::White, 500, 380, 50, 300, "", 25, sf::Color::Black);
        Button Send(sf::Color::Blue, 570, 470, 60, 160, "Transfer", 30, sf::Color::Yellow);
        Button Cancel(sf::Color::Red, 1120, 50, 50, 120, "Cancel", 25, sf::Color::Black);

        sf::Vector2 mousePosition = sf::Mouse::getPosition(*window);
        int actual_x, actual_y, prior_x, prior_y;

        while (window->isOpen()) {
            sf::Event event{};
            while (window->pollEvent(event)) {
                mousePosition = sf::Mouse::getPosition(*window);
                actual_x = mousePosition.x;
                actual_y = mousePosition.y;

                switch (event.type) {
                    case sf::Event::Closed:
                        return options::close;
                    case sf::Event::TextEntered:
                        if (event.text.unicode >= 128 || event.text.unicode <= 47) {
                            break;
                        }
                        if (Username.isOnBound(actual_x, actual_y)) {
                            Username.updateText(Username.getWord()->mensaje + static_cast<char>(event.text.unicode));
                        } else if (event.text.unicode >= 48 && event.text.unicode <= 57 &&
                                   Amount.isOnBound(actual_x, actual_y)) {
                            Amount.updateText(Amount.getWord()->mensaje + static_cast<char>(event.text.unicode));
                        }
                        break;
                    case sf::Event::KeyPressed:
                        switch (event.key.code) {
                            case sf::Keyboard::BackSpace:
                                if (Username.isOnBound(actual_x, actual_y)) {
                                    Username.deleteLastCharacter();
                                } else if (Amount.isOnBound(actual_x, actual_y)) {
                                    Amount.deleteLastCharacter();
                                }
                                break;
                            default:
                                break;
                        }
                        break;
                    case sf::Event::MouseMoved:
                        if (Send.isOnBound(actual_x, actual_y) && !Send.isOnBound(prior_x, prior_y)) {
                            Send.mouseEnterEvent();
                        } else if (Send.isOnBound(prior_x, prior_y) && !Send.isOnBound(actual_x, actual_y)) {
                            Send.mouseLeaveEvent();
                        }
                        if (Cancel.isOnBound(actual_x, actual_y) && !Cancel.isOnBound(prior_x, prior_y)) {
                            Cancel.mouseEnterEvent();
                        } else if (Cancel.isOnBound(prior_x, prior_y) && !Cancel.isOnBound(actual_x, actual_y)) {
                            Cancel.mouseLeaveEvent();
                        }
                        prior_x = actual_x;
                        prior_y = actual_y;
                        break;
                    case sf::Event::MouseButtonPressed:
                        if (Send.isOnBound(actual_x, actual_y)) {
                            std::string to = Username.getWord()->mensaje;
                            std::string amount = Amount.getWord()->mensaje;
                            loadNewTransaction(this->sessionUsername, this->sessionPassword, to, amount);
                            return options::showTransactions;
                        } else if (Cancel.isOnBound(actual_x, actual_y)) {
                            return options::showTransactions;
                        }
                        break;
                    default:
                        break;
                }
            }

            window->clear();
            Username.draw(window);
            Amount.draw(window);
            Send.draw(window);
            Cancel.draw(window);
            window->draw(messageUsername.texto);
            window->draw(messageAmount.texto);
            window->display();
        }

        return {};
    }

    void loadNewTransaction(const std::string &from, const std::string &password, const std::string &to,
                            const std::string &amount) {
        time_t curr_time;
        curr_time = time(nullptr);

        tm *tm_local = localtime(&curr_time);
        auto unixTimestamp = timegm(tm_local);

        std::ofstream outTransactions("./assets/users/transactions.txt", std::ios_base::app);
        outTransactions << from << " " << password << " " << to << " " << amount << " " << unixTimestamp << std::endl;
        this->blockchain->setTx(from, password, to, std::stof(amount), std::to_string(unixTimestamp));
        outTransactions.close();
    }

    options showTransactions() {
        blockchain->myTxs(sessionUsername, sessionPassword);

        std::stringstream rowText;
        rowText << std::left << std::setw(15) << "sender" << std::left << std::setw(15) << "receiver" << std::left
                << std::setw(15) << "amount" << std::left << std::setw(15) << "full date";
        Word myTransactions("These are your completed transactions!", 350, 50, 30, sf::Color::Cyan);
        Word row(rowText.str(), 50, 160, 24, sf::Color::Cyan, "./assets/fonts/Anonymous_Pro.ttf");

        // Boton de logout
        Button LogOut(sf::Color::Red, 1120, 50, 50, 120, "Logout", 25, sf::Color::Black);

        // Boton de nueva transaccion
        Button newTransaction(sf::Color::Blue, 1000, 50, 50, 90, "New", 20, sf::Color::Yellow);

        // Realiza minado en cascada
        Button mineCascade(sf::Color::Blue, 50, 50, 50, 50, "Mine", 20, sf::Color::Yellow);

        // Botones del amount range
        Button amountRange(sf::Color::Green, 1000, 200, 50, 240, "Amount range", 25, sf::Color::Black);
        InputField minAmount(sf::Color::White, 1000, 270, 50, 110, "", 20, sf::Color::Black, "./assets/fonts/Anonymous_Pro.ttf");
        InputField maxAmount(sf::Color::White, 1130, 270, 50, 110, "", 20, sf::Color::Black, "./assets/fonts/Anonymous_Pro.ttf");

        // Botones del date range
        Button dateRange(sf::Color::Green, 1000, 340, 50, 240, "Date range d/m/y h:m:s", 16, sf::Color::Black);
        InputField minDate(sf::Color::White, 1000, 410, 25, 240, "", 20, sf::Color::Black, "./assets/fonts/Anonymous_Pro.ttf");
        InputField maxDate(sf::Color::White, 1000, 450, 25, 240, "", 20, sf::Color::Black, "./assets/fonts/Anonymous_Pro.ttf");

        // Boton de max amount
        Button topMaxAmount(sf::Color::Green, 1000, 500, 50, 110, "Max Amount", 17, sf::Color::Black);
        // Boton de min amount
        Button topMinAmount(sf::Color::Green, 1130, 500, 50, 110, "Min Amount", 17, sf::Color::Black);
        // Boton de max DATE
        Button topMaxDate(sf::Color::Green, 1000, 570, 50, 110, "Max Date", 17, sf::Color::Black);
        // Boton de min DATE
        Button topMinDate(sf::Color::Green, 1130, 570, 50, 110, "Min Date", 17, sf::Color::Black);

        sf::Vector2 mousePosition = sf::Mouse::getPosition(*window);
        int actual_x, actual_y, prior_x, prior_y;

        while (window->isOpen()) {
            sf::Event event{};
            while (window->pollEvent(event)) {
                mousePosition = sf::Mouse::getPosition(*window);
                actual_x = mousePosition.x;
                actual_y = mousePosition.y;

                switch (event.type) {
                    case sf::Event::Closed:
                        return options::close;
                    case sf::Event::TextEntered: {
                        char new_char = static_cast<char>(event.text.unicode);
                        if ((event.text.unicode >= 128 || event.text.unicode <= 46) && event.text.unicode != 32) {
                            break;
                        }

                        if (minDate.isOnBound(actual_x, actual_y)) {
                            minDate.updateText(minDate.getWord()->mensaje + new_char);
                        } else if (maxDate.isOnBound(actual_x, actual_y)) {
                            maxDate.updateText(maxDate.getWord()->mensaje + new_char);
                        }

                        if (event.text.unicode >= 58) {
                            break;
                        }
                        if (minAmount.isOnBound(actual_x, actual_y)) {
                            minAmount.updateText(minAmount.getWord()->mensaje + new_char);
                        } else if (maxAmount.isOnBound(actual_x, actual_y)) {
                            maxAmount.updateText(maxAmount.getWord()->mensaje + new_char);
                        }
                    }
                    case sf::Event::KeyPressed:
                        switch (event.key.code) {
                            case sf::Keyboard::BackSpace:
                                if (minAmount.isOnBound(actual_x, actual_y)) {
                                    minAmount.deleteLastCharacter();
                                } else if (maxAmount.isOnBound(actual_x, actual_y)) {
                                    maxAmount.deleteLastCharacter();
                                }
                                if (minDate.isOnBound(actual_x, actual_y)) {
                                    minDate.deleteLastCharacter();
                                } else if (maxDate.isOnBound(actual_x, actual_y)) {
                                    maxDate.deleteLastCharacter();
                                }
                                break;
                            default:
                                break;
                        }
                    case sf::Event::MouseMoved:
                        if (LogOut.isOnBound(actual_x, actual_y) && !LogOut.isOnBound(prior_x, prior_y)) {
                            LogOut.mouseEnterEvent();
                        } else if (LogOut.isOnBound(prior_x, prior_y) && !LogOut.isOnBound(actual_x, actual_y)) {
                            LogOut.mouseLeaveEvent();
                        }
                        if (newTransaction.isOnBound(actual_x, actual_y) && !newTransaction.isOnBound(prior_x, prior_y)) {
                            newTransaction.mouseEnterEvent();
                        } else if (newTransaction.isOnBound(prior_x, prior_y) && !newTransaction.isOnBound(actual_x, actual_y)) {
                            newTransaction.mouseLeaveEvent();
                        }
                        if (amountRange.isOnBound(actual_x, actual_y) && !amountRange.isOnBound(prior_x, prior_y)) {
                            amountRange.mouseEnterEvent();
                        } else if (amountRange.isOnBound(prior_x, prior_y) && !amountRange.isOnBound(actual_x, actual_y)) {
                            amountRange.mouseLeaveEvent();
                        }
                        if (dateRange.isOnBound(actual_x, actual_y) && !dateRange.isOnBound(prior_x, prior_y)) {
                            dateRange.mouseEnterEvent();
                        } else if (dateRange.isOnBound(prior_x, prior_y) && !dateRange.isOnBound(actual_x, actual_y)) {
                            dateRange.mouseLeaveEvent();
                        }
                        if (topMinAmount.isOnBound(actual_x, actual_y) && !topMinAmount.isOnBound(prior_x, prior_y)) {
                            topMinAmount.mouseEnterEvent();
                        } else if (topMinAmount.isOnBound(prior_x, prior_y) && !topMinAmount.isOnBound(actual_x, actual_y)) {
                            topMinAmount.mouseLeaveEvent();
                        }
                        if (topMaxAmount.isOnBound(actual_x, actual_y) && !topMaxAmount.isOnBound(prior_x, prior_y)) {
                            topMaxAmount.mouseEnterEvent();
                        } else if (topMaxAmount.isOnBound(prior_x, prior_y) && !topMaxAmount.isOnBound(actual_x, actual_y)) {
                            topMaxAmount.mouseLeaveEvent();
                        }
                        if (topMinDate.isOnBound(actual_x, actual_y) && !topMinDate.isOnBound(prior_x, prior_y)) {
                            topMinDate.mouseEnterEvent();
                        } else if (topMinDate.isOnBound(prior_x, prior_y) && !topMinDate.isOnBound(actual_x, actual_y)) {
                            topMinDate.mouseLeaveEvent();
                        }
                        if (topMaxDate.isOnBound(actual_x, actual_y) && !topMaxDate.isOnBound(prior_x, prior_y)) {
                            topMaxDate.mouseEnterEvent();
                        } else if (topMaxDate.isOnBound(prior_x, prior_y) && !topMaxDate.isOnBound(actual_x, actual_y)) {
                            topMaxDate.mouseLeaveEvent();
                        }
                        if (mineCascade.isOnBound(actual_x, actual_y) && !mineCascade.isOnBound(prior_x, prior_y)) {
                            mineCascade.mouseEnterEvent();
                        } else if (mineCascade.isOnBound(prior_x, prior_y) && !mineCascade.isOnBound(actual_x, actual_y)) {
                            mineCascade.mouseLeaveEvent();
                        }

                        prior_x = actual_x;
                        prior_y = actual_y;
                        break;
                    case sf::Event::MouseButtonPressed:
                        if (LogOut.isOnBound(actual_x, actual_y)) {
                            this->sessionUsername.clear();
                            this->sessionPassword.clear();
                            return options::showMenu;
                        } else if (newTransaction.isOnBound(actual_x, actual_y)) {
                            return options::newTransaction;
                        } else if (amountRange.isOnBound(actual_x, actual_y)) {
                            blockchain->myTxsAmountRange(sessionUsername, sessionPassword,
                                                         std::stof(minAmount.getWord()->mensaje),
                                                         std::stof(maxAmount.getWord()->mensaje));
                            minAmount.updateText("");
                            maxAmount.updateText("");
                        } else if (dateRange.isOnBound(actual_x, actual_y)) {
                            std::string minimumDate = minDate.getWord()->mensaje;
                            std::string maximumDate = maxDate.getWord()->mensaje;
                            std::string minDateUnixTimestamp = dateToUnixTimestamp(minimumDate);
                            std::string maxDateUnixTimestamp = dateToUnixTimestamp(maximumDate);
                            blockchain->myTxsDateRange(sessionUsername, sessionPassword, minDateUnixTimestamp, maxDateUnixTimestamp);
                            minDate.updateText("");
                            maxDate.updateText("");
                        } else if (topMinAmount.isOnBound(actual_x, actual_y)) {
                            blockchain->myMinTxA(sessionUsername, sessionPassword);
                        } else if (topMaxAmount.isOnBound(actual_x, actual_y)) {
                            blockchain->myMaxTxA(sessionUsername, sessionPassword);
                        } else if (topMaxDate.isOnBound(actual_x, actual_y)) {
                            blockchain->myMaxTxD(sessionUsername, sessionPassword);
                        } else if (topMinDate.isOnBound(actual_x, actual_y)) {
                            blockchain->myMinTxD(sessionUsername, sessionPassword);
                        } else if (mineCascade.isOnBound(actual_x, actual_y)) {
                            std::cout << "ANTES DEL MINE CASCADE:" << std::endl;
                            std::cout << *this->blockchain << std::endl;
                            blockchain->mineCascade(this->sessionUsername, this->sessionPassword);
                            std::cout << "DESPUES DEL MINE CASCADE:" << std::endl;
                            std::cout << *this->blockchain << std::endl;
                        }
                        break;
                    default:
                        break;
                }
            }

            window->clear();
            myTransactions.draw(window);
            row.draw(window);
            LogOut.draw(window);
            newTransaction.draw(window);
            amountRange.draw(window);
            minAmount.draw(window);
            maxAmount.draw(window);
            dateRange.draw(window);
            minDate.draw(window);
            maxDate.draw(window);
            topMaxAmount.draw(window);
            topMinAmount.draw(window);
            topMaxDate.draw(window);
            topMinDate.draw(window);
            mineCascade.draw(window);

            float x = 50;
            float y = 200;
            std::ifstream transactionsToShow("./assets/users/session.txt");
            std::string sender, reciever, amount, unixTimestamp;

            while (transactionsToShow >> sender >> reciever >> amount >> unixTimestamp) {
                std::time_t time_t_date(std::stol(unixTimestamp));
                std::string fullDate = std::asctime(std::gmtime(&time_t_date));

                std::stringstream ss_transaction;
                ss_transaction << std::left << std::setw(15) << sender << std::left << std::setw(15) << reciever
                               << std::left
                               << std::setw(15) << amount << std::left << std::setw(15)
                               << fullDate;
                Word transaction(ss_transaction.str(), x, y, 24, sf::Color::Yellow, "./assets/fonts/Anonymous_Pro.ttf");
                window->draw(transaction.texto);
                y += 50;
            }
            window->display();
        }
        return options::close;
    }

    void closeWindow() {
        std::cout << "closing window" << std::endl;
        window->close();
    }

    options loginUser() {
        Word messageUsername("Username:", 560, 180, 30, sf::Color::Yellow);
        Word messagePassword("Password:", 560, 330, 30, sf::Color::Yellow);

        InputField Username(sf::Color::White, 500, 230, 50, 300, "", 25, sf::Color::Black);
        PasswordField Password(sf::Color::White, 500, 380, 50, 300, "", 25, sf::Color::Black);
        Button Login(sf::Color::Blue, 570, 470, 60, 160, "Login", 30, sf::Color::Yellow);
        Button Back(sf::Color::Red, 1120, 50, 50, 100, "Back", 25, sf::Color::Black);

        sf::Vector2 mousePosition = sf::Mouse::getPosition(*window);
        int actual_x, actual_y, prior_x, prior_y;

        while (window->isOpen()) {
            sf::Event event{};
            while (window->pollEvent(event)) {
                mousePosition = sf::Mouse::getPosition(*window);
                actual_x = mousePosition.x;
                actual_y = mousePosition.y;

                switch (event.type) {
                    case sf::Event::Closed:
                        return options::close;
                    case sf::Event::TextEntered:
                        if (event.text.unicode >= 128 || event.text.unicode <= 47) {
                            break;
                        }
                        if (Username.isOnBound(actual_x, actual_y)) {
                            Username.updateText(Username.getWord()->mensaje + static_cast<char>(event.text.unicode));
                        } else if (Password.isOnBound(actual_x, actual_y)) {
                            Password.updateText(static_cast<char>(event.text.unicode));
                        }
                        break;
                    case sf::Event::KeyPressed:
                        switch (event.key.code) {
                            case sf::Keyboard::BackSpace:
                                if (Username.isOnBound(actual_x, actual_y)) {
                                    Username.deleteLastCharacter();
                                } else if (Password.isOnBound(actual_x, actual_y)) {
                                    Password.deleteLastCharacter();
                                }
                            default:
                                break;
                        }
                        break;
                    case sf::Event::MouseMoved:
                        if (Login.isOnBound(actual_x, actual_y) && !Login.isOnBound(prior_x, prior_y)) {
                            Login.mouseEnterEvent();
                        } else if (Login.isOnBound(prior_x, prior_y) && !Login.isOnBound(actual_x, actual_y)) {
                            Login.mouseLeaveEvent();
                        }
                        if (Back.isOnBound(actual_x, actual_y) && !Back.isOnBound(prior_x, prior_y)) {
                            Back.mouseEnterEvent();
                        } else if (Back.isOnBound(prior_x, prior_y) && !Back.isOnBound(actual_x, actual_y)) {
                            Back.mouseLeaveEvent();
                        }
                        if (Password.isOnBound(actual_x, actual_y)) {
                            Password.showPassword();
                        } else if (Password.isOnBound(prior_x, prior_y)) {
                            Password.hidePassword();
                        }
                        prior_x = actual_x;
                        prior_y = actual_y;
                        break;
                    case sf::Event::MouseButtonPressed:
                        if (Login.isOnBound(actual_x, actual_y)) {
                            Password.showPassword();
                            this->sessionUsername = Username.getWord()->mensaje;
                            this->sessionPassword = Password.getWord()->mensaje;
                            return options::showTransactions;
                        } else if (Back.isOnBound(actual_x, actual_y)) {
                            return options::showMenu;
                        }
                    default:
                        break;
                }
            }

            window->clear();
            Back.draw(window);
            Login.draw(window);
            Username.draw(window);
            Password.draw(window);
            messagePassword.draw(window);
            messageUsername.draw(window);
            window->display();
        }

        return {};
    }

    options registerUser() {
        Word messageUsername("Register Username:", 500, 180, 30, sf::Color::Yellow);
        Word messagePassword("Register Password:", 500, 330, 30, sf::Color::Yellow);

        InputField Username(sf::Color::White, 500, 230, 50, 300, "", 25, sf::Color::Black);
        PasswordField Password(sf::Color::White, 500, 380, 50, 300, "", 25, sf::Color::Black);
        Button Register(sf::Color::Blue, 560, 470, 60, 170, "Register", 30, sf::Color::Yellow);
        Button Back(sf::Color::Red, 1120, 50, 50, 100, "Back", 25, sf::Color::Black);

        sf::Vector2 mousePosition = sf::Mouse::getPosition(*window);
        int actual_x, actual_y;
        int prior_x, prior_y;

        while (window->isOpen()) {
            sf::Event event{};
            while (window->pollEvent(event)) {
                mousePosition = sf::Mouse::getPosition(*window);
                actual_x = mousePosition.x;
                actual_y = mousePosition.y;

                switch (event.type) {
                    case sf::Event::Closed:
                        return options::close;
                    case sf::Event::TextEntered:
                        if (event.text.unicode >= 128 || event.text.unicode <= 47) {
                            break;
                        }
                        if (Username.isOnBound(actual_x, actual_y)) {
                            Username.updateText(Username.getWord()->mensaje + static_cast<char>(event.text.unicode));
                        } else if (Password.isOnBound(actual_x, actual_y)) {
                            Password.updateText(static_cast<char>(event.text.unicode));
                        }
                        break;
                    case sf::Event::KeyPressed:
                        switch (event.key.code) {
                            case sf::Keyboard::BackSpace:
                                if (Username.isOnBound(actual_x, actual_y)) {
                                    Username.deleteLastCharacter();
                                } else if (Password.isOnBound(actual_x, actual_y)) {
                                    Password.deleteLastCharacter();
                                }
                            default:
                                break;
                        }
                        break;
                    case sf::Event::MouseMoved:
                        if (Register.isOnBound(actual_x, actual_y) && !Register.isOnBound(prior_x, prior_y)) {
                            Register.mouseEnterEvent();
                        } else if (Register.isOnBound(prior_x, prior_y) && !Register.isOnBound(actual_x, actual_y)) {
                            Register.mouseLeaveEvent();
                        }

                        if (Back.isOnBound(actual_x, actual_y) && !Back.isOnBound(prior_x, prior_y)) {
                            Back.mouseEnterEvent();
                        } else if (Back.isOnBound(prior_x, prior_y) && !Back.isOnBound(actual_x, actual_y)) {
                            Back.mouseLeaveEvent();
                        }
                        if (Password.isOnBound(actual_x, actual_y)) {
                            Password.showPassword();
                        } else if (Password.isOnBound(prior_x, prior_y)) {
                            Password.hidePassword();
                        }
                        prior_x = actual_x;
                        prior_y = actual_y;
                        break;

                    case sf::Event::MouseButtonPressed:
                        if (Register.isOnBound(actual_x, actual_y)) {
                            Password.showPassword();
                            std::string username = Username.getWord()->mensaje;
                            std::string password = Password.getWord()->mensaje;
                            loadToFile(username, password, "./assets/users/users.txt");
                            loadToBlockchain(username, password);
                            return options::showMenu;
                        } else if (Back.isOnBound(actual_x, actual_y)) {
                            return options::showMenu;
                        }
                    default:
                        break;
                }
            }

            window->clear();
            Register.draw(window);
            Username.draw(window);
            Password.draw(window);
            Back.draw(window);
            messagePassword.draw(window);
            messageUsername.draw(window);
            window->display();
        }

        return {};
    }

    static void loadToFile(const std::string &username, const std::string &password, const std::string &path) {
        std::ofstream outFile(path, std::ios_base::app);
        if (outFile.fail()) {
            std::cout << "error loading file: " << path << std::endl;
        }
        outFile << username << " " << password << std::endl;
        outFile.close();
    }

    void loadToBlockchain(const std::string &username, const std::string &password) {
        blockchain->createUser(username, password);
        std::cout << *blockchain << std::endl;
    }

    options showMenu() {
        sf::Vector2 size = window->getSize();
        DoubleList<Button*> buttons;

        Hash<std::string, options> hashOptions;
        hashOptions.set("Login", options::login);
        hashOptions.set("Close", options::close);
        hashOptions.set("Register", options::register_);

        float largo, alto, x, y, textSize;
        std::string text, color, textColor;

        std::ifstream file("./assets/components/menu/buttons.txt");
        std::getline(file, text);
        while (file >> x >> y >> largo >> alto >> color >> text >> textSize >> textColor) {
            buttons.push_back(new Button(colors.get(color), x, y, alto, largo, text, textSize, colors.get(textColor)));
        }

        int actual_x, actual_y, prior_x, prior_y;
        sf::Vector2 mousePosition = sf::Mouse::getPosition(*window);

        while (window->isOpen()) {
            sf::Event event{};
            while (window->pollEvent(event)) {
                mousePosition = sf::Mouse::getPosition(*window);
                actual_x = mousePosition.x;
                actual_y = mousePosition.y;

                switch (event.type) {
                    case sf::Event::Closed:
                        return options::close;
                    case sf::Event::MouseMoved: {
                        nl::node<Button *> *button = buttons.begin();
                        for (int i=0; i<buttons.SIZE(); ++i) {
                            if (button->data->isOnBound(actual_x, actual_y) && !button->data->isOnBound(prior_x, prior_y)) {
                                button->data->mouseEnterEvent();
                            } else if (button->data->isOnBound(prior_x, prior_y) && !button->data->isOnBound(actual_x, actual_y)) {
                                button->data->mouseLeaveEvent();
                            }
                            button = button->next;
                        }
                        prior_x = actual_x;
                        prior_y = actual_y;
                        break;
                    }
                    case sf::Event::MouseButtonPressed: {
                        nl::node<Button *> *button = buttons.begin();
                        for (int i=0; i<buttons.SIZE(); ++i) {
                            if (button->data->isOnBound(actual_x, actual_y)){
                                return hashOptions.get(button->data->getWord()->mensaje);
                            }
                            button = button->next;
                        }
                    }

                    default:
                        break;
                }
            }

            window->clear();
            nl::node<Button *> *button = buttons.begin();
            for (int i=0; i<buttons.SIZE(); ++i) {
                button->data->draw(window);
                button = button->next;
            }
            window->display();
        }
        return {};
    }
};

#endif //BLOCK_CHAIN_GUI_H
