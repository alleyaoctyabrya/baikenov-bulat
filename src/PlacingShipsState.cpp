#include "PlacingShipsState.h"
#include "Game.h"
#include "ResourceManager.h"
#include <iostream>

// Ship sizes defined as a constant array for easier access
static const int SHIP_SIZES[] = {5, 4, 3, 3, 2};
static const int TOTAL_SHIPS = sizeof(SHIP_SIZES) / sizeof(SHIP_SIZES[0]);

PlacingShipsState::PlacingShipsState(Game* game) 
    : GameState(game),
      shipPlaceIndex(0),
      placeOrientation(Orientation::Horizontal),
      boardOffsetX1(50),
      boardOffsetX2(400),
      boardOffsetY(100) {
}

void PlacingShipsState::enter() {
    // Reset ship placement state
    shipPlaceIndex = 0;
    placeOrientation = Orientation::Horizontal;
    
    // Set up UI elements
    auto* font = ResourceManager::getInstance().getFont("main");
    if (!font) {
        std::cerr << "Failed to get main font" << std::endl;
        return;
    }
    
    instructionText.setFont(*font);
    instructionText.setCharacterSize(20);
    instructionText.setFillColor(sf::Color::White);
    instructionText.setString(L"R - поворот, Backspace - отмена, A - авто");
    instructionText.setPosition(20.f, 50.f);
    
    playerText.setFont(*font);
    playerText.setCharacterSize(20);
    playerText.setFillColor(sf::Color::White);
    
    // Set the player text based on game mode
    GameMode mode = game->getGameMode();
    if (mode == GameMode::Hotseat) {
        if (game->getCurrentPlayerIndex() == 0) {
            playerText.setString(L"Расставляет игрок 1");
        } else {
            playerText.setString(L"Расставляет игрок 2");
        }
    } else {
        playerText.setString(L"Расставьте корабли");
    }
    playerText.setPosition(20.f, 20.f);
}

void PlacingShipsState::exit() {
    // Nothing to clean up
}

void PlacingShipsState::update() {
    GameMode mode = game->getGameMode();
    
    // Handle network-specific updates
    if ((mode == GameMode::NetworkHost || mode == GameMode::NetworkClient)) {
        int sx, sy;
        if (game->getNetwork().receiveShot(sx, sy)) {
            if (sx == -1 && sy == -1) {
                if (shipPlaceIndex >= TOTAL_SHIPS) {
                    game->changeState(GameStateType::Playing);
                } else {
                    game->setOpponentReady(true);
                }
            }
        }
    }
}

void PlacingShipsState::render(sf::RenderWindow& window) {
    // Draw the player's board
    Board& board = getCurrentPlayerBoard();
    
    // Determine the offsets based on current player
    int offsetX = (game->getCurrentPlayerIndex() == 0 || 
                  game->getGameMode() != GameMode::Hotseat) ? 
                  boardOffsetX1 : boardOffsetX2;
    
    // Get textures
    auto* texShip = ResourceManager::getInstance().getTexture("ship");
    auto* texHit = ResourceManager::getInstance().getTexture("hit");
    auto* texMiss = ResourceManager::getInstance().getTexture("miss");
    
    // Draw the board
    board.draw(window, offsetX, boardOffsetY, true, texShip, texHit, texMiss);
    
    // Draw placement preview
    renderPlacementPreview(window);
    
    // Draw UI text
    window.draw(playerText);
    window.draw(instructionText);
}

void PlacingShipsState::handleMousePressed(int x, int y) {
    GameMode mode = game->getGameMode();
    
    if (mode == GameMode::Hotseat) {
        handleHotseatPlacingClick(x, y);
    } else {
        handleSingleNetworkPlacingClick(x, y);
    }
}

void PlacingShipsState::handleHotseatPlacingClick(int x, int y) {
    int currentPlayer = game->getCurrentPlayerIndex();
    int offsetX = (currentPlayer == 0) ? boardOffsetX1 : boardOffsetX2;
    
    if (x >= offsetX && x < offsetX + BOARD_SIZE * CELL_SIZE &&
        y >= boardOffsetY && y < boardOffsetY + BOARD_SIZE * CELL_SIZE) {
        
        int cellX = (x - offsetX) / CELL_SIZE;
        int cellY = (y - boardOffsetY) / CELL_SIZE;
        Board& board = getCurrentPlayerBoard();
        
        if (isValidPlacement(cellX, cellY, SHIP_SIZES[shipPlaceIndex], placeOrientation, board)) {
            // Create and place the ship
            Ship newShip(SHIP_SIZES[shipPlaceIndex], placeOrientation);
            for (int i = 0; i < SHIP_SIZES[shipPlaceIndex]; ++i) {
                int cx = cellX + (placeOrientation == Orientation::Horizontal ? i : 0);
                int cy = cellY + (placeOrientation == Orientation::Horizontal ? 0 : i);
                newShip.addCoord(cx, cy);
                board.shipGrid[cy][cx] = board.ships.size();
            }
            board.ships.push_back(newShip);
            
            // Move to the next ship
            shipPlaceIndex++;
            
            // Check if we're done placing ships
            if (shipPlaceIndex >= TOTAL_SHIPS) {
                if (currentPlayer == 0) {
                    // Switch to player 2
                    game->switchPlayer();
                    shipPlaceIndex = 0;
                    placeOrientation = Orientation::Horizontal;
                    playerText.setString(L"Расставляет игрок 2");
                } else {
                    // Done with placement, start the game
                    game->changeState(GameStateType::Playing);
                }
            }
        }
    }
}

void PlacingShipsState::handleSingleNetworkPlacingClick(int x, int y) {
    if (x >= boardOffsetX1 && x < boardOffsetX1 + BOARD_SIZE * CELL_SIZE &&
        y >= boardOffsetY && y < boardOffsetY + BOARD_SIZE * CELL_SIZE) {
        
        int cellX = (x - boardOffsetX1) / CELL_SIZE;
        int cellY = (y - boardOffsetY) / CELL_SIZE;
        Board& board = game->getPlayer1().getBoard();
        
        if (isValidPlacement(cellX, cellY, SHIP_SIZES[shipPlaceIndex], placeOrientation, board)) {
            // Create and place the ship
            Ship newShip(SHIP_SIZES[shipPlaceIndex], placeOrientation);
            for (int i = 0; i < SHIP_SIZES[shipPlaceIndex]; ++i) {
                int cx = cellX + (placeOrientation == Orientation::Horizontal ? i : 0);
                int cy = cellY + (placeOrientation == Orientation::Horizontal ? 0 : i);
                newShip.addCoord(cx, cy);
                board.shipGrid[cy][cx] = board.ships.size();
            }
            board.ships.push_back(newShip);
            
            // Move to the next ship
            shipPlaceIndex++;
            
            // Check if we're done placing ships
            if (shipPlaceIndex >= TOTAL_SHIPS) {
                GameMode mode = game->getGameMode();
                
                if (mode == GameMode::Single) {
                    // Start the game immediately in single player mode
                    game->changeState(GameStateType::Playing);
                } else {
                    // In network mode, notify opponent we're ready
                    game->getNetwork().sendShot(-1, -1);
                    
                    // If opponent is already ready, start the game
                    if (game->isOpponentReady()) {
                        game->changeState(GameStateType::Playing);
                    }
                }
            }
        }
    }
}

void PlacingShipsState::handleKeyPressed(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::R) {
        // Toggle orientation
        handleRotateKey();
    } else if (key == sf::Keyboard::Backspace) {
        // Reset current player's board
        handleResetKey();
    } else if (key == sf::Keyboard::A) {
        // Auto-place ships
        handleAutoPlaceKey();
    }
}

void PlacingShipsState::handleRotateKey() {
    placeOrientation = (placeOrientation == Orientation::Horizontal) ? 
                        Orientation::Vertical : Orientation::Horizontal;
}

void PlacingShipsState::handleResetKey() {
    Board& board = getCurrentPlayerBoard();
    board.reset(false);
    shipPlaceIndex = 0;
}

void PlacingShipsState::handleAutoPlaceKey() {
    Board& board = getCurrentPlayerBoard();
    board.reset(true); // Reset with random ships
    shipPlaceIndex = TOTAL_SHIPS;
    
    GameMode mode = game->getGameMode();
    
    if (mode == GameMode::Hotseat) {
        int currentPlayer = game->getCurrentPlayerIndex();
        
        if (currentPlayer == 0) {
            // Switch to player 2
            game->switchPlayer();
            shipPlaceIndex = 0;
            placeOrientation = Orientation::Horizontal;
            playerText.setString(L"Расставляет игрок 2");
        } else {
            // Done with placement, start the game
            game->changeState(GameStateType::Playing);
        }
    } else if (mode == GameMode::Single) {
        // Start the game immediately in single player mode
        game->changeState(GameStateType::Playing);
    } else {
        // In network mode, notify opponent we're ready
        game->getNetwork().sendShot(-1, -1);
        
        // If opponent is already ready, start the game
        if (game->isOpponentReady()) {
            game->changeState(GameStateType::Playing);
        }
    }
}

void PlacingShipsState::renderPlacementPreview(sf::RenderWindow& window) {
    if (shipPlaceIndex >= TOTAL_SHIPS) {
        return;
    }
    
    // Get mouse position
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    
    // Determine which board to show preview on
    int offsetX = (game->getCurrentPlayerIndex() == 0 || 
                  game->getGameMode() != GameMode::Hotseat) ? 
                  boardOffsetX1 : boardOffsetX2;
    
    // Convert to grid coordinates
    int gridX = (mousePos.x - offsetX) / CELL_SIZE;
    int gridY = (mousePos.y - boardOffsetY) / CELL_SIZE;
    
    // Check if mouse is over the board
    if (gridX >= 0 && gridX < BOARD_SIZE && gridY >= 0 && gridY < BOARD_SIZE) {
        // Get ship size
        int size = SHIP_SIZES[shipPlaceIndex];
        
        // Check if placement is valid
        bool valid = isValidPlacement(gridX, gridY, size, placeOrientation, getCurrentPlayerBoard());
        
        // Draw preview
        sf::Color color = valid ? sf::Color(100, 255, 100, 150) : sf::Color(255, 100, 100, 150);
        
        for (int i = 0; i < size; ++i) {
            int cx = gridX + (placeOrientation == Orientation::Horizontal ? i : 0);
            int cy = gridY + (placeOrientation == Orientation::Horizontal ? 0 : i);
            
            // Make sure we don't draw outside the board
            if (cx >= 0 && cx < BOARD_SIZE && cy >= 0 && cy < BOARD_SIZE) {
                sf::RectangleShape cell({(float)CELL_SIZE, (float)CELL_SIZE});
                cell.setFillColor(color);
                cell.setPosition(offsetX + cx * CELL_SIZE, boardOffsetY + cy * CELL_SIZE);
                window.draw(cell);
            }
        }
    }
}

bool PlacingShipsState::isValidPlacement(int x, int y, int size, Orientation orient, const Board& board) {
    // Check if the ship would extend off the board
    if (orient == Orientation::Horizontal) {
        if (x + size > BOARD_SIZE) {
            return false;
        }
    } else {
        if (y + size > BOARD_SIZE) {
            return false;
        }
    }
    
    // Check each cell and its surroundings
    for (int i = 0; i < size; ++i) {
        int cx = x + (orient == Orientation::Horizontal ? i : 0);
        int cy = y + (orient == Orientation::Horizontal ? 0 : i);
        
        // Check if this cell already has a ship
        if (board.shipGrid[cy][cx] != -1) {
            return false;
        }
        
        // Check surrounding cells
        for (int ny = cy - 1; ny <= cy + 1; ++ny) {
            for (int nx = cx - 1; nx <= cx + 1; ++nx) {
                if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE) {
                    if (board.shipGrid[ny][nx] != -1) {
                        return false;
                    }
                }
            }
        }
    }
    
    return true;
}

Board& PlacingShipsState::getCurrentPlayerBoard() {
    return game->getCurrentPlayer()->getBoard();
}