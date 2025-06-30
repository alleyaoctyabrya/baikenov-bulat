#ifndef GAME_H
#define GAME_H

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <memory>
#include "AI.h"
#include "Board.h"
#include "Network.h"
#include "Player.h"
#include "ResourceManager.h"

// Forward declare all state classes
class GameState;
class MainMenuState;
class PlacingShipsState;
class ConnectingState;
class PlayingState;
class GameOverState;

const int BOARD_SIZE = 10;

enum class GameStateType { MainMenu, Placing, Connecting, Playing, GameOver };
enum class GameMode { Single, Hotseat, NetworkHost, NetworkClient };

class Game {
public:
    Game();
    ~Game();
    void run();

    // State management
    void changeState(GameStateType newState);
    GameStateType getCurrentStateType() const;
    GameMode getGameMode() const;
    void setGameMode(GameMode mode);

    // Game control
    void startGame(GameMode mode);
    void endGame(const std::wstring& resultMessage);
    
    // Player management
    Player& getPlayer1();
    Player& getPlayer2();
    AI& getAI();
    Player* getCurrentPlayer();
    void switchPlayer();
    int getCurrentPlayerIndex() const;
    
    // Network
    Network& getNetwork();
    bool isOpponentReady() const;
    void setOpponentReady(bool ready);
    
    // Resources
    sf::RenderWindow& getWindow();

private:
    sf::RenderWindow window;
    GameStateType currentStateType;
    GameMode mode;
    
    // Players
    Player player1;
    Player player2;
    AI playerAI;
    Player* players[2];
    int currentPlayerIndex;
    
    // Network state
    bool opponentReady;
    Network network;
    
    // Game states
    std::unique_ptr<MainMenuState> mainMenuState;
    std::unique_ptr<PlacingShipsState> placingState;
    std::unique_ptr<ConnectingState> connectingState;
    std::unique_ptr<PlayingState> playingState;
    std::unique_ptr<GameOverState> gameOverState;
    
    GameState* currentState;
    
    void processEvents();
    void initStates();
};

#endif // GAME_H
