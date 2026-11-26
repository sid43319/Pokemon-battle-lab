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
        return moveNumber - 1;
}


int main() {
    struct Pokemon charmander;
    charmander.type = Type::Fire;
    charmander.attack = 20;
    charmander.currentHp = 50;
    charmander.defense = 20;
    charmander.specialAttack = 25;
    charmander.specialDefense = 30;
    charmander.maxHp = 50;
    charmander.moves = {{"Flamethrower", 30, 100, MoveCategory::Special, 0, Type::Fire}, {"Scratch", 20, 100, MoveCategory::Physical}};
    charmander.name = "Charmander";
    charmander.speed = 30;

    struct Pokemon squirtle;
    squirtle.type = Type::Water;
    squirtle.attack = 25;
    squirtle.currentHp = 50;
    squirtle.defense = 15;
    squirtle.specialAttack = 30;
    squirtle.specialDefense = 30;
    squirtle.maxHp = 50;
    squirtle.moves = {{"Water Gun", 35, 95, MoveCategory::Special, 0, Type::Water}, {"Tackle", 20}, {"Quick Strike", 15, 100, MoveCategory::Physical, 1}};
    squirtle.name = "Squirtle";
    squirtle.speed = 20;

    BattleState battle{charmander, squirtle};

    while (!battle.isOver()) {
        std::cout << "Turn Count: " << battle.turnNumber << std::endl;

        //call chooseMove to allow user input to select the pokemon's moves
        int moveIndex = chooseMove(battle.first);
        if (moveIndex == -1) {
            return 0;
        }
        
        //attribute moveIndex to action type
        Action user{moveIndex};

        //randomly select opponent's move
        int opponentMoveIndex = chooseRandomMove(battle.second, battle.rng);
        if (opponentMoveIndex == -1) {
            return 0;
        }

        //attribute opponentMoveIndex to action type
        Action opponent{opponentMoveIndex};

        TurnResult result = resolveTurn(battle, user, opponent);

        if(result.status != TurnStatus::Resolved) {
            std::cout << "Turn cannot be completed, invalid turn status. Terminating battle." << std::endl;
            return 0;
        }

        //print out relevant info related to attack during each turn
        for (AttackResult a: result.attacks) {
            if (a.hit == false) {
                std::cout << a.attackerName << " used " << a.moveName << ", but missed!" << std::endl;
            } else {
                std::cout << a.attackerName << " used " << a.moveName << ", dealing " << a.damage << " damage." << std::endl;
            }
        }

        std::cout << battle.first.name << ": " << battle.first.currentHp << "HP" << std::endl;
        std::cout << battle.second.name << ": " << battle.second.currentHp << "HP" << std::endl;

    }

    if (battle.first.isFainted()) {
        std::cout << battle.second.name << " is the winner!" << std::endl;
    } else if (battle.second.isFainted()) {
        std::cout << battle.first.name << " is the winner!" << std::endl;
    }

    return 0;
}
