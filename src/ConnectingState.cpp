#include "ConnectingState.h"
#include "Game.h"
#include "ResourceManager.h"
#include <iostream>

ConnectingState::ConnectingState(Game* game) 
    : GameState(game) {
}

void ConnectingState::enter() {
    // Set up UI elements
    auto* font = ResourceManager::getInstance().getFont("main");
    if (!font) {
        std::cerr << "Failed to get main font" << std::endl;
        return;
    }
    
    statusText.setFont(*font);
    statusText.setCharacterSize(28);
    statusText.setFillColor(sf::Color::White);
    statusText.setString(L"Ожидание подключения...");
    
    // Center the text
    sf::FloatRect bounds = statusText.getLocalBounds();
    statusText.setPosition(
        game->getWindow().getSize().x / 2 - bounds.width / 2,
        game->getWindow().getSize().y / 2 - bounds.height / 2 - 40
    );
    
    instructionText.setFont(*font);
    instructionText.setCharacterSize(20);
    instructionText.setFillColor(sf::Color::Yellow);
    instructionText.setString(L"Нажмите ESC для отмены");
    
    // Center the instruction text
    bounds = instructionText.getLocalBounds();
    instructionText.setPosition(
        game->getWindow().getSize().x / 2 - bounds.width / 2,
        game->getWindow().getSize().y / 2 - bounds.height / 2 + 40
    );
    
    // Reset the connection timer
    connectionTimer.restart();
}

void ConnectingState::exit() {
    // Nothing to clean up
}

void ConnectingState::update() {
    checkNetworkConnection();
    
    // Animate dots for waiting effect
    if (connectionTimer.getElapsedTime().asSeconds() > 3.0f) {
        connectionTimer.restart();
    }
    
    float time = connectionTimer.getElapsedTime().asSeconds();
    int dotCount = static_cast<int>(time * 2) + 1;
    
    std::wstring dots = L"";
    for (int i = 0; i < dotCount && i < 3; ++i) {
        dots += L".";
    }
    
    statusText.setString(L"Ожидание подключения" + dots);
    
    // Center the text again after string change
    sf::FloatRect bounds = statusText.getLocalBounds();
    statusText.setPosition(
        game->getWindow().getSize().x / 2 - bounds.width / 2,
        game->getWindow().getSize().y / 2 - bounds.height / 2 - 40
    );
}

void ConnectingState::render(sf::RenderWindow& window) {
    // Draw UI text
    window.draw(statusText);
    window.draw(instructionText);
}

void ConnectingState::handleMousePressed(int x, int y) {
    // No action on mouse press
}

void ConnectingState::handleKeyPressed(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Escape) {
        // Cancel connection and return to main menu
        game->getNetwork().disconnect();
        game->changeState(GameStateType::MainMenu);
    }
}

void ConnectingState::checkNetworkConnection() {
    // Check if a client has connected
    if (game->getNetwork().acceptClient()) {
        // Connection successful, move to placing state
        game->changeState(GameStateType::Placing);
    }
}