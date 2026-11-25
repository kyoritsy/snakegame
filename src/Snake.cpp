#include "Snake.h"

// ---------- Конструктор ----------
Snake::Snake(int startX, int startY)
    : length(1), dx(0), dy(0), score(0), alive(true)
{
    body[0] = {startX, startY};
}

// ---------- Установка направления ----------
bool Snake::setDirection(int newDx, int newDy) {
    if ((dx != 0 || dy != 0) && dx == -newDx && dy == -newDy) {
        return false;
    }
    dx = newDx;
    dy = newDy;
    return true;
}

// ---------- Движение ----------
void Snake::move(bool grow) {
    if (!alive) return;

    Segment tail = body[length - 1];

    for (int i = length - 1; i > 0; --i) {
        body[i] = body[i - 1];
    }

    body[0].x += dx;
    body[0].y += dy;

    if (grow && length < MAX_SNAKE_LENGTH) {
        body[length] = tail;
        ++length;
    }
}

bool Snake::checkCollision(int gridSize) {
    if (body[0].x < 0 || body[0].x >= gridSize ||
        body[0].y < 0 || body[0].y >= gridSize) {
        alive = false;
        return true;
    }
    for (int i = 1; i < length; ++i) {
        if (body[0].x == body[i].x && body[0].y == body[i].y) {
            alive = false;
            return true;
        }
    }
    return false;
}

bool Snake::isOnCell(int x, int y) const {
    for (int i = 0; i < length; ++i) {
        if (body[i].x == x && body[i].y == y) return true;
    }
    return false;
}