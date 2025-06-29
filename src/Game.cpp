
#include "Game.h"
#include <iostream>

static const int SHIP_SIZES[] = {5, 4, 3, 3, 2};
static const int TOTAL_SHIPS = sizeof(SHIP_SIZES) / sizeof(SHIP_SIZES[0]);

static bool canPlaceShip(Board& board, int x, int y, int size,
                         Orientation orient) {

  if (orient == Orientation::Horizontal) {
    if (x + size > BOARD_SIZE)
      return false;
  } else {
    if (y + size > BOARD_SIZE)
      return false;
  }
  for (int i = 0; i < size; ++i) {
    int cx = x + (orient == Orientation::Horizontal ? i : 0);
    int cy = y + (orient == Orientation::Horizontal ? 0 : i);
    if (cx < 0 || cx >= BOARD_SIZE || cy < 0 || cy >= BOARD_SIZE)
      return false;
    if (board.shipGrid[cy][cx] != -1)
      return false;
    for (int ny = cy - 1; ny <= cy + 1; ++ny) {
      for (int nx = cx - 1; nx <= cx + 1; ++nx) {
        if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE) {
          if (board.shipGrid[ny][nx] != -1)
            return false;
        }
      }
    }
  }
  return true;
}

static bool placeShipOnBoard(Board& board, int x, int y, int size,
                             Orientation orient) {

  if (!canPlaceShip(board, x, y, size, orient))
    return false;
  Ship newShip(size, orient);
  int shipIndex = static_cast<int>(board.ships.size());
  for (int i = 0; i < size; ++i) {
    int cx = x + (orient == Orientation::Horizontal ? i : 0);
    int cy = y + (orient == Orientation::Horizontal ? 0 : i);
    newShip.addCoord(cx, cy);
    board.shipGrid[cy][cx] = shipIndex;
  }
  board.ships.push_back(newShip);
  return true;
}

Game::Game()
    : window(sf::VideoMode(800, 600), sf::String(L"Морской бой"),
             sf::Style::Titlebar | sf::Style::Close) {

  state = GameState::MainMenu;
  mode = GameMode::Single;
  isGameOver = false;
  waitingForNetworkResult = false;
  opponentReady = false;
  shipPlaceIndex = 0;
  placeOrient = Orientation::Horizontal;
  currentPlayerIndex = 0;
  player2ShipsRemaining = 0;
  boardOffsetX1 = 50;
  boardOffsetY = 100;
  boardOffsetX2 = 400;

  initResources();
  initMenuUI();
  initHUD();
}

Game::~Game() { network.disconnect(); }

void Game::initResources() {

  if (!font.loadFromFile("res/fonts/arial.ttf")) {
    std::cerr << "Failed to load font\n";
  }
  if (!shotBuffer.loadFromFile("res/sounds/shot.wav")) {
    std::cerr << "Failed to load shot.wav\n";
  }
  if (!hitBuffer.loadFromFile("res/sounds/hit.wav")) {
    std::cerr << "Failed to load hit.wav\n";
  }
  if (!winBuffer.loadFromFile("res/sounds/win.wav")) {
    std::cerr << "Failed to load win.wav\n";
  }
  soundShot.setBuffer(shotBuffer);
  soundHit.setBuffer(hitBuffer);
  soundWin.setBuffer(winBuffer);

  texShip.loadFromFile("res/textures/ship.png");
  texHit.loadFromFile("res/textures/hit.png");
  texMiss.loadFromFile("res/textures/miss.png");
}

void Game::initMenuUI() {

  titleText.setFont(font);
  titleText.setCharacterSize(36);
  titleText.setFillColor(sf::Color::White);
  titleText.setString(L"Морской бой");
  sf::FloatRect tb = titleText.getLocalBounds();
  titleText.setPosition(window.getSize().x / 2 - tb.width / 2, 20.f);

  const wchar_t* options[] = {L"Одиночная игра", L"Игра вдвоем (hot-seat)",
                              L"Сетевая игра: сервер", L"Сетевая игра: клиент",
                              L"Выход"};
  menuOptionCount = 5;
  for (int i = 0; i < menuOptionCount; ++i) {
    menuOptions[i].setFont(font);
    menuOptions[i].setCharacterSize(24);
    menuOptions[i].setFillColor(sf::Color::White);
    menuOptions[i].setString(options[i]);
    float mx = window.getSize().x / 2;
    float my = 100.f + i * 40.f;
    sf::FloatRect mb = menuOptions[i].getLocalBounds();
    menuOptions[i].setPosition(mx - mb.width / 2, my);
  }
}

void Game::initHUD() {

  turnText.setFont(font);
  turnText.setCharacterSize(20);
  turnText.setFillColor(sf::Color::White);
  turnText.setPosition(20.f, 20.f);

  infoText.setFont(font);
  infoText.setCharacterSize(20);
  infoText.setFillColor(sf::Color::White);
  infoText.setPosition(20.f, 50.f);

  resultText.setFont(font);
  resultText.setCharacterSize(24);
  resultText.setFillColor(sf::Color::Yellow);
}

void Game::run() {
  while (window.isOpen()) {
    processEvents();
    update();
    render();
  }
}

void Game::startGame(GameMode newMode) {
  mode = newMode;
  isGameOver = false;
  waitingForNetworkResult = false;
  opponentReady = false;
  resultText.setString(L"");
  shipPlaceIndex = 0;
  placeOrient = Orientation::Horizontal;

  if (mode == GameMode::Single) {
    startSingleGame();
  } else if (mode == GameMode::Hotseat) {
    startHotseatGame();
  } else if (mode == GameMode::NetworkHost) {
    startNetworkHostGame();
  } else if (mode == GameMode::NetworkClient) {
    startNetworkClientGame();
  }
}

void Game::startSingleGame() {
  playerAI.getBoard().reset(true);
  playerAI.setName("Computer");
  player1.getBoard().reset(false);
  player1.setName("Player");
  players[0] = &player1;
  players[1] = &playerAI;
  currentPlayerIndex = 0;
  state = GameState::Placing;
  turnText.setString(L"Расставьте корабли");
  infoText.setString(L"R - поворот, Backspace - отмена, A - авто");
  player2ShipsRemaining = TOTAL_SHIPS;
}

void Game::startHotseatGame() {
  player1.getBoard().reset(false);
  player2.getBoard().reset(false);
  player1.setName("Player 1");
  player2.setName("Player 2");
  players[0] = &player1;
  players[1] = &player2;
  currentPlayerIndex = 0;
  state = GameState::Placing;
  turnText.setString(L"Расставляет игрок 1");
  infoText.setString(L"R - поворот, Backspace - отмена, A - авто");
  player2ShipsRemaining = TOTAL_SHIPS;
}

void Game::startNetworkHostGame() {
  player1.getBoard().reset(false);
  player2.getBoard().reset(false);
  player1.setName("Player (Host)");
  player2.setName("Opponent");
  players[0] = &player1;
  players[1] = &player2;
  currentPlayerIndex = 0;
  if (network.startServer(5000)) {
    state = GameState::Connecting;
    infoText.setString(L"Ожидание подключения...");
  } else {
    std::wcerr << L"Не удалось запустить сервер.\n";
    state = GameState::MainMenu;
    return;
  }
  player2ShipsRemaining = TOTAL_SHIPS;
}

void Game::startNetworkClientGame() {
  player1.getBoard().reset(false);
  player2.getBoard().reset(false);
  player1.setName("Player (Client)");
  player2.setName("Opponent");
  players[0] = &player1;
  players[1] = &player2;
  if (network.connectToServer(sf::IpAddress::LocalHost, 5000)) {
    currentPlayerIndex = 1;
    state = GameState::Placing;
    turnText.setString(L"Расставьте корабли");
    infoText.setString(L"R - поворот, Backspace - отмена, A - авто");
  } else {
    std::wcerr << L"Не удалось подключиться к серверу.\n";
    state = GameState::MainMenu;
    return;
  }
  player2ShipsRemaining = TOTAL_SHIPS;
}

void Game::processEvents() {
  sf::Event event;
  while (window.pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      window.close();
    } else if (event.type == sf::Event::MouseButtonPressed) {
      int mx = event.mouseButton.x;
      int my = event.mouseButton.y;
      handleMousePressed(mx, my);
    } else if (event.type == sf::Event::KeyPressed) {
      handleKeyPressed(event.key.code);
    }
  }
}

void Game::handleMousePressed(int x, int y) {
  if (state == GameState::MainMenu) {
    handleMenuClick(x, y);
  } else if (state == GameState::Placing) {
    handlePlacingClick(x, y);
  } else if (state == GameState::Playing) {
    handleGameClick(x, y);
  } else if (state == GameState::GameOver) {
    state = GameState::MainMenu;
  }
}

void Game::handlePlacingClick(int x, int y) {
  if (mode == GameMode::Hotseat) {
    handleHotseatPlacingClick(x, y);
  } else {
    handleSingleNetworkPlacingClick(x, y);
  }
}

void Game::handleHotseatPlacingClick(int x, int y) {
  if (currentPlayerIndex == 0) {
    handleHotseatPlayer0PlacementClick(x, y);
  } else {
    handleHotseatPlayer1PlacementClick(x, y);
  }
}

void Game::handleHotseatPlayer0PlacementClick(int x, int y) {
  if (x >= boardOffsetX1 && x < boardOffsetX1 + BOARD_SIZE * CELL_SIZE &&
      y >= boardOffsetY && y < boardOffsetY + BOARD_SIZE * CELL_SIZE) {
    int cellX = (x - boardOffsetX1) / CELL_SIZE;
    int cellY = (y - boardOffsetY) / CELL_SIZE;
    if (placeShipOnBoard(player1.getBoard(), cellX, cellY,
                         SHIP_SIZES[shipPlaceIndex], placeOrient)) {
      shipPlaceIndex++;
      if (shipPlaceIndex >= TOTAL_SHIPS) {
        currentPlayerIndex = 1;
        shipPlaceIndex = 0;
        placeOrient = Orientation::Horizontal;
        turnText.setString(L"Расставляет игрок 2");
      }
    }
  }
}

void Game::handleHotseatPlayer1PlacementClick(int x, int y) {
  if (x >= boardOffsetX2 && x < boardOffsetX2 + BOARD_SIZE * CELL_SIZE &&
      y >= boardOffsetY && y < boardOffsetY + BOARD_SIZE * CELL_SIZE) {
    int cellX = (x - boardOffsetX2) / CELL_SIZE;
    int cellY = (y - boardOffsetY) / CELL_SIZE;
    if (placeShipOnBoard(player2.getBoard(), cellX, cellY,
                         SHIP_SIZES[shipPlaceIndex], placeOrient)) {
      shipPlaceIndex++;
      if (shipPlaceIndex >= TOTAL_SHIPS) {
        state = GameState::Playing;
        isGameOver = false;
        currentPlayerIndex = 0;
        resultText.setString(L"");
      }
    }
  }
}

void Game::handleSingleNetworkPlacingClick(int x, int y) {
  if (x >= boardOffsetX1 && x < boardOffsetX1 + BOARD_SIZE * CELL_SIZE &&
      y >= boardOffsetY && y < boardOffsetY + BOARD_SIZE * CELL_SIZE) {
    int cellX = (x - boardOffsetX1) / CELL_SIZE;
    int cellY = (y - boardOffsetY) / CELL_SIZE;
    if (placeShipOnBoard(player1.getBoard(), cellX, cellY,
                         SHIP_SIZES[shipPlaceIndex], placeOrient)) {
      shipPlaceIndex++;
      if (shipPlaceIndex >= TOTAL_SHIPS) {
        if (mode == GameMode::Single) {
          state = GameState::Playing;
          isGameOver = false;
          currentPlayerIndex = 0;
          resultText.setString(L"");
        } else {
          network.sendShot(-1, -1);
          if (opponentReady) {
            state = GameState::Playing;
            isGameOver = false;
            resultText.setString(L"");
          }
        }
      }
    }
  }
}

void Game::handleKeyPressed(sf::Keyboard::Key key) {
  if (state != GameState::Placing)
    return;
  if (key == sf::Keyboard::R) {
    placeOrient =
        (placeOrient == Orientation::Horizontal ? Orientation::Vertical
                                                : Orientation::Horizontal);
  } else if (key == sf::Keyboard::Backspace) {
    handleKeyBackspace();
  } else if (key == sf::Keyboard::A) {
    handleKeyAuto();
  }
}

void Game::handleKeyBackspace() {
  if (mode == GameMode::Hotseat) {
    handleBackspaceHotseat();
  } else {
    handleBackspaceSingle();
  }
}

void Game::handleBackspaceHotseat() {
  if (currentPlayerIndex == 0) {
    player1.getBoard().reset(false);
    shipPlaceIndex = 0;
  } else {
    player2.getBoard().reset(false);
    shipPlaceIndex = 0;
  }
}

void Game::handleBackspaceSingle() {
  player1.getBoard().reset(false);
  shipPlaceIndex = 0;
}

void Game::handleKeyAuto() {
  if (mode == GameMode::Hotseat) {
    if (currentPlayerIndex == 0) {
      handleAutoHotseatFirst();
    } else {
      handleAutoHotseatSecond();
    }
  } else {
    handleAutoSingleNetwork();
  }
}

void Game::handleAutoHotseatFirst() {
  player1.getBoard().reset(true);
  shipPlaceIndex = TOTAL_SHIPS;
  currentPlayerIndex = 1;
  shipPlaceIndex = 0;
  placeOrient = Orientation::Horizontal;
  turnText.setString(L"Расставляет игрок 2");
}

void Game::handleAutoHotseatSecond() {
  player2.getBoard().reset(true);
  shipPlaceIndex = TOTAL_SHIPS;
  state = GameState::Playing;
  isGameOver = false;
  currentPlayerIndex = 0;
  resultText.setString(L"");
}

void Game::handleAutoSingleNetwork() {
  player1.getBoard().reset(true);
  shipPlaceIndex = TOTAL_SHIPS;
  if (mode == GameMode::Single) {
    state = GameState::Playing;
    isGameOver = false;
    currentPlayerIndex = 0;
    resultText.setString(L"");
  } else {
    network.sendShot(-1, -1);
    if (opponentReady) {
      state = GameState::Playing;
      isGameOver = false;
      resultText.setString(L"");
    }
  }
}

void Game::update() {
  if (state == GameState::Connecting && mode == GameMode::NetworkHost) {
    updateConnecting();
  }
  if (state == GameState::Placing &&
      (mode == GameMode::NetworkHost || mode == GameMode::NetworkClient)) {
    updateNetworkPlacement();
  }
  if (state == GameState::Playing) {
    if (mode == GameMode::Single && currentPlayerIndex == 1) {
      updateSingleAI();
    }
    if (mode == GameMode::NetworkHost || mode == GameMode::NetworkClient) {
      updateNetworkReceiveShot();
      if (waitingForNetworkResult) {
        updateNetworkReceiveResult();
      }
    }
  }
}

void Game::updateConnecting() {
  int sx, sy;
  if (network.acceptClient()) {
    state = GameState::Placing;
    turnText.setString(L"Расставьте корабли");
    infoText.setString(L"R - поворот, Backspace - отмена, A - авто");
    opponentReady = false;
  }
}

void Game::updateNetworkPlacement() {
  int sx, sy;
  if (network.receiveShot(sx, sy)) {
    if (sx == -1 && sy == -1) {
      if (shipPlaceIndex >= TOTAL_SHIPS) {
        state = GameState::Playing;
        isGameOver = false;
        resultText.setString(L"");
      } else {
        opponentReady = true;
      }
    }
  }
}

void Game::updateSingleAI() {
  playerAI.makeMove(player1.getBoard());
  ShotResult aiRes = playerAI.getLastResult();
  if (aiRes == ShotResult::Miss) {
    soundShot.play();
    resultText.setString(L"Противник промахнулся");
    currentPlayerIndex = 0;
  } else if (aiRes == ShotResult::Hit) {
    soundHit.play();
    resultText.setString(L"Противник попал!");
  } else if (aiRes == ShotResult::Sunk) {
    soundHit.play();
    resultText.setString(L"Ваш корабль потоплен!");
  } else if (aiRes == ShotResult::AllSunk) {
    soundHit.play();
    resultText.setString(L"Вы проиграли!");
    isGameOver = true;
    state = GameState::GameOver;
  }
}

void Game::updateNetworkReceiveShot() {
  int sx, sy;
  if (network.receiveShot(sx, sy) && !(sx == -1 && sy == -1)) {
    processNetworkShot(sx, sy);
  }
}

void Game::processNetworkShot(int sx, int sy) {
  ShotResult res = player1.getBoard().receiveShot(sx, sy);
  int code = 0;
  if (res == ShotResult::Miss) {
    code = 0;
    soundShot.play();
    resultText.setString(L"Противник мимо");
    currentPlayerIndex = 0;
  } else if (res == ShotResult::Hit) {
    code = 1;
    soundHit.play();
    resultText.setString(L"Противник попал по вам");
  } else if (res == ShotResult::Sunk) {
    code = 2;
    soundHit.play();
    resultText.setString(L"Ваш корабль потоплен!");
  } else {
    code = 3;
    soundHit.play();
    resultText.setString(L"Вы проиграли!");
    isGameOver = true;
    state = GameState::GameOver;
  }
  network.sendResult(code);
}

void Game::updateNetworkReceiveResult() {
  int rcode;
  if (network.receiveResult(rcode)) {
    waitingForNetworkResult = false;
    processNetworkShotResult(rcode);
  }
}

void Game::processNetworkShotResult(int rcode) {
  ShotResult res;
  if (rcode == 0) {
    res = ShotResult::Miss;
  } else if (rcode == 1) {
    res = ShotResult::Hit;
  } else if (rcode == 2) {
    res = ShotResult::Sunk;
  } else {
    res = ShotResult::AllSunk;
  }
  Board& enemyBoard = player2.getBoard();
  enemyBoard.hitGrid[lastShotY][lastShotX] = true;
  if (res == ShotResult::Miss) {
    soundShot.play();
    resultText.setString(L"Мимо!");
    currentPlayerIndex = 1;
  } else if (res == ShotResult::Hit) {
    enemyBoard.shipGrid[lastShotY][lastShotX] = -2;
    soundHit.play();
    resultText.setString(L"Попадание!");
    currentPlayerIndex = 0;
  } else if (res == ShotResult::Sunk) {
    player2ShipsRemaining--;
    enemyBoard.shipGrid[lastShotY][lastShotX] = -2;
    for (int ny = lastShotY - 1; ny <= lastShotY + 1; ++ny) {
      for (int nx = lastShotX - 1; nx <= lastShotX + 1; ++nx) {
        if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE) {
          enemyBoard.hitGrid[ny][nx] = true;
        }
      }
    }
    soundHit.play();
    resultText.setString(L"Корабль противника потоплен!");
    currentPlayerIndex = 0;
  } else {
    player2ShipsRemaining = 0;
    enemyBoard.shipGrid[lastShotY][lastShotX] = -2;
    for (int ny = lastShotY - 1; ny <= lastShotY + 1; ++ny) {
      for (int nx = lastShotX - 1; nx <= lastShotX + 1; ++nx) {
        if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE) {
          enemyBoard.hitGrid[ny][nx] = true;
        }
      }
    }
    soundHit.play();
    soundWin.play();
    resultText.setString(L"Победа!");
    isGameOver = true;
    state = GameState::GameOver;
  }
}

void Game::render() {
  window.clear();
  if (state == GameState::MainMenu) {
    renderMainMenu();
    return;
  }

  bool showShipsP1 = true;
  bool showShipsP2 = false;
  updateShipVisibility(showShipsP1, showShipsP2);

  renderBoards(showShipsP1, showShipsP2);
  renderLabels();

  if (state == GameState::Placing && shipPlaceIndex < TOTAL_SHIPS) {
    renderPlacementPreview();
  }
  if (state == GameState::Playing || state == GameState::GameOver) {
    renderGameInfo();
  }

  window.draw(turnText);
  window.draw(infoText);
  if (resultText.getString().getSize() > 0) {
    sf::FloatRect rb = resultText.getLocalBounds();
    resultText.setOrigin(rb.width / 2, rb.height / 2);
    resultText.setPosition(window.getSize().x / 2.0f,
                           boardOffsetY + BOARD_SIZE * CELL_SIZE + 30);
    window.draw(resultText);
  }
  window.display();
}

void Game::renderMainMenu() {
  window.draw(titleText);
  for (int i = 0; i < menuOptionCount; ++i) {
    window.draw(menuOptions[i]);
  }
  window.display();
}

void Game::renderBoards(bool showShipsP1, bool showShipsP2) {
  player1.getBoard().draw(window, boardOffsetX1, boardOffsetY, showShipsP1,
                          &texShip, &texHit, &texMiss);
  if (mode == GameMode::Single) {
    playerAI.getBoard().draw(window, boardOffsetX2, boardOffsetY, showShipsP2,
                             &texShip, &texHit, &texMiss);
  } else {
    player2.getBoard().draw(window, boardOffsetX2, boardOffsetY, showShipsP2,
                            &texShip, &texHit, &texMiss);
  }
}

void Game::renderLabels() {
  sf::Text label;
  label.setFont(font);
  label.setCharacterSize(16);
  label.setFillColor(sf::Color::White);
  std::wstring letters = L"ABCDEFGHIJ";
  for (int j = 0; j < BOARD_SIZE; ++j) {
    label.setString(letters.substr(j, 1));
    float lx1 = boardOffsetX1 + j * CELL_SIZE + CELL_SIZE / 2.0f;
    label.setPosition(lx1 - label.getLocalBounds().width / 2,
                      boardOffsetY - 20);
    window.draw(label);
    float lx2 = boardOffsetX2 + j * CELL_SIZE + CELL_SIZE / 2.0f;
    label.setPosition(lx2 - label.getLocalBounds().width / 2,
                      boardOffsetY - 20);
    window.draw(label);
  }
  for (int i = 0; i < BOARD_SIZE; ++i) {
    label.setString(std::to_wstring(i + 1));
    float ly = boardOffsetY + i * CELL_SIZE + CELL_SIZE / 2.0f;
    label.setPosition(boardOffsetX1 - 20,
                      ly - label.getLocalBounds().height / 2);
    window.draw(label);
    label.setPosition(boardOffsetX2 - 20,
                      ly - label.getLocalBounds().height / 2);
    window.draw(label);
  }
}

void Game::renderPlacementPreview() {
  int mouseX = sf::Mouse::getPosition(window).x;
  int mouseY = sf::Mouse::getPosition(window).y;
  int cellX, cellY;
  bool onBoard = false;
  int boardX = (mode == GameMode::Hotseat && currentPlayerIndex == 1)
                   ? boardOffsetX2
                   : boardOffsetX1;
  if (mouseX >= boardX && mouseX < boardX + BOARD_SIZE * CELL_SIZE &&
      mouseY >= boardOffsetY &&
      mouseY < boardOffsetY + BOARD_SIZE * CELL_SIZE) {
    cellX = (mouseX - boardX) / CELL_SIZE;
    cellY = (mouseY - boardOffsetY) / CELL_SIZE;
    onBoard = true;
  }
  if (onBoard) {
    int size = SHIP_SIZES[shipPlaceIndex];
    Orientation orient = placeOrient;
    Board& target = (mode == GameMode::Hotseat && currentPlayerIndex == 1)
                        ? player2.getBoard()
                        : player1.getBoard();
    bool valid = canPlaceShip(target, cellX, cellY, size, orient);
    sf::Color color =
        valid ? sf::Color(0, 255, 0, 100) : sf::Color(255, 0, 0, 100);
    sf::RectangleShape rect(sf::Vector2f((float)CELL_SIZE, (float)CELL_SIZE));
    rect.setFillColor(color);
    for (int i = 0; i < size; ++i) {
      int px = cellX + (orient == Orientation::Horizontal ? i : 0);
      int py = cellY + (orient == Orientation::Horizontal ? 0 : i);
      if (px < 0 || px >= BOARD_SIZE || py < 0 || py >= BOARD_SIZE)
        break;
      rect.setPosition((float)(boardX + px * CELL_SIZE),
                       (float)(boardOffsetY + py * CELL_SIZE));
      window.draw(rect);
    }
  }
}

void Game::renderGameInfo() {
  if (mode == GameMode::Hotseat) {
    turnText.setString(currentPlayerIndex == 0 ? L"Ход игрока 1"
                                               : L"Ход игрока 2");
    int p1left = player1.getBoard().getShipsRemaining();
    int p2left = player2.getBoard().getShipsRemaining();
    infoText.setString(L"Игрок 1: " + std::to_wstring(p1left) +
                       L"   Игрок 2: " + std::to_wstring(p2left));
  } else if (mode == GameMode::Single) {
    turnText.setString(currentPlayerIndex == 0 ? L"Ваш ход"
                                               : L"Ход компьютера");
    int yourLeft = player1.getBoard().getShipsRemaining();
    int aiLeft = playerAI.getBoard().getShipsRemaining();
    infoText.setString(L"Ваших кораблей: " + std::to_wstring(yourLeft) +
                       L"   Кораблей противника: " + std::to_wstring(aiLeft));
  } else {
    turnText.setString(currentPlayerIndex == 0 ? L"Ваш ход"
                                               : L"Ход противника");
    int yourLeft = player1.getBoard().getShipsRemaining();
    infoText.setString(L"Ваших кораблей: " + std::to_wstring(yourLeft) +
                       L"   Кораблей противника: " +
                       std::to_wstring(player2ShipsRemaining));
  }
}

void Game::updateShipVisibility(bool& showShipsP1, bool& showShipsP2) {

  showShipsP1 = true;
  showShipsP2 = false;
  if (state == GameState::Placing && mode == GameMode::Hotseat &&
      currentPlayerIndex == 1) {
    showShipsP1 = false;
    showShipsP2 = true;
  } else if (state == GameState::GameOver &&
             (mode == GameMode::Single || mode == GameMode::Hotseat)) {
    showShipsP1 = true;
    showShipsP2 = true;
  } else if (state == GameState::Playing && mode == GameMode::Hotseat &&
             currentPlayerIndex == 1) {
    showShipsP1 = false;
    showShipsP2 = true;
  }
}

void Game::handleMenuClick(int x, int y) {
  for (int i = 0; i < menuOptionCount; ++i) {
    if (menuOptions[i].getGlobalBounds().contains((float)x, (float)y)) {
      switch (i) {
        case 0:
          startGame(GameMode::Single);
          break;
        case 1:
          startGame(GameMode::Hotseat);
          break;
        case 2:
          startGame(GameMode::NetworkHost);
          break;
        case 3:
          startGame(GameMode::NetworkClient);
          break;
        case 4:
          window.close();
          break;
      }
      return;
    }
  }
}

void Game::handleGameClick(int x, int y) {
  if (isGameOver || (mode == GameMode::Single && currentPlayerIndex == 1))
    return;

  if (mode == GameMode::Single || mode == GameMode::Hotseat)
    handleLocalGameClick(x, y);
  else
    handleNetworkGameClick(x, y);
}

void Game::handleLocalGameClick(int x, int y) {
  int boardX = (currentPlayerIndex == 0) ? boardOffsetX2 : boardOffsetX1;
  if (x < boardX || x >= boardX + BOARD_SIZE * CELL_SIZE || y < boardOffsetY ||
      y >= boardOffsetY + BOARD_SIZE * CELL_SIZE)
    return;

  int cellX = (x - boardX) / CELL_SIZE;
  int cellY = (y - boardOffsetY) / CELL_SIZE;
  Board& enemy = players[1 - currentPlayerIndex]->getBoard();
  if (enemy.wasShot(cellX, cellY))
    return;

  ShotResult res = enemy.receiveShot(cellX, cellY);
  soundShot.play();
  if (res == ShotResult::Miss) {
    resultText.setString(L"Мимо!");
    currentPlayerIndex = 1 - currentPlayerIndex;
  } else {
    soundHit.play();
    if (res == ShotResult::Hit)
      resultText.setString(L"Попадание!");
    else if (res == ShotResult::Sunk)
      resultText.setString(L"Корабль потоплен!");
    else {
      soundWin.play();
      resultText.setString(L"Победа!");
      isGameOver = true;
      state = GameState::GameOver;
    }
  }
}

void Game::handleNetworkGameClick(int x, int y) {
  if (currentPlayerIndex != 0 || waitingForNetworkResult)
    return;

  if (x < boardOffsetX2 || x >= boardOffsetX2 + BOARD_SIZE * CELL_SIZE ||
      y < boardOffsetY || y >= boardOffsetY + BOARD_SIZE * CELL_SIZE)
    return;

  int cellX = (x - boardOffsetX2) / CELL_SIZE;
  int cellY = (y - boardOffsetY) / CELL_SIZE;
  Board& enemy = player2.getBoard();
  if (enemy.wasShot(cellX, cellY))
    return;

  lastShotX = cellX;
  lastShotY = cellY;
  if (network.sendShot(cellX, cellY)) {
    soundShot.play();
    waitingForNetworkResult = true;
  } else {
    std::cerr << "Failed to send shot.\n";
  }
}
