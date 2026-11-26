#pragma once
#include <engine/pokemon.hpp>
#include <random>
#include <string>
#include <vector>
#include <engine/action.hpp>
#include <engine/battle_state.hpp>





//create struct to hold data for pertinent info for each turn
struct AttackResult {
    std::string attackerName;
    std::string moveName;
    int damage = 0;
    bool hit = false;
};

enum class TurnStatus {
    Resolved,
    BattleOver,
    InvalidFirstAction,
    InvalidSecondAction
};


struct TurnResult {
    TurnStatus status = TurnStatus::BattleOver;
    std::vector<AttackResult> attacks;
};




AttackResult performAttack(const Pokemon& attacker, Pokemon& defender, const Move &move, std::mt19937& rng);
std::vector<AttackResult> performTurn(Pokemon& first, Pokemon& second, const Move& firstMove, const Move& secondMove, std::mt19937& rng);
int calculateDamage(const Pokemon& attacker, const Pokemon& defender, const Move& move);
int chooseRandomMove(const Pokemon& pokemon, std::mt19937& rng);  //mt19937 is random number generator
double typeEffectiveness(Type moveType, Type defenderType);
bool isValidAction(const Pokemon& pokemon, const Action& action);
TurnResult resolveTurn(BattleState& battle, const Action& firstAction, const Action& secondAction);
