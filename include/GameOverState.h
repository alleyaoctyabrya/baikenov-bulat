#ifndef GAMEOVERSTATE_H
#define GAMEOVERSTATE_H

#include "GameState.h"
#include <SFML/Graphics.hpp>

class GameOverState : public GameState {
public:
    GameOverState(Game* game);
    ~GameOverState() override = default;

    // Core state functions
    void enter() override;
    void exit() override;
    void update() override;
    void render(sf::RenderWindow& window) override;
    
    // Event handlers
    void handleMousePressed(int x, int y) override;
    void handleKeyPressed(sf::Keyboard::Key key) override;

    // Set result message
    void setResultMessage(const std::wstring& message);

private:
    sf::Text resultText;
    sf::Text instructionText;
    
    // Game result info
    std::wstring resultMessage;
    float animationTime;
};

#endif // GAMEOVERSTATE_H