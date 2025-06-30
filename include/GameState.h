#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <SFML/Graphics.hpp>
#include <memory>

class Game;

class GameState {
public:
    GameState(Game* game) : game(game) {}
    virtual ~GameState() = default;

    // Core state functions
    virtual void enter() = 0;
    virtual void exit() = 0;
    virtual void update() = 0;
    virtual void render(sf::RenderWindow& window) = 0;
    
    // Event handlers
    virtual void handleMousePressed(int x, int y) = 0;
    virtual void handleKeyPressed(sf::Keyboard::Key key) = 0;

protected:
    Game* game; // Reference to the main game object
};

// Forward declarations of all state classes
class MainMenuState;
class PlacingShipsState;
class ConnectingState;
class PlayingState;
class GameOverState;

#endif // GAMESTATE_H