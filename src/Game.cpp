#include "Game.h"
#include <sstream>
#include <iostream>
#include <optional>
#include <string>

// ==================== КОНСТРУКТОР ====================
Game::Game()
    : window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Snake: Turn-Based"),
      font(),
      infoText(font),
      helpText(font),
      player1(START_X1, START_Y1),
      player2(START_X2, START_Y2),
      currentPlayer(0),
      movesLeft(MOVES_PER_TURN),
      gameOver(false),
      showHelpOverlay(false),
      showStartScreen(true),
      rng(std::random_device{}())
{
    if (!font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        std::cerr << "Ошибка загрузки шрифта DejaVuSans.ttf\n";
        std::cerr << "Установите: sudo apt install fonts-dejavu\n";
    }

    infoText.setCharacterSize(12);
    infoText.setFillColor(sf::Color::White);

    helpText.setCharacterSize(14);
    helpText.setFillColor(sf::Color::White);

    spawnFood();
}

// ==================== СБРОС ИГРЫ (РЕСТАРТ) ====================
void Game::reset() {
    player1 = Snake(START_X1, START_Y1);
    player2 = Snake(START_X2, START_Y2);
    
    currentPlayer = 0;
    movesLeft = MOVES_PER_TURN;
    gameOver = false;
    showHelpOverlay = false;
    showStartScreen = false; 
    
    spawnFood();
}

// ==================== ИГРОВАЯ ЛОГИКА ====================

void Game::spawnFood() {
    std::uniform_int_distribution<int> dist(0, GRID_SIZE - 1);
    do {
        food.x = dist(rng);
        food.y = dist(rng);
    } while (player1.isOnCell(food.x, food.y) ||
             player2.isOnCell(food.x, food.y));
}

void Game::step(int dx, int dy) {
    if (gameOver || showHelpOverlay || showStartScreen) return;

    Snake& current = (currentPlayer == 0) ? player1 : player2;
    if (!current.alive) return;

    if (!current.setDirection(dx, dy)) {
        return; 
    }

    int newX = current.body[0].x + current.dx;
    int newY = current.body[0].y + current.dy;
    bool grow = (newX == food.x && newY == food.y);

    current.move(grow);

    if (grow) {
        current.score++;
        spawnFood();
    }

    if (current.checkCollision(GRID_SIZE)) {
        gameOver = true;
        return;
    }

    if (current.score >= WIN_SCORE) {
        gameOver = true;
        return;
    }

    --movesLeft;
    if (movesLeft == 0) {
        currentPlayer = 1 - currentPlayer;
        movesLeft = MOVES_PER_TURN;
    }
}

// ==================== ОТРИСОВКА ====================

void Game::drawGrid() {
    sf::VertexArray grid(sf::PrimitiveType::Lines);
    sf::Color gridColor(50, 50, 50);

    for (int i = 0; i <= GRID_SIZE; ++i) {
        float pos = i * CELL_SIZE;
        float maxPos = GRID_SIZE * CELL_SIZE;

        grid.append(sf::Vertex{sf::Vector2f(pos, 0.f), gridColor});
        grid.append(sf::Vertex{sf::Vector2f(pos, maxPos), gridColor});
        grid.append(sf::Vertex{sf::Vector2f(0.f, pos), gridColor});
        grid.append(sf::Vertex{sf::Vector2f(maxPos, pos), gridColor});
    }
    window.draw(grid);
}

void Game::drawSnake(const Snake& snake, sf::Color bodyColor, sf::Color headColor) {
    for (int i = 0; i < snake.length; ++i) {
        sf::RectangleShape rect({CELL_SIZE - 2.f, CELL_SIZE - 2.f});
        rect.setPosition({snake.body[i].x * CELL_SIZE + 1.f,
                          snake.body[i].y * CELL_SIZE + 1.f});
        rect.setFillColor(i == 0 ? headColor : bodyColor);
        window.draw(rect);
    }
}

void Game::drawFood() {
    float radius = CELL_SIZE / 3.0f; 
    sf::CircleShape shape(radius);
    shape.setFillColor(sf::Color::Red);
    
    float offset = (CELL_SIZE - radius * 2.0f) / 2.0f;
    
    shape.setPosition({food.x * CELL_SIZE + offset,
                       food.y * CELL_SIZE + offset});
    window.draw(shape);
}

void Game::drawInfoPanel() {
    sf::RectangleShape panel(sf::Vector2f(WINDOW_WIDTH, INFO_HEIGHT));
    panel.setPosition({0.f, GRID_SIZE * CELL_SIZE});
    panel.setFillColor(sf::Color(30, 30, 30));
    window.draw(panel);

    std::ostringstream oss;
    oss << "Игрок 1 (WASD): "      << player1.score << " очков\n"
        << "Игрок 2 (стрелки): "   << player2.score << " очков\n"
        << "Ход игрока: "          << (currentPlayer == 0 ? "1" : "2")
        << " | Осталось ходов: "   << movesLeft << "\n"
        << "H — справка, Esc — выход";

    std::string str = oss.str();
    infoText.setString(sf::String::fromUtf8(str.begin(), str.end()));
    infoText.setPosition({10.f, GRID_SIZE * CELL_SIZE + 10.f});
    window.draw(infoText);
}

void Game::drawHelpOverlay() {
    sf::RectangleShape overlay(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    overlay.setFillColor(sf::Color(0, 0, 0, 230));
    window.draw(overlay);

    std::string s =
        "=== СПРАВКА ===\n\n"
        "Цель игры:\n"
        "  Съешьте как можно больше еды.\n"
        "  Побеждает первый набравший " + std::to_string(WIN_SCORE) + " очков\n"
        "  или единственный выживший.\n\n"
        "Правила:\n"
        "  - Поле 10x10.\n"
        "  - Игроки ходят по очереди по " + std::to_string(MOVES_PER_TURN) + " ходов.\n"
        "  - Змейки не сталкиваются друг с другом.\n"
        "  - Нельзя врезаться в стену или в свой хвост.\n"
        "  - За еду +1 очко и +1 к длине.\n\n"
        "Управление:\n"
        "  Игрок 1: W, A, S, D\n"
        "  Игрок 2: стрелки\n"
        "  H   — показать/скрыть справку\n"
        "  Esc — выход\n\n"
        "Нажмите H, чтобы продолжить.";

    helpText.setString(sf::String::fromUtf8(s.begin(), s.end()));
    helpText.setPosition({20.f, 20.f});
    window.draw(helpText);
}

void Game::drawStartScreen() {
    std::string s =
        "=== ЗМЕЙКА: ИГРА ПО ОЧЕРЕДИ ===\n\n"
        "Правила:\n"
        "  - Два игрока на одном поле 10x10.\n"
        "  - Игроки ходят по очереди по " + std::to_string(MOVES_PER_TURN) + " ходов.\n"
        "  - Змейки не сталкиваются друг с другом.\n"
        "  - Собирайте еду, чтобы расти и получать очки.\n"
        "  - Побеждает тот, кто наберёт " + std::to_string(WIN_SCORE) + " очков\n"
        "    или останется единственным выжившим.\n\n"
        "Управление:\n"
        "  Игрок 1: W, A, S, D\n"
        "  Игрок 2: стрелки\n\n"
        "Нажмите Enter, чтобы начать.";

    sf::Text text(font);
    text.setString(sf::String::fromUtf8(s.begin(), s.end()));
    text.setCharacterSize(14);
    text.setFillColor(sf::Color::White);
    text.setPosition({20.f, 20.f});
    window.draw(text);
}

void Game::drawGameOver() {
    sf::RectangleShape overlay(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    overlay.setFillColor(sf::Color(0, 0, 0, 200));
    window.draw(overlay);

    std::string result;
    if (player1.alive && !player2.alive)      result = "Игрок 1 победил!";
    else if (player2.alive && !player1.alive) result = "Игрок 2 победил!";
    else if (player1.score >= WIN_SCORE && player2.score >= WIN_SCORE)
                                              result = "Ничья! Оба набрали " + std::to_string(WIN_SCORE) + ".";
    else if (player1.score >= WIN_SCORE)      result = "Игрок 1 победил по очкам!";
    else if (player2.score >= WIN_SCORE)      result = "Игрок 2 победил по очкам!";
    else                                      result = "Игра окончена.";

    std::ostringstream oss;
    oss << result << "\n\n"
        << "Игрок 1: " << player1.score << " очков\n"
        << "Игрок 2: " << player2.score << " очков\n\n"
        << "Нажмите R для рестарта\n"
        << "или Esc для выхода.";

    sf::RectangleShape textBg(sf::Vector2f(300.f, 180.f));
    textBg.setPosition({(WINDOW_WIDTH - 300.f) / 2.f, WINDOW_HEIGHT / 2.f - 100.f});
    textBg.setFillColor(sf::Color(40, 40, 40));
    textBg.setOutlineThickness(2.f);
    textBg.setOutlineColor(sf::Color::Yellow);
    window.draw(textBg);

    sf::Text text(font);
    std::string str = oss.str();
    text.setString(sf::String::fromUtf8(str.begin(), str.end()));
    text.setCharacterSize(16);
    text.setFillColor(sf::Color::Yellow);
    
    text.setPosition({(WINDOW_WIDTH - 260.f) / 2.f, WINDOW_HEIGHT / 2.f - 80.f});
    window.draw(text);
}

// ==================== СОБЫТИЯ ====================

void Game::handleEvents() {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            return;
        }

        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (showStartScreen) {
                if (keyPressed->code == sf::Keyboard::Key::Enter)
                    showStartScreen = false;
                continue;
            }

            if (keyPressed->code == sf::Keyboard::Key::R && gameOver) {
                reset();
                continue;
            }

            if (keyPressed->code == sf::Keyboard::Key::H) {
                if (!gameOver) showHelpOverlay = !showHelpOverlay;
            }
            else if (keyPressed->code == sf::Keyboard::Key::Escape) {
                if (showHelpOverlay) showHelpOverlay = false;
                else                 window.close();
            }
            else if (!gameOver && !showHelpOverlay) {
                if (currentPlayer == 0) {
                    if (keyPressed->code == sf::Keyboard::Key::W)      step(0, -1);
                    else if (keyPressed->code == sf::Keyboard::Key::S) step(0,  1);
                    else if (keyPressed->code == sf::Keyboard::Key::A) step(-1, 0);
                    else if (keyPressed->code == sf::Keyboard::Key::D) step( 1, 0);
                } else {
                    if (keyPressed->code == sf::Keyboard::Key::Up)         step(0, -1);
                    else if (keyPressed->code == sf::Keyboard::Key::Down)  step(0,  1);
                    else if (keyPressed->code == sf::Keyboard::Key::Left)  step(-1, 0);
                    else if (keyPressed->code == sf::Keyboard::Key::Right) step( 1, 0);
                }
            }
        }
    }
}

// ==================== РЕНДЕР ====================

void Game::render() {
    window.clear(sf::Color::Black);

    if (showStartScreen) {
        drawStartScreen();
        window.display();
        return;
    }

    drawGrid();
    drawFood();
    drawSnake(player1, sf::Color::Green, sf::Color(144, 238, 144)); 
    drawSnake(player2, sf::Color::Blue,  sf::Color::Cyan);
    drawInfoPanel();

    if (showHelpOverlay) drawHelpOverlay();
    if (gameOver)        drawGameOver();

    window.display();
}

// ==================== ГЛАВНЫЙ ЦИКЛ ====================

void Game::run() {
    while (window.isOpen()) {
        handleEvents();
        render();
    }
}