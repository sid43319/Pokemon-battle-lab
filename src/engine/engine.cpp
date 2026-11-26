#include <engine/battle.hpp>


AttackResult performAttack(const Pokemon& attacker, Pokemon& defender, const Move& move, std::mt19937& rng) {
    if (attacker.currentHp == 0 || defender.currentHp == 0) {
        return {};
    }
    bool hit = false;
    if (move.accuracy <= 0) {
        return {attacker.name, move.name, 0, false}; //in case of miss, no damage deducted
    } else if (move.accuracy >= 100) {
        hit = true;
    } else {
        std::uniform_int_distribution<int> dist(1,100);
        int roll = dist(rng);
        if(roll <= move.accuracy) { //if rng is less than given accuracy, hit
            hit = true;             //else keep the move as a miss
        }
    }

    if (hit) {
        int defenderHp = defender.currentHp;
        int calcDamage = calculateDamage(attacker, defender, move);
        defender.takeDamage(calcDamage);
        return {attacker.name, move.name, defenderHp - defender.currentHp, true};
    }
    
    return {attacker.name, move.name, 0, false}; //in case of miss, no damage deducted
 
}


std::vector<AttackResult> performTurn(Pokemon& first, Pokemon& second, const Move& firstMove, const Move& secondMove, std::mt19937& rng) {
    if (first.currentHp == 0 || second.currentHp == 0) {
        return {};
    }

    bool firstGoesFirst;
    bool diffPriority = false;
    //different move priority case
    if (firstMove.priority > secondMove.priority) {
        firstGoesFirst = true;
        diffPriority = true;
    } else if (firstMove.priority < secondMove.priority) {
        firstGoesFirst = false;
        diffPriority = true;
    }


    std::vector<AttackResult> results;

    //same speed, same priority case, choose randomly between who attacks first

    if ((first.speed == second.speed) && !diffPriority) {
        std::uniform_int_distribution<int> coinFlip(0, 1);
        firstGoesFirst = coinFlip(rng) == 0;
    } else if ((first.speed != second.speed) && !diffPriority) {
        firstGoesFirst = first.speed > second.speed;    //same priority, different speed case
    }


    if (firstGoesFirst) {
        results.push_back(performAttack(first, second, firstMove, rng));
        if (!second.isFainted()) {
            results.push_back(performAttack(second, first, secondMove, rng));
        }
    } else {
        results.push_back(performAttack(second, first, secondMove, rng));
        if (!first.isFainted()) {
            results.push_back(performAttack(first, second, firstMove, rng));
        }
    }
    return results;
}

int calculateDamage(const Pokemon& attacker, const Pokemon& defender, const Move& move) {
    int attack;
    int defense;
    //checks category of move, changing attack and defense stats
    if (move.category == MoveCategory::Physical) {
        attack = attacker.attack;
        defense = defender.defense;
    } else if (move.category == MoveCategory::Special) {
        attack = attacker.specialAttack;
        defense = defender.specialDefense;
    }

    int movePower = move.power;

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

int chooseRandomMove(const Pokemon& pokemon, std::mt19937& rng) {
    if (pokemon.moves.empty()) {
        return -1;
    }

    std::uniform_int_distribution<int> dist(0, pokemon.moves.size() - 1);
    return dist(rng);

}
