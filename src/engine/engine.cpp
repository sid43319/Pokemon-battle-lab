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


    //checks the effectiveness of move based on move and defender type matchups
    double effectiveness = typeEffectiveness(move.type, defender.type);
    
    //calculate Same-type attack bonus, equals 1.5 if the move is same type as attacker
    double STAB = move.type == attacker.type ? 1.5 : 1.0;

    double finalDamage = total * effectiveness * STAB;

    int damage = (int) finalDamage;

    if (damage <= 1) {
        damage = 1;
    }

    return damage;

}

int chooseRandomMove(const Pokemon& pokemon, std::mt19937& rng) {
    if (pokemon.moves.empty()) {
        return -1;
    }

    std::uniform_int_distribution<int> dist(0, pokemon.moves.size() - 1);
    return dist(rng);

}

double typeEffectiveness(Type moveType, Type defenderType) {
    if (moveType == Type::Normal) {
        return 1.0;
    } else if (moveType == Type::Fire) {
        if (defenderType == Type::Normal) {
            return 1.0;
        } else if (defenderType == Type::Fire || defenderType == Type::Water) {
            return 0.5;
        } else if (defenderType == Type::Grass) {
            return 2.0;
        }
    } else if (moveType == Type::Water) {
        if (defenderType == Type::Normal) {
            return 1.0;
        } else if (defenderType == Type::Water || defenderType == Type::Grass) {
            return 0.5;
        } else if (defenderType == Type::Fire) {
            return 2.0;
        }
    } else if (moveType == Type::Grass) {
        if (defenderType == Type::Normal) {
            return 1.0;
        } else if (defenderType == Type::Fire || defenderType == Type::Grass) {
            return 0.5;
        } else if (defenderType == Type::Water) {
            return 2.0;
        }
    }
    return 1.0;
}

bool isValidAction(const Pokemon& pokemon, const Action& action) {
    if (pokemon.isFainted()) {
        return false;
    } else if (action.moveIndex < 0) {
        return false;
    } else if (static_cast<std::size_t>(action.moveIndex) >= pokemon.moves.size()) {
        return false;
    }
    return true;
}

TurnResult resolveTurn(BattleState& battle, const Action& firstAction, const Action& secondAction) {
    if(battle.isOver()) {
        return {TurnStatus::BattleOver, {}};
    }

    //validate actions
    if (!isValidAction(battle.first, firstAction)) {
        return {TurnStatus::InvalidFirstAction, {}};
    }

    if (!isValidAction(battle.second, secondAction)) {
        return {TurnStatus::InvalidSecondAction, {}};
    }

    Move first = battle.first.moves[firstAction.moveIndex];
    Move second = battle.second.moves[secondAction.moveIndex];

    std::vector<AttackResult> turn = performTurn(battle.first, battle.second, first, second, battle.rng);
    battle.turnNumber++;
    return {TurnStatus::Resolved, turn};
}