#include <iostream>
#include <engine/pokemon.hpp>
#include <engine/battle.hpp>
#include <engine/battle_state.hpp>
#include <limits>


int chooseMove(const Pokemon& pokemon) {
        int moveCount = 1;
        int moveNumber = 0;
        std::cout << "Choose a move:\n";
        for (const Move& move: pokemon.moves) {
            std::cout << moveCount << ". " << move.name << " (Power: " << move.power << ")\n";
            moveCount++;
        }

        if (pokemon.moves.empty()) {
            return -1;
        }

        while (true) {
            std::cout << "Enter move number: ";
            if (!(std::cin >> moveNumber)) {
                if (std::cin.eof()) {
                    return -1;
                }
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Please enter a number.\n";
                continue;
            }

            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (moveNumber >= 1 &&
                moveNumber <= static_cast<int>(pokemon.moves.size())) {
                break;
            }
            std::cout << "That move number is out of range.\n";
        }
        return moveNumber -1;
}


int main() {
    struct Pokemon charmander;
    charmander.attack = 20;
    charmander.currentHp = 50;
    charmander.defense = 20;
    charmander.maxHp = 50;
    charmander.moves = {{"Flamethrower", 30}, {"Scratch", 20}};
    charmander.name = "Charmander";
    charmander.speed = 30;

    struct Pokemon squirtle;
    squirtle.attack = 25;
    squirtle.currentHp = 50;
    squirtle.defense = 15;
    squirtle.maxHp = 50;
    squirtle.moves = {{"Water Gun", 35}, {"Tackle", 20}};
    squirtle.name = "Squirtle";
    squirtle.speed = 20;

    BattleState battle{charmander, squirtle};


    while (!battle.isOver()) {
        std::cout << "Turn Count: " << battle.turnNumber << std::endl;
        int moveIndex = chooseMove(battle.first);
        if (moveIndex == -1) {
            return 0;
        }
        Move moveChoice = battle.first.moves[moveIndex];
        performTurn(battle.first, battle.second, moveChoice, battle.second.moves[0]);
        std::cout << battle.first.name << ": " << battle.first.currentHp << "HP" << std::endl;
        std::cout << battle.second.name << ": " << battle.second.currentHp << "HP" << std::endl;
        battle.turnNumber++;

    }

    if (battle.first.isFainted()) {
        std::cout << battle.second.name << " is the winner!" << std::endl;
    } else if (battle.second.isFainted()) {
        std::cout << battle.first.name << " is the winner!" << std::endl;
    }

    return 0;
}
