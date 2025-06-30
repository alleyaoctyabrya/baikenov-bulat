#ifndef CONNECTINGSTATE_H
#define CONNECTINGSTATE_H

#include "GameState.h"
#include <SFML/Graphics.hpp>

class ConnectingState : public GameState {
public:
    ConnectingState(Game* game);
    ~ConnectingState() override = default;

    // Core state functions
    void enter() override;
    void exit() override;
    void update() override;
    void render(sf::RenderWindow& window) override;
    
    // Event handlers
    void handleMousePressed(int x, int y) override;
    void handleKeyPressed(sf::Keyboard::Key key) override;

private:
    sf::Text statusText;
    sf::Text instructionText;
    sf::Clock connectionTimer;
    
    void checkNetworkConnection();
};

#endif // CONNECTINGSTATE_H