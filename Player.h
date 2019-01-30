#ifndef PLAYER_H
#define PLAYER_H

#include "NeuralNetwork.h"
#include "GameState.h"
#include <utility>

// needed forward declaration
class GameState;

class Player {

    public:
        NeuralNetwork* brain;
        GameState* game;
        std::pair<int, int> position;
        int length = 2;
        int steps = 0;
        int turn_sum = 0;
        int turns_without_food = 0;
    
        std::pair<int, int> decide_next_move();
        void reset();
        Player(GameState* game);
    
    private:
        int VIEW_DISTANCE = 3;
        std::vector<std::pair<int, int>> tail; 
        
        

};

#endif
