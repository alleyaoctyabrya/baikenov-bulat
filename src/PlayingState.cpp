#include "PlayingState.h"
#include "Game.h"
#include "ResourceManager.h"
#include <iostream>

PlayingState::PlayingState(Game* game) 
    : GameState(game),
      boardOffsetX1(50),
      boardOffsetX2(400),
      boardOffsetY(100),
      waitingForNetworkResult(false),
      lastShotX(0),
      lastShotY(0) {
}

void PlayingState::enter() {
    // Set up UI elements
    auto* font = ResourceManager::getInstance().getFont("main");
    if (!font) {
        std::cerr << "Failed to get main font" << std::endl;
        return;
    }
    
    turnText.setFont(*font);
    turnText.setCharacterSize(20);
    turnText.setFillColor(sf::Color::White);
    turnText.setPosition(20.f, 20.f);
    
    infoText.setFont(*font);
    infoText.setCharacterSize(20);
    infoText.setFillColor(sf::Color::White);
    infoText.setPosition(20.f, 50.f);
    
    resultText.setFont(*font);
    resultText.setCharacterSize(24);
    resultText.setFillColor(sf::Color::Yellow);
    resultText.setPosition(20.f, 80.f);
    
    // Set initial turn text
    GameMode mode = game->getGameMode();
    int currentPlayer = game->getCurrentPlayerIndex();
    
    if (mode == GameMode::Single) {
        turnText.setString(L"Ваш ход");
    } else if (mode == GameMode::Hotseat) {
        turnText.setString(L"Ход игрока " + std::to_wstring(currentPlayer + 1));
    } else {
        // Network game
        if ((mode == GameMode::NetworkHost && currentPlayer == 0) ||
            (mode == GameMode::NetworkClient && currentPlayer == 1)) {
            turnText.setString(L"Ваш ход");
        } else {
            turnText.setString(L"Ход противника");
        }
    }
    
    infoText.setString(L"");
    resultText.setString(L"");
    
    // Reset network state
    waitingForNetworkResult = false;
}

void PlayingState::exit() {
    // Nothing to clean up
}

void PlayingState::update() {
    GameMode mode = game->getGameMode();
    int currentPlayer = game->getCurrentPlayerIndex();
    
    // Single player AI update
    if (mode == GameMode::Single && currentPlayer == 1) {
        updateAI();
    }
    
    // Network game updates
    if (mode == GameMode::NetworkHost || mode == GameMode::NetworkClient) {
        updateNetworkReceiveShot();
        
        if (waitingForNetworkResult) {
            updateNetworkReceiveResult();
        }
    }
    
    // Check for game over
    checkGameOver();
}

void PlayingState::updateAI() {
    // Make AI move
    AI& ai = game->getAI();
    ai.makeMove(game->getPlayer1().getBoard());
    
    // Get the result and update UI
    ShotResult aiRes = ai.getLastResult();
    
    // Play appropriate sound
    auto& rm = ResourceManager::getInstance();
    
    if (aiRes == ShotResult::Miss) {
        auto sound = rm.createSound("shot");
        if (sound) sound->play();
        resultText.setString(L"Противник промахнулся");
        game->switchPlayer();
    } else if (aiRes == ShotResult::Hit) {
        auto sound = rm.createSound("hit");
        if (sound) sound->play();
        resultText.setString(L"Противник попал!");
    } else if (aiRes == ShotResult::Sunk) {
        auto sound = rm.createSound("hit");
        if (sound) sound->play();
        resultText.setString(L"Ваш корабль потоплен!");
    } else if (aiRes == ShotResult::AllSunk) {
        auto sound = rm.createSound("hit");
        if (sound) sound->play();
        game->endGame(L"Вы проиграли!");
    }
}

void PlayingState::updateNetworkReceiveShot() {
    int sx, sy;
    if (game->getNetwork().receiveShot(sx, sy) && !(sx == -1 && sy == -1)) {
        processNetworkShot(sx, sy);
    }
}

void PlayingState::processNetworkShot(int x, int y) {
    // Get the opponent's board reference
    Board& myBoard = game->getCurrentPlayer()->getBoard();
    
    // Process the shot
    ShotResult result = myBoard.receiveShot(x, y);
    
    // Send result back to the opponent
    game->getNetwork().sendResult(static_cast<int>(result));
    
    // Play appropriate sound
    auto& rm = ResourceManager::getInstance();
    
    if (result == ShotResult::Miss) {
        auto sound = rm.createSound("shot");
        if (sound) sound->play();
        resultText.setString(L"Противник промахнулся");
        game->switchPlayer();
    } else if (result == ShotResult::Hit) {
        auto sound = rm.createSound("hit");
        if (sound) sound->play();
        resultText.setString(L"Противник попал!");
    } else if (result == ShotResult::Sunk) {
        auto sound = rm.createSound("hit");
        if (sound) sound->play();
        resultText.setString(L"Ваш корабль потоплен!");
    } else if (result == ShotResult::AllSunk) {
        auto sound = rm.createSound("hit");
        if (sound) sound->play();
        game->endGame(L"Вы проиграли!");
    }
    
    // Update turn text
    GameMode mode = game->getGameMode();
    int currentPlayer = game->getCurrentPlayerIndex();
    
    if ((mode == GameMode::NetworkHost && currentPlayer == 0) ||
        (mode == GameMode::NetworkClient && currentPlayer == 1)) {
        turnText.setString(L"Ваш ход");
    } else {
        turnText.setString(L"Ход противника");
    }
}

void PlayingState::updateNetworkReceiveResult() {
    int resultCode;
    if (game->getNetwork().receiveResult(resultCode)) {
        processNetworkShotResult(resultCode);
        waitingForNetworkResult = false;
    }
}

void PlayingState::processNetworkShotResult(int resultCode) {
    ShotResult result = static_cast<ShotResult>(resultCode);
    
    // Play appropriate sound
    auto& rm = ResourceManager::getInstance();
    
    if (result == ShotResult::Miss) {
        auto sound = rm.createSound("shot");
        if (sound) sound->play();
        resultText.setString(L"Вы промахнулись");
        game->switchPlayer();
    } else if (result == ShotResult::Hit) {
        auto sound = rm.createSound("hit");
        if (sound) sound->play();
        resultText.setString(L"Вы попали!");
    } else if (result == ShotResult::Sunk) {
        auto sound = rm.createSound("hit");
        if (sound) sound->play();
        resultText.setString(L"Корабль противника потоплен!");
    } else if (result == ShotResult::AllSunk) {
        auto sound = rm.createSound("win");
        if (sound) sound->play();
        game->endGame(L"Вы победили!");
    }
    
    // Update turn text
    GameMode mode = game->getGameMode();
    int currentPlayer = game->getCurrentPlayerIndex();
    
    if ((mode == GameMode::NetworkHost && currentPlayer == 0) ||
        (mode == GameMode::NetworkClient && currentPlayer == 1)) {
        turnText.setString(L"Ваш ход");
    } else {
        turnText.setString(L"Ход противника");
    }
}

void PlayingState::render(sf::RenderWindow& window) {
    // Determine ship visibility
    bool showShipsP1 = true;
    bool showShipsP2 = false;
    
    GameMode mode = game->getGameMode();
    int currentPlayer = game->getCurrentPlayerIndex();
    
    if (mode == GameMode::Hotseat) {
        // In hotseat mode, show only the current player's ships
        showShipsP1 = (currentPlayer == 0);
        showShipsP2 = (currentPlayer == 1);
    } else if (mode == GameMode::Single) {
        // In single player, always show player 1's ships but not AI's
        showShipsP1 = true;
        showShipsP2 = false;
    } else {
        // In network mode
        if (mode == GameMode::NetworkHost) {
            showShipsP1 = true;  // Show host's ships
            showShipsP2 = false; // Don't show client's ships
        } else {
            showShipsP1 = true;  // Show client's ships
            showShipsP2 = false; // Don't show host's ships
        }
    }
    
    // Draw boards and UI
    renderBoards(window, showShipsP1, showShipsP2);
    renderLabels(window);
    renderGameInfo(window);
}

void PlayingState::renderBoards(sf::RenderWindow& window, bool showShipsP1, bool showShipsP2) {
    // Get textures
    auto* texShip = ResourceManager::getInstance().getTexture("ship");
    auto* texHit = ResourceManager::getInstance().getTexture("hit");
    auto* texMiss = ResourceManager::getInstance().getTexture("miss");
    
    // Draw player 1's board
    game->getPlayer1().getBoard().draw(
        window, boardOffsetX1, boardOffsetY, showShipsP1, texShip, texHit, texMiss);
    
    // Draw player 2's board (AI or human)
    if (game->getGameMode() == GameMode::Single) {
        game->getAI().getBoard().draw(
            window, boardOffsetX2, boardOffsetY, showShipsP2, texShip, texHit, texMiss);
    } else {
        game->getPlayer2().getBoard().draw(
            window, boardOffsetX2, boardOffsetY, showShipsP2, texShip, texHit, texMiss);
    }
}

void PlayingState::renderLabels(sf::RenderWindow& window) {
    auto* font = ResourceManager::getInstance().getFont("main");
    if (!font) return;
    
    // Create and draw player name labels
    sf::Text p1Label;
    sf::Text p2Label;
    
    p1Label.setFont(*font);
    p1Label.setCharacterSize(16);
    p1Label.setFillColor(sf::Color::White);
    p1Label.setString(game->getPlayer1().getName());
    p1Label.setPosition(boardOffsetX1, boardOffsetY - 24);
    
    if (game->getGameMode() == GameMode::Single) {
        p2Label.setFont(*font);
        p2Label.setCharacterSize(16);
        p2Label.setFillColor(sf::Color::White);
        p2Label.setString(game->getAI().getName());
        p2Label.setPosition(boardOffsetX2, boardOffsetY - 24);
    } else {
        p2Label.setFont(*font);
        p2Label.setCharacterSize(16);
        p2Label.setFillColor(sf::Color::White);
        p2Label.setString(game->getPlayer2().getName());
        p2Label.setPosition(boardOffsetX2, boardOffsetY - 24);
    }
    
    window.draw(p1Label);
    window.draw(p2Label);
}

void PlayingState::renderGameInfo(sf::RenderWindow& window) {
    window.draw(turnText);
    window.draw(infoText);
    window.draw(resultText);
}

void PlayingState::handleMousePressed(int x, int y) {
    GameMode mode = game->getGameMode();
    
    if (mode == GameMode::NetworkHost || mode == GameMode::NetworkClient) {
        handleNetworkGameClick(x, y);
    } else {
        handleLocalGameClick(x, y);
    }
}

void PlayingState::handleLocalGameClick(int x, int y) {
    int currentPlayer = game->getCurrentPlayerIndex();
    Board& opponentBoard = (currentPlayer == 0) ? 
        (game->getGameMode() == GameMode::Single ? game->getAI().getBoard() : game->getPlayer2().getBoard()) :
        game->getPlayer1().getBoard();
    
    // Determine which board to interact with (always the opponent's board)
    int offsetX = (currentPlayer == 0) ? boardOffsetX2 : boardOffsetX1;
    
    // Check if click is within opponent's board
    if (x >= offsetX && x < offsetX + BOARD_SIZE * CELL_SIZE &&
        y >= boardOffsetY && y < boardOffsetY + BOARD_SIZE * CELL_SIZE) {
        
        // Convert to grid coordinates
        int cellX = (x - offsetX) / CELL_SIZE;
        int cellY = (y - boardOffsetY) / CELL_SIZE;
        
        // Check if cell was already shot at
        if (opponentBoard.wasShot(cellX, cellY)) {
            return;
        }
        
        // Process the shot
        ShotResult result = opponentBoard.receiveShot(cellX, cellY);
        
        // Play appropriate sound
        auto& rm = ResourceManager::getInstance();
        
        if (result == ShotResult::Miss) {
            auto sound = rm.createSound("shot");
            if (sound) sound->play();
            resultText.setString(L"Промах!");
            game->switchPlayer();
            
            // Update turn text
            if (game->getGameMode() == GameMode::Hotseat) {
                turnText.setString(L"Ход игрока " + std::to_wstring(game->getCurrentPlayerIndex() + 1));
            } else {
                turnText.setString(L"Ход противника");
            }
        } else if (result == ShotResult::Hit) {
            auto sound = rm.createSound("hit");
            if (sound) sound->play();
            resultText.setString(L"Попадание!");
        } else if (result == ShotResult::Sunk) {
            auto sound = rm.createSound("hit");
            if (sound) sound->play();
            resultText.setString(L"Корабль противника потоплен!");
        } else if (result == ShotResult::AllSunk) {
            auto sound = rm.createSound("win");
            if (sound) sound->play();
            
            if (game->getGameMode() == GameMode::Hotseat) {
                game->endGame(L"Игрок " + std::to_wstring(currentPlayer + 1) + L" победил!");
            } else {
                game->endGame(L"Вы победили!");
            }
        }
    }
}

void PlayingState::handleNetworkGameClick(int x, int y) {
    GameMode mode = game->getGameMode();
    int currentPlayer = game->getCurrentPlayerIndex();
    
    // Check if it's our turn
    bool isMyTurn = (mode == GameMode::NetworkHost && currentPlayer == 0) ||
                   (mode == GameMode::NetworkClient && currentPlayer == 1);
    
    if (!isMyTurn || waitingForNetworkResult) {
        return;
    }
    
    // Check if click is within opponent's board
    if (x >= boardOffsetX2 && x < boardOffsetX2 + BOARD_SIZE * CELL_SIZE &&
        y >= boardOffsetY && y < boardOffsetY + BOARD_SIZE * CELL_SIZE) {
        
        // Convert to grid coordinates
        int cellX = (x - boardOffsetX2) / CELL_SIZE;
        int cellY = (y - boardOffsetY) / CELL_SIZE;
        
        // Only allow shooting at cells that haven't been shot at
        Board& opponentBoard = (currentPlayer == 0) ? 
            game->getPlayer2().getBoard() : game->getPlayer1().getBoard();
        
        if (!opponentBoard.wasShot(cellX, cellY)) {
            // Send shot to opponent
            game->getNetwork().sendShot(cellX, cellY);
            lastShotX = cellX;
            lastShotY = cellY;
            waitingForNetworkResult = true;
        }
    }
}

void PlayingState::handleKeyPressed(sf::Keyboard::Key key) {
    // No special key handling in playing state
}

void PlayingState::checkGameOver() {
    // Game over is handled in shot result processing
}