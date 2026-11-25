#pragma once
#include "Constants.h"

// Класс одной змейки: массив сегментов, направление, очки, состояние.
class Snake {
public:
    Segment body[MAX_SNAKE_LENGTH];
    int length;
    int dx, dy;
    int score;
    bool alive;

    Snake(int startX, int startY);
    bool setDirection(int newDx, int newDy);
    void move(bool grow);
    bool checkCollision(int gridSize);
    bool isOnCell(int x, int y) const;
};