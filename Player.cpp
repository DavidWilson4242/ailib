#include "Player.h"
#include "GameState.h"
#include <iostream>
#include <math.h>

Player::Player(GameState* game_handle) {

    brain = new NeuralNetwork(7, 3, {16});
    game = game_handle;
    reset();

}

void
Player::reset() {
    position = std::make_pair<int, int>(7, 7);
    length = 4;
    steps = 0;
    turn_sum = 0;
    turns_without_food = 0;
}


std::pair<int, int>
Player::decide_next_move() {

    std::vector<double> input_data = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

    auto is_wall = [this](int x, int y) -> bool {
        if (x < 0 || x >= GameState::MAP_X || y < 0 || y >= GameState::MAP_Y) {
            return false;
        }
        return game->world[x][y] == GameState::TileType::TILE_WALL || game->world[x][y] == GameState::TileType::TILE_SNAKE;
    };

    auto is_food = [this](int x, int y) -> bool {
        if (x < 0 || x >= GameState::MAP_X || y < 0 || y >= GameState::MAP_Y) {
            return false;
        }
        return game->world[x][y] == GameState::TileType::TILE_FOOD;
    };
    
    int mx = position.first;
    int my = position.second;

    switch (turn_sum % 4) {
        case 0: // up
            input_data[0] = (int)is_wall(mx - 1, my);
            input_data[1] = (int)is_food(mx - 1, my);
            input_data[2] = (int)is_wall(mx, my - 1);
            input_data[3] = (int)is_food(mx, my - 1);
            input_data[4] = (int)is_wall(mx + 1, my);
            input_data[5] = (int)is_food(mx + 1, my);
            break;
        case 1: // right
            input_data[0] = (int)is_wall(mx, my - 1);
            input_data[1] = (int)is_food(mx, my - 1);
            input_data[2] = (int)is_wall(mx + 1, my);
            input_data[3] = (int)is_food(mx + 1, my);
            input_data[4] = (int)is_wall(mx, my + 1);
            input_data[5] = (int)is_food(mx, my + 1);
            break;
        case 2: // down
            input_data[0] = (int)is_wall(mx + 1, my);
            input_data[1] = (int)is_food(mx + 1, my);
            input_data[2] = (int)is_wall(mx, my + 1);
            input_data[3] = (int)is_food(mx, my + 1);
            input_data[4] = (int)is_wall(mx - 1, my);
            input_data[5] = (int)is_food(mx - 1, my);
            break;
        case 3: // left
            input_data[0] = (int)is_wall(mx, my + 1);
            input_data[1] = (int)is_food(mx, my + 1);
            input_data[2] = (int)is_wall(mx - 1, my);
            input_data[3] = (int)is_food(mx - 1, my);
            input_data[4] = (int)is_wall(mx, my - 1);
            input_data[5] = (int)is_food(mx, my - 1);
            break;
        default:
            break;
    }
    
    double best_mag = 1.0;

    for (int i = 0; i < GameState::MAP_X; i++) {
        for (int j = 0; j < GameState::MAP_Y; j++) {
            if (game->world[i][j] == GameState::TileType::TILE_FOOD) {
                double dist = sqrt(pow(i - mx, 2) + pow(j - my, 2)) / 100.0;
                if (dist < best_mag) {
                    best_mag = dist;
                }
            }
        }
    }
    
    //input_data[6] = fmax(0.0, 1.0 - best_mag*10);
    input_data[6] = (sin(6.28 * steps/200.0) / 2.0) + 0.5;

    std::vector<double> output = brain->feed_forward(input_data);
    // output format:
    //  [0] up
    //  [1] right
    //  [2] down
    //  [3] left

    double max_value = 0.0;
    int max_index = 0;

    for (int i = 0; i < 3; i++) {
        if (output[i] > max_value) {
            max_value = output[i];
            max_index = i;
        }
    }

    int d_sum = max_index - 1;
    turn_sum += d_sum;

    switch (turn_sum % 4) {
        case 0:
            return std::make_pair<int, int>(0, -1);
        case 1:
            return std::make_pair<int, int>(1, 0);
        case 2:
            return std::make_pair<int, int>(0, 1);
        case 3:
            return std::make_pair<int, int>(-1, 0);
        default:
            return std::make_pair<int, int>(0, 0);
    }


}
