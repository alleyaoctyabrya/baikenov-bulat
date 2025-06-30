# Battleship Game 🚢

A classic Battleship game implementation with single-player, hot-seat, and network multiplayer modes, built with C++ and SFML.

## Features

- **Multiple Game Modes**:
  - 🎮 Single-player against AI opponent
  - 👥 Hot-seat multiplayer (two players on one computer)
  - 🌐 Network play (host/client)
  
- **Game Features**:
  - Interactive ship placement with auto-placement option
  - Intuitive UI with visual and sound feedback
  - Different difficulty levels in single-player mode
  - Real-time gameplay with instant feedback

## Table of Contents

1. [Requirements](#requirements)
2. [Building and Running](#building-and-running)
3. [How to Play](#how-to-play)
4. [Running Tests](#running-tests)
5. [Project Architecture](#project-architecture)
6. [Project Structure](#project-structure)
7. [Recent Improvements](#recent-improvements)
8. [Contributing](#contributing)
9. [License](#license)

## Requirements

| Tool | Minimum Version |
|------|-----------------|
| **CMake** | 3.15 |
| **C++17-compatible compiler** | GCC 9 / Clang 10 / MSVC 2019 |
| **Git** (for *FetchContent*) | any |

## Building and Running

```bash
# Clone the repository
git clone https://github.com/your-name/BattleshipGame.git
cd BattleshipGame

# Create build directory and configure
cmake -S . -B build

# Build the project
cmake --build build

# Run the game
./build/BattleshipGame
```

## How to Play

### Controls

- **Main Menu**:
  - Click on desired game mode to start
  
- **Ship Placement**:
  - Left-click: Place ship at cursor position
  - R: Rotate ship (horizontal/vertical)
  - Backspace: Reset all ship placements
  - A: Auto-place all ships randomly
  
- **During Game**:
  - Left-click: Shoot at selected grid cell on opponent's board
  - Game automatically switches turns in hotseat mode
  
### Game Modes

- **Single Player**: Play against an AI opponent.
- **Hot-seat Multiplayer**: Take turns with a friend on the same computer.
- **Network Play - Host**: Host a game that another player can connect to.
- **Network Play - Client**: Connect to another player's hosted game.

## Running Tests

The project includes unit tests using Google Test. To run the tests:

```bash
# Build and run tests
cmake --build build --target test

# Alternatively, run the test executable directly
./build/BattleshipTests
```

## Project Architecture

The game uses a state-based architecture with the following key components:

- **Game States**: The game flow is managed through state transitions (Main Menu, Ship Placement, Playing, Game Over)
- **Resource Management**: A centralized ResourceManager handles all game assets
- **Network Layer**: Supports peer-to-peer gameplay
- **AI**: Computer opponent with strategic targeting logic

The diagram below shows the core architecture:

```
Game (Controller)
  │
  ├── GameStates (Strategy Pattern)
  │     ├── MainMenuState
  │     ├── PlacingShipsState
  │     ├── ConnectingState
  │     ├── PlayingState
  │     └── GameOverState
  │
  ├── ResourceManager (Singleton)
  │
  ├── Players
  │     ├── Player
  │     ├── AI (extends Player)
  │     └── Board (contains Ships)
  │
  └── Network
```

## Project Structure

```
BattleshipGame/
├── CMakeLists.txt          # Build configuration
├── README.md               # This file
├── include/                # Header files
│   ├── AI.h                # Computer opponent
│   ├── Board.h             # Game board implementation
│   ├── Game.h              # Main game controller
│   ├── GameState.h         # Base state class
│   ├── MainMenuState.h     # Menu state
│   ├── PlacingShipsState.h # Ship placement state
│   ├── ConnectingState.h   # Network connection state
│   ├── PlayingState.h      # Main gameplay state
│   ├── GameOverState.h     # End game state
│   ├── Network.h           # Networking functionality
│   ├── Player.h            # Player interface
│   ├── ResourceManager.h   # Asset management
│   └── Ship.h              # Ship implementation
├── src/                    # Implementation files
├── test/                   # Unit tests
│   ├── AI_test.cpp
│   ├── Board_test.cpp
│   ├── Game_test.cpp
│   ├── Network_test.cpp
│   ├── Player_test.cpp
│   └── Ship_test.cpp
└── res/                    # Game resources
    ├── fonts/              # Text fonts
    ├── sounds/             # Game sound effects
    └── textures/           # Game graphics
```

## Recent Improvements

The project has undergone significant refactoring to improve code quality and maintainability:

- **Implementation of State Pattern**: Game states are now properly encapsulated in separate classes
- **Resource Management**: Centralized asset loading with better error handling
- **Modern C++ Features**: Smart pointers, RAII principles, and improved memory management
- **Enhanced Error Handling**: More robust error checking throughout the codebase
- **Code Organization**: Better separation of concerns between components

## Contributing

Contributions are welcome! Here's how you can contribute:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

Please make sure to update tests as appropriate and follow the existing code style.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- SFML - Simple and Fast Multimedia Library
- Google Test - Testing framework
- All contributors who have helped shape this project
