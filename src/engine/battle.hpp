#pragma once
#include <engine/pokemon.hpp>


void performAttack(const Pokemon& attacker, Pokemon& defender, const Move &move);
void performTurn(Pokemon& first, Pokemon& second, const Move& firstMove, const Move& secondMove);
int calculateDamage(const Pokemon& attacker, const Pokemon& defender, const Move& move);