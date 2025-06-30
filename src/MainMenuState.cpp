#include "MainMenuState.h"
#include "Game.h"
#include "ResourceManager.h"
#include <iostream>

MainMenuState::MainMenuState(Game* game) : GameState(game) {
}

void MainMenuState::enter() {
    auto& window = game->getWindow();
    auto* font = ResourceManager::getInstance().getFont("main");
    if (!font) {
        std::cerr << "Failed to get main font" << std::endl;
        return;
    }
    
    initMenuUI(window, *font);
}

void MainMenuState::exit() {
    // Nothing to clean up
}

void MainMenuState::update() {
    // Main menu doesn't need to update anything
}

void MainMenuState::render(sf::RenderWindow& window) {
    // Draw title
    window.draw(titleText);
    
    // Draw menu options
    for (int i = 0; i < MENU_OPTION_COUNT; ++i) {
        window.draw(menuOptions[i]);
    }
}

void MainMenuState::handleMousePressed(int x, int y) {
    // Check if any menu option was clicked
    for (int i = 0; i < MENU_OPTION_COUNT; ++i) {
        sf::FloatRect bounds = menuOptions[i].getGlobalBounds();
        if (bounds.contains(static_cast<float>(x), static_cast<float>(y))) {
            // Handle menu selection
            switch (i) {
                case 0: // Single player
                    game->startGame(GameMode::Single);
                    break;
                    
                case 1: // Hotseat
                    game->startGame(GameMode::Hotseat);
                    break;
                    
                case 2: // Network host
                    game->startGame(GameMode::NetworkHost);
                    break;
                    
                case 3: // Network client
                    // Try to connect to server
                    if (game->getNetwork().connectToServer(sf::IpAddress::LocalHost, 5000)) {
                        game->startGame(GameMode::NetworkClient);
                    } else {
                        std::cerr << "Failed to connect to server" << std::endl;
                    }
                    break;
                    
                case 4: // Exit
                    game->getWindow().close();
                    break;
            }
        }
    }
}

void MainMenuState::handleKeyPressed(sf::Keyboard::Key key) {
    // No key handling in main menu
}

void MainMenuState::initMenuUI(sf::RenderWindow& window, sf::Font& font) {
    // Initialize title
    titleText.setFont(font);
    titleText.setCharacterSize(36);
    titleText.setFillColor(sf::Color::White);
    titleText.setString(L"Морской бой");
    sf::FloatRect tb = titleText.getLocalBounds();
    titleText.setPosition(window.getSize().x / 2 - tb.width / 2, 20.f);
    
    // Initialize menu options
    const wchar_t* options[] = {
        L"Одиночная игра", 
        L"Игра вдвоем (hot-seat)",
        L"Сетевая игра: сервер", 
        L"Сетевая игра: клиент",
        L"Выход"
    };
    
    for (int i = 0; i < MENU_OPTION_COUNT; ++i) {
        menuOptions[i].setFont(font);
        menuOptions[i].setCharacterSize(24);
        menuOptions[i].setFillColor(sf::Color::White);
        menuOptions[i].setString(options[i]);
        
        float mx = window.getSize().x / 2;
        float my = 100.f + i * 40.f;
        sf::FloatRect mb = menuOptions[i].getLocalBounds();
        menuOptions[i].setPosition(mx - mb.width / 2, my);
    }
}