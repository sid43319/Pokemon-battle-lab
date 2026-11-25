#pragma once
#include <engine/pokemon.hpp>

struct BattleState {
    Pokemon first;
    Pokemon second;
    int turnNumber = 1;

    bool isOver() const {
        return first.isFainted() || second.isFainted();
    }
};