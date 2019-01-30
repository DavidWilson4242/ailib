#include <iostream>
#include <thread>
#include <chrono>
#include <time.h>
#include "NeuralNetwork.h"
#include "GameState.h"
#include "Player.h"

const int NUM_PLAYERS = 1000;
int generation = 0;
const std::chrono::milliseconds STEP_TIME_MS(30);

int main(int argc, char** argv) {

    srand(time(NULL));

    GameState game;
    std::vector<Player*> players;
    
    for (int i = 0; i < NUM_PLAYERS; i++) {
        players.push_back(new Player(&game));
    }

    int max_steps = 0;
    
    // main game loop
    while (true) {

        std::vector<std::pair<int, int>> initial_food_spots;
        
        for (int i = 0; i < NUM_PLAYERS; i++) {
            Player* player = players[i];
            if (i == 0) {
                game.reset();
                initial_food_spots = game.initial_food_spots;
            } else {
                game.reset(initial_food_spots);
            }
            game.register_player(player);
            while (!game.is_game_over()) {
                game.move_player(player->decide_next_move());
                if (i == 0) {
                    game.print();
                    std::cout << "player:     (" << i << " / " << NUM_PLAYERS << ")" << std::endl;
                    std::cout << "generation: " << generation << std::endl;
                    std::cout << "max steps:  " << max_steps << std::endl;
                    player->brain->print(NeuralNetwork::PrintType::CHARGE);
                    std::this_thread::sleep_for(STEP_TIME_MS);
                }
            }
        }
        generation++;

        std::sort(players.begin(), players.end(), [](Player* a, Player* b) -> bool {
            return (a->length + 0*(double)a->steps/100.0) > (b->length + 0*(double)b->steps/100.0);
        });

        max_steps = players[0]->length;

        for (int i = NUM_PLAYERS/2; i < NUM_PLAYERS; i++) {
            players[i - NUM_PLAYERS/2]->brain->convert(players[i]->brain);
        }

        for (auto player: players) {
            player->reset();
        }

        std::this_thread::sleep_for(STEP_TIME_MS);
    }

    return 0;
}
