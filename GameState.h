#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "Player.h"
#include <utility>

// needed forward declaration
class Player;

class GameState {

    public:
        static const int MAP_X = 25;
        static const int MAP_Y = 25;

        enum TileType {
            TILE_SNAKE,
            TILE_FOOD,
            TILE_WALL,
            TILE_EMPTY
        };
        
        int score;
        TileType world[MAP_X][MAP_Y];
        Player* player;
        std::vector<std::pair<int, int>> tail;
        std::vector<std::pair<int, int>> initial_food_spots;

        GameState();
        void print();
        bool is_game_over();
        void reset();
        void reset(std::vector<std::pair<int, int>>);
        void initialize_world(bool do_make_food);
        void move_player(std::pair<int, int>);
        void register_player(Player*);
        void make_food();

    private:
        bool game_over = false;
    

};

#endif
