#include "Game.h"
#include "GameState.h"
#include "MainMenuState.h"
#include "PlacingShipsState.h"
#include "ConnectingState.h"
#include "PlayingState.h"
#include "GameOverState.h"
#include <iostream>

Game::Game()
    : window(sf::VideoMode(800, 600), sf::String(L"Морской бой"),
             sf::Style::Titlebar | sf::Style::Close),
      currentStateType(GameStateType::MainMenu),
      mode(GameMode::Single),
      currentPlayerIndex(0),
      opponentReady(false) {
    
    // Initialize player pointers
    players[0] = &player1;
    players[1] = &player2;
    
    // Set default names
    player1.setName("Player 1");
    player2.setName("Player 2");
    playerAI.setName("Computer");
    
    // Load resources
    auto& resourceManager = ResourceManager::getInstance();
    resourceManager.loadTexture("ship", "res/textures/ship.png");
    resourceManager.loadTexture("hit", "res/textures/hit.png");
    resourceManager.loadTexture("miss", "res/textures/miss.png");
    resourceManager.loadFont("main", "res/fonts/arial.ttf");
    resourceManager.loadSoundBuffer("shot", "res/sounds/shot.wav");
    resourceManager.loadSoundBuffer("hit", "res/sounds/hit.wav");
    resourceManager.loadSoundBuffer("win", "res/sounds/win.wav");
    
    // Initialize game states
    initStates();
    
    // Set initial state
    currentState = mainMenuState.get();
    currentState->enter();
}

Game::~Game() {
    network.disconnect();
}

void Game::initStates() {
    mainMenuState = std::make_unique<MainMenuState>(this);
    placingState = std::make_unique<PlacingShipsState>(this);
    connectingState = std::make_unique<ConnectingState>(this);
    playingState = std::make_unique<PlayingState>(this);
    gameOverState = std::make_unique<GameOverState>(this);
}

void Game::run() {
    while (window.isOpen()) {
        processEvents();
        
        // Update current state
        currentState->update();
        
        // Render
        window.clear(sf::Color(20, 20, 50));
        currentState->render(window);
        window.display();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        } else if (event.type == sf::Event::MouseButtonPressed) {
            int mx = event.mouseButton.x;
            int my = event.mouseButton.y;
            currentState->handleMousePressed(mx, my);
        } else if (event.type == sf::Event::KeyPressed) {
            currentState->handleKeyPressed(event.key.code);
        }
    }
}

void Game::changeState(GameStateType newState) {
    // Exit the current state
    if (currentState) {
        currentState->exit();
    }
    
    // Set the new state
    currentStateType = newState;
    
    // Transition to the new state
    switch (newState) {
        case GameStateType::MainMenu:
            currentState = mainMenuState.get();
            break;
        case GameStateType::Placing:
            currentState = placingState.get();
            break;
        case GameStateType::Connecting:
            currentState = connectingState.get();
            break;
        case GameStateType::Playing:
            currentState = playingState.get();
            break;
        case GameStateType::GameOver:
            currentState = gameOverState.get();
            break;
    }
    
    // Enter the new state
    if (currentState) {
        currentState->enter();
    }
}

GameStateType Game::getCurrentStateType() const {
    return currentStateType;
}

GameMode Game::getGameMode() const {
    return mode;
}

void Game::setGameMode(GameMode newMode) {
    mode = newMode;
}

void Game::startGame(GameMode newMode) {
    mode = newMode;
    
    // Reset player boards
    player1.getBoard().reset(false);
    player2.getBoard().reset(false);
    playerAI.getBoard().reset(true);
    playerAI.resetAI();
    
    // Set player names based on mode
    if (mode == GameMode::Single) {
        player1.setName("Player");
        playerAI.setName("Computer");
        players[0] = &player1;
        players[1] = &playerAI;
    } else if (mode == GameMode::Hotseat) {
        player1.setName("Player 1");
        player2.setName("Player 2");
        players[0] = &player1;
        players[1] = &player2;
    } else if (mode == GameMode::NetworkHost) {
        player1.setName("Player (Host)");
        player2.setName("Opponent");
        players[0] = &player1;
        players[1] = &player2;
    } else if (mode == GameMode::NetworkClient) {
        player1.setName("Player (Client)");
        player2.setName("Opponent");
        players[0] = &player1;
        players[1] = &player2;
    }
    
    // Reset player index
    currentPlayerIndex = (mode == GameMode::NetworkClient) ? 1 : 0;
    
    // Reset network state
    opponentReady = false;
    
    // Start the appropriate state
    if (mode == GameMode::NetworkHost) {
        if (network.startServer(5000)) {
            changeState(GameStateType::Connecting);
        } else {
            std::cerr << "Failed to start server" << std::endl;
            changeState(GameStateType::MainMenu);
        }
    } else {
        changeState(GameStateType::Placing);
    }
}

void Game::endGame(const std::wstring& resultMessage) {
    dynamic_cast<GameOverState*>(gameOverState.get())->setResultMessage(resultMessage);
    changeState(GameStateType::GameOver);
}

Player& Game::getPlayer1() {
    return player1;
}

Player& Game::getPlayer2() {
    return player2;
}

AI& Game::getAI() {
    return playerAI;
}

Player* Game::getCurrentPlayer() {
    return players[currentPlayerIndex];
}

void Game::switchPlayer() {
    currentPlayerIndex = 1 - currentPlayerIndex;
}

int Game::getCurrentPlayerIndex() const {
    return currentPlayerIndex;
}

Network& Game::getNetwork() {
    return network;
}

bool Game::isOpponentReady() const {
    return opponentReady;
}

void Game::setOpponentReady(bool ready) {
    opponentReady = ready;
}

sf::RenderWindow& Game::getWindow() {
    return window;
}
