#ifndef MAINMENUSTATE_H
#define MAINMENUSTATE_H

#include "GameState.h"
#include <SFML/Graphics.hpp>
#include <array>

class MainMenuState : public GameState {
public:
    MainMenuState(Game* game);
    ~MainMenuState() override = default;

    // Core state functions
    void enter() override;
    void exit() override;
    void update() override;
    void render(sf::RenderWindow& window) override;
    
    // Event handlers
    void handleMousePressed(int x, int y) override;
    void handleKeyPressed(sf::Keyboard::Key key) override;

private:
    sf::Text titleText;
    static const int MENU_OPTION_COUNT = 5;
    std::array<sf::Text, MENU_OPTION_COUNT> menuOptions;
    
    void initMenuUI(sf::RenderWindow& window, sf::Font& font);
};

#endif // MAINMENUSTATE_H