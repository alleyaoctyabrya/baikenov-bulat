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
  void startGame(GameMode mode);
  void processEvents();
  void update();
  void render();
  void handleMenuClick(int x, int y);
  void handleGameClick(int x, int y);
};

#endif
