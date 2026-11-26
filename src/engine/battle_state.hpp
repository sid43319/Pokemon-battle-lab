#pragma once
#include <engine/pokemon.hpp>
#include <random>

struct BattleState {
    Pokemon first;
    Pokemon second;
    int turnNumber = 1;
    std::mt19937 rng{42};
    
    bool isOver() const {
        return first.isFainted() || second.isFainted();
    }
};