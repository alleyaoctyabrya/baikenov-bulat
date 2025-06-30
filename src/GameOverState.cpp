#include "GameOverState.h"
#include "Game.h"
#include "ResourceManager.h"
#include <iostream>
#include <cmath>
GameOverState::GameOverState(Game* game) 
    : GameState(game),
      animationTime(0.0f) {
}

void GameOverState::enter() {
    // Set up UI elements
    auto* font = ResourceManager::getInstance().getFont("main");
    if (!font) {
        std::cerr << "Failed to get main font" << std::endl;
        return;
    }
    
    resultText.setFont(*font);
    resultText.setCharacterSize(32);
    resultText.setFillColor(sf::Color::Yellow);
    resultText.setString(resultMessage);
    
    // Center the text
    sf::FloatRect bounds = resultText.getLocalBounds();
    resultText.setPosition(
        game->getWindow().getSize().x / 2 - bounds.width / 2,
        game->getWindow().getSize().y / 2 - bounds.height / 2 - 40
    );
    
    instructionText.setFont(*font);
    instructionText.setCharacterSize(24);
    instructionText.setFillColor(sf::Color::White);
    instructionText.setString(L"Нажмите для возврата в меню");
    
    // Center the instruction text
    bounds = instructionText.getLocalBounds();
    instructionText.setPosition(
        game->getWindow().getSize().x / 2 - bounds.width / 2,
        game->getWindow().getSize().y / 2 - bounds.height / 2 + 40
    );
    
    // Play win sound
    auto sound = ResourceManager::getInstance().createSound("win");
    if (sound) {
        sound->play();
    }
    
    // Reset animation time
    animationTime = 0.0f;
}

void GameOverState::exit() {
    // Nothing to clean up
}

void GameOverState::update() {
    // Update animation time (for potential victory effects)
    animationTime += 0.016f; // Approximate for 60 FPS
    
    // Make the result text pulse
    float scale = 1.0f + 0.1f * sin(animationTime * 3.0f);
    resultText.setScale(scale, scale);
    
    // Recenter the text after scaling
    sf::FloatRect bounds = resultText.getLocalBounds();
    resultText.setPosition(
        game->getWindow().getSize().x / 2 - (bounds.width * scale) / 2,
        game->getWindow().getSize().y / 2 - (bounds.height * scale) / 2 - 40
    );
}

void GameOverState::render(sf::RenderWindow& window) {
    // Draw UI text
    window.draw(resultText);
    window.draw(instructionText);
}

void GameOverState::handleMousePressed(int x, int y) {
    // Any click returns to the main menu
    game->changeState(GameStateType::MainMenu);
}

void GameOverState::handleKeyPressed(sf::Keyboard::Key key) {
    // Any key returns to the main menu
    game->changeState(GameStateType::MainMenu);
}

void GameOverState::setResultMessage(const std::wstring& message) {
    resultMessage = message;
}