#pragma once

const int GRID_SIZE = 10;
const int CELL_SIZE = 40;
const int INFO_HEIGHT = 120;
const int WINDOW_WIDTH = GRID_SIZE * CELL_SIZE;
const int WINDOW_HEIGHT = GRID_SIZE * CELL_SIZE + INFO_HEIGHT;
const int MAX_SNAKE_LENGTH = GRID_SIZE * GRID_SIZE;
const int MOVES_PER_TURN = 10;
const int WIN_SCORE = 15;

const int START_X1 = 2;
const int START_Y1 = GRID_SIZE / 2;
const int START_X2 = GRID_SIZE - 3;
const int START_Y2 = GRID_SIZE / 2;

struct Segment {
    int x, y;
};