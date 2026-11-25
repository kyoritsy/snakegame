#pragma once
#include <SFML/Graphics.hpp>
#include <random>
#include "Snake.h"
#include "Constants.h"

class Game {
public:
    Game();
    void run();

private:
    sf::RenderWindow window;
    sf::Font font;
    sf::Text infoText;
    sf::Text helpText;

    Snake player1;
    Snake player2;

    Segment food;
    int currentPlayer;
    int movesLeft;
    bool gameOver;
    bool showHelpOverlay;
    bool showStartScreen;

    std::mt19937 rng;

    void spawnFood();
    void step(int dx, int dy);

    void drawGrid();
    void drawSnake(const Snake& snake, sf::Color bodyColor, sf::Color headColor);
    void drawFood();
    void drawInfoPanel();
    void drawHelpOverlay();
    void drawStartScreen();
    void drawGameOver();

    void handleEvents();
    void render();
    void reset();
};