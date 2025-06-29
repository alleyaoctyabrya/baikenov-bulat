
#ifndef GAME_H
#define GAME_H

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include "AI.h"
#include "Board.h"
#include "Network.h"
#include "Player.h"

const int BOARD_SIZE = 10;

enum class GameState { MainMenu, Placing, Connecting, Playing, GameOver };
enum class GameMode { Single, Hotseat, NetworkHost, NetworkClient };

class Game {
 public:
  Game();
  ~Game();
  void run();

 private:
  sf::RenderWindow window;
  GameState state;
  GameMode mode;
  bool isGameOver;
  bool waitingForNetworkResult;
  Player player1;
  Player player2;
  AI playerAI;
  Player* players[2];
  int currentPlayerIndex;
  int player2ShipsRemaining;
  bool opponentReady;
  Network network;

  sf::Font font;
  sf::Text titleText;
  sf::Text menuOptions[5];
  int menuOptionCount;
  sf::Text turnText;
  sf::Text infoText;
  sf::Text resultText;

  sf::Texture texShip;
  sf::Texture texHit;
  sf::Texture texMiss;
  sf::SoundBuffer shotBuffer;
  sf::SoundBuffer hitBuffer;
  sf::SoundBuffer winBuffer;
  sf::Sound soundShot;
  sf::Sound soundHit;
  sf::Sound soundWin;

  int boardOffsetX1, boardOffsetX2, boardOffsetY;
  int lastShotX, lastShotY;
  int shipPlaceIndex;
  Orientation placeOrient;

  void initResources();
  void initMenuUI();
  void initHUD();

  void startGame(GameMode mode);
  void startSingleGame();
  void startHotseatGame();
  void startNetworkHostGame();
  void startNetworkClientGame();

  void processEvents();
  void handleMousePressed(int x, int y);
  void handleKeyPressed(sf::Keyboard::Key key);

  void handleMenuClick(int x, int y);
  void handlePlacingClick(int x, int y);
  void handleHotseatPlacingClick(int x, int y);
  void handleHotseatPlayer0PlacementClick(int x, int y);
  void handleHotseatPlayer1PlacementClick(int x, int y);
  void handleSingleNetworkPlacingClick(int x, int y);

  void handleKeyBackspace();
  void handleKeyAuto();
  void handleBackspaceHotseat();
  void handleBackspaceSingle();
  void handleAutoHotseatFirst();
  void handleAutoHotseatSecond();
  void handleAutoSingleNetwork();

  void update();
  void updateConnecting();
  void updateNetworkPlacement();
  void updateSingleAI();
  void updateNetworkReceiveShot();
  void processNetworkShot(int sx, int sy);
  void updateNetworkReceiveResult();
  void processNetworkShotResult(int rcode);

  void render();
  void renderMainMenu();
  void renderBoards(bool showShipsP1, bool showShipsP2);
  void renderLabels();
  void renderPlacementPreview();
  void renderGameInfo();
  void updateShipVisibility(bool& showShipsP1, bool& showShipsP2);

  void handleGameClick(int x, int y);
  void handleLocalGameClick(int x, int y);
  void handleNetworkGameClick(int x, int y);
};

#endif
