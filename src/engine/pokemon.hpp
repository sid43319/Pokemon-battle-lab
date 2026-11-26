#pragma once
#include <string>
#include <engine/move.hpp>
#include <vector>
#include <engine/type.hpp>
//defined pokemon struct
struct Pokemon {
    std::string name;
    std::vector<Move> moves;
    int maxHp = 0;
    int currentHp = 0;
    int attack = 0;
    int defense = 0;
    int speed = 0;
    int specialAttack = 0;
    int specialDefense = 0;
    Type type = Type::Normal;


    //damagetaken func
    void takeDamage(int damage) {
        if(damage < 0) {
            return;
        }
        currentHp -= damage;
        if (currentHp < 0) {
            currentHp = 0;
        }
    }

    //checks if fainted
    bool isFainted() const {
       return currentHp == 0;
    }




};
