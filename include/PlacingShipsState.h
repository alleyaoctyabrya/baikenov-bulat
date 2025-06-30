#ifndef PLACINGSHIPSSTATE_H
#define PLACINGSHIPSSTATE_H

#include "GameState.h"
#include "Board.h"
#include "Ship.h"
#include <SFML/Graphics.hpp>

class PlacingShipsState : public GameState {
public:
    PlacingShipsState(Game* game);
    ~PlacingShipsState() override = default;

    // Core state functions
    void enter() override;
    void exit() override;
    void update() override;
    void render(sf::RenderWindow& window) override;
    
    // Event handlers
    void handleMousePressed(int x, int y) override;
    void handleKeyPressed(sf::Keyboard::Key key) override;

private:
    int shipPlaceIndex;
    Orientation placeOrientation;
    
    // Board rendering offsets
    int boardOffsetX1;
    int boardOffsetX2;
    int boardOffsetY;
    
    // UI elements
    sf::Text instructionText;
    sf::Text playerText;
    
    // Methods for ship placement
    void handlePlacingClick(int x, int y);
    void handleHotseatPlacingClick(int x, int y);
    void handleSingleNetworkPlacingClick(int x, int y);
    
    void handleRotateKey();
    void handleResetKey();
    void handleAutoPlaceKey();
    
    void renderPlacementPreview(sf::RenderWindow& window);
    bool isValidPlacement(int x, int y, int size, Orientation orient, const Board& board);
    
    // Helper method to get active player's board
    Board& getCurrentPlayerBoard();
};

#endif // PLACINGSHIPSSTATE_H