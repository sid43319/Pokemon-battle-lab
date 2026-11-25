#include <engine/battle.hpp>


void performAttack(const Pokemon& attacker, Pokemon& defender, const Move& move) {
    if (attacker.currentHp ==0 || defender.currentHp == 0) {
        return;
    }
    int calcDamage = calculateDamage(attacker, defender, move);
    defender.takeDamage(calcDamage);
}

void performTurn(Pokemon& first, Pokemon& second, const Move& firstMove, const Move& secondMove) {
    if (first.currentHp == 0 || second.currentHp == 0) {
        return;
    }
    if (first.speed >= second.speed) {
        performAttack(first, second, firstMove);
        if (!second.isFainted()) {
            performAttack(second, first, secondMove);
        }
    } else {
        performAttack(second, first, secondMove);
        if (!first.isFainted()) {
            performAttack(first, second, firstMove);
        }
    }
}

int calculateDamage(const Pokemon& attacker, const Pokemon& defender, const Move& move) {
    int attack = attacker.attack;
    int movePower = move.power;
    int defense = defender.defense;

    if (attack <= 0 || movePower <= 0) {
        return 0;
    }

    if (defense <= 1) {
        defense = 1;
    }

    int total = (attack * movePower) / defense;

    if (total <= 1) {
        total = 1;
    }

    return total;

}