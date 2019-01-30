#include "GameState.h"
#include <iostream>
#include <utility>

GameState::GameState() {
    initialize_world(true);
}

void
GameState::initialize_world(bool do_make_food) {
    // initialize everything as empty
    for (int x = 0; x < MAP_X; x++) {
        for (int y = 0; y < MAP_Y; y++) {
            world[x][y] = TILE_EMPTY;       
        }
    } 
    
    // create wall boundaries
    for (int x = 0; x < MAP_X; x++) {
        world[x][0]         = TILE_WALL;
        world[x][MAP_Y - 1] = TILE_WALL;
    }
    for (int y = 0; y < MAP_Y; y++) {
        world[0][y]         = TILE_WALL;
        world[MAP_X - 1][y] = TILE_WALL;
    }
    
    /*
    for (int i = 0; i < 4; i++) {
        int x = ((double)rand() / RAND_MAX) * (MAP_X - 4);
        int y = ((double)rand() / RAND_MAX) * (MAP_Y - 4);
        if (x >= 3 && x <= 9 && y >= 3 && y <= 9) {
            continue;
        }
        int dir = (double)rand() / RAND_MAX;
        for (int j = 0; j < 4; j++) {
            if (dir < 0.5) {
                world[x + j][y] = TILE_WALL;
            } else {
                world[x][y + j] = TILE_WALL;
            }
        }
    }
    */
    
    if (do_make_food) { 
        for (int i = 0; i < 30; i++) {
            make_food();
        }
    }
    score = 0;
}

bool
GameState::is_game_over() {
    return game_over;
}

void
GameState::make_food() {
    
    std::vector<std::pair<int, int>> free_spots;

    for (int i = 0; i < MAP_X; i++) {
        for (int j = 0; j < MAP_Y; j++) {
            if (world[i][j] == TILE_EMPTY) {
                free_spots.push_back(std::make_pair<int, int>((int)i, (int)j));
            }
        }
    }

    if (free_spots.size() == 0) {
        return;
    }

    std::random_shuffle(free_spots.begin(), free_spots.end());

    int x = free_spots[0].first;
    int y = free_spots[0].second;
    world[x][y] = TILE_FOOD;
    initial_food_spots.push_back(std::make_pair(x, y));

}

void
GameState::reset() {
    initialize_world(true);
    game_over = false;
}

void
GameState::reset(std::vector<std::pair<int, int>> initial_food_spots) {
    initialize_world(false);
    for (auto& pair: initial_food_spots) {
        world[pair.first][pair.second] = TILE_FOOD;   
    }
    game_over = false;
}

void
GameState::register_player(Player* p) {
    player = p;

    int px = player->position.first;
    int py = player->position.second;
    
    for (int i = 2; i >= 0; i--) {
        world[px - i][py] = TILE_SNAKE;
        tail.push_back(std::make_pair(px - i, py));
    }
}

void
GameState::move_player(std::pair<int, int> movement) {
    player->position.first += movement.first;
    player->position.second += movement.second;
    player->steps += 1;

    int px = player->position.first;
    int py = player->position.second;

    if (world[px][py] == TILE_WALL || world[px][py] == TILE_SNAKE) {
        game_over = true;
        return;
    }

    if (px < 0 || px > MAP_X || py < 0 || py > MAP_Y) {
        game_over = true;
        return;
    }

    if (world[px][py] == TILE_FOOD) {
        player->turns_without_food = 0;
        player->length++;
        score++;
        make_food();
    } else {
        player->turns_without_food++;
    }
    
    player->steps++;

    if (player->turns_without_food > 50) {
        player->length--;
        player->turns_without_food = 0;
    }

    if (player->length < 2) {
        game_over = true;
        return;
    }

    if (player->steps > 50000) {
        game_over = true;
        return;
    }

    world[px][py] = TILE_SNAKE;
    tail.insert(tail.begin(), std::make_pair(px, py));

    while (tail.size() > player->length) {
        std::pair<int, int> pos = tail[tail.size() - 1];
        tail.pop_back();
        world[pos.first][pos.second] = TILE_EMPTY;
    }
}

char tt_to_char(GameState::TileType t) {
    switch (t) {
        case GameState::TILE_SNAKE:
            return 'X';
        case GameState::TILE_FOOD:
            return 'F';
        case GameState::TILE_WALL:
            return 'W';
        case GameState::TILE_EMPTY:
            return ' ';
        default:
            return '?';
    }
}

void
GameState::print() {
    for (int i = 0; i < 50; i++) {
        std::cout << std::endl;
    }

    for (int y = 0; y < MAP_Y; y++) {
        for (int x = 0; x < MAP_X; x++) {
            std::cout << tt_to_char(world[x][y]) << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << "score: " << score << std::endl;
    
}
