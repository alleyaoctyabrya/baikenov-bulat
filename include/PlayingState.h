#ifndef PLAYINGSTATE_H
#define PLAYINGSTATE_H

#include "GameState.h"
#include <SFML/Graphics.hpp>

class PlayingState : public GameState {
public:
    PlayingState(Game* game);
    ~PlayingState() override = default;

    // Core state functions
    void enter() override;
    void exit() override;
    void update() override;
    void render(sf::RenderWindow& window) override;
    
    // Event handlers
    void handleMousePressed(int x, int y) override;
    void handleKeyPressed(sf::Keyboard::Key key) override;

private:
    sf::Text turnText;
    sf::Text infoText;
    sf::Text resultText;
    
    int boardOffsetX1;
    int boardOffsetX2;
    int boardOffsetY;
    
    bool waitingForNetworkResult;
    int lastShotX, lastShotY;
    
    void handleGameClick(int x, int y);
    void handleLocalGameClick(int x, int y);
    void handleNetworkGameClick(int x, int y);
    
    void updateAI();
    void updateNetworkReceiveShot();
    void updateNetworkReceiveResult();
    
    void processNetworkShot(int x, int y);
    void processNetworkShotResult(int resultCode);
    
    void renderBoards(sf::RenderWindow& window, bool showShipsP1, bool showShipsP2);
    void renderLabels(sf::RenderWindow& window);
    void renderGameInfo(sf::RenderWindow& window);
    
    void checkGameOver();
};

#endif // PLAYINGSTATE_H