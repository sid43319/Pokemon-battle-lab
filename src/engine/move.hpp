#pragma once
#include <string>
#include <engine/type.hpp>

enum class MoveCategory {
    Physical,
    Special
};



struct Move {
    std::string name;
    int power = 0;
    int accuracy = 100;
    MoveCategory category = MoveCategory::Physical;
    int priority = 0;
    Type type = Type::Normal;
};