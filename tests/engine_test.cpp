#include <gtest/gtest.h>
#include "engine/pokemon.hpp"
#include "engine/battle.hpp"
// This checks the test setup; battle-rule tests will come later.
TEST(Setup, BasicAssertion) {
    EXPECT_EQ(2 + 2, 4);
}


TEST(Pokemon, StoresHealth) {
    Pokemon pikachu;
    pikachu.maxHp = 100;
    pikachu.currentHp = 65;

    EXPECT_EQ(pikachu.maxHp, 100);
    EXPECT_EQ(pikachu.currentHp, 65);
}

TEST(Pokemon, TakingDamageReducesHealth) {
    Pokemon pikachu;
    pikachu.maxHp = 100;
    pikachu.currentHp = 65;

    pikachu.takeDamage(20);

    EXPECT_EQ(pikachu.currentHp, 45);
    EXPECT_EQ(pikachu.maxHp, 100);
}

TEST(Pokemon, TakingNegativeDamageDoesNothing) {
    Pokemon pikachu;
    pikachu.maxHp = 100;
    pikachu.currentHp = 65;

    pikachu.takeDamage(-20);

    EXPECT_EQ(pikachu.currentHp, 65);
    EXPECT_EQ(pikachu.maxHp, 100);
}

TEST(Pokemon, TakingDamageGreaterThanCurrHpEqualsZero) {
    Pokemon pikachu;
    pikachu.maxHp = 100;
    pikachu.currentHp = 65;

    pikachu.takeDamage(120);

    EXPECT_EQ(pikachu.currentHp, 0);
    EXPECT_EQ(pikachu.maxHp, 100);
}

TEST(Pokemon, PokemonFainted) {
    Pokemon pikachu;
    pikachu.maxHp = 100;
    pikachu.currentHp = 0;

    EXPECT_TRUE(pikachu.isFainted());
}

TEST(Pokemon, PokemonNotFainted) {
    Pokemon pikachu;
    pikachu.maxHp = 100;
    pikachu.currentHp = 1;

    EXPECT_FALSE(pikachu.isFainted());
}

TEST(Pokemon, TakingAttackReducesDefenderHp) {
    Pokemon pikachu;
    pikachu.maxHp = 100;
    pikachu.currentHp = 65;
    pikachu.attack = 20;
    pikachu.defense = 20;
    Pokemon squirtle;
    squirtle.maxHp = 60;
    squirtle.currentHp = 50;
    squirtle.moves = {{"Water Gun", 35}};
    squirtle.attack = 20;
    squirtle.defense = 20;

    performAttack(squirtle, pikachu, squirtle.moves[0]);

    EXPECT_EQ(pikachu.currentHp, 30);
    EXPECT_EQ(squirtle.currentHp, 50);
}

TEST(Pokemon, FaintedAttackerDoesNoDamage) {
    Pokemon pikachu;
    pikachu.maxHp = 100;
    pikachu.currentHp = 65;
    pikachu.attack = 20;
    pikachu.defense = 20;
    Pokemon squirtle;
    squirtle.maxHp = 60;
    squirtle.currentHp = 0;
    squirtle.moves = {{"Water Gun", 35}};
    squirtle.attack = 20;
    squirtle.defense = 20;

    performAttack(squirtle, pikachu, squirtle.moves[0]);

    EXPECT_EQ(pikachu.currentHp, 65);
    EXPECT_EQ(squirtle.currentHp, 0);
}

TEST(Pokemon, SpeedDeterminesOrder) {
    Pokemon pikachu;
    pikachu.maxHp = 100;
    pikachu.currentHp = 65;
    pikachu.speed = 30;
    pikachu.moves = {{"Thunder Shock", 40}};
    pikachu.attack = 20;
    pikachu.defense = 20;
    Pokemon squirtle;
    squirtle.maxHp = 60;
    squirtle.currentHp = 50;
    squirtle.moves = {{"Water Gun", 70}};
    squirtle.speed = 35;
    squirtle.attack = 20;
    squirtle.defense = 20;

    performTurn(pikachu, squirtle, pikachu.moves[0], squirtle.moves[0]);

    EXPECT_EQ(pikachu.currentHp, 0);
    EXPECT_EQ(squirtle.currentHp, 50);
}


TEST(Pokemon, DiffSpeedsBothAttack) {
    Pokemon pikachu;
    pikachu.maxHp = 100;
    pikachu.currentHp = 65;
    pikachu.speed = 30;
    pikachu.moves = {{"Thunder Shock", 40}};
    pikachu.attack = 20;
    pikachu.defense = 20;
    Pokemon squirtle;
    squirtle.maxHp = 60;
    squirtle.currentHp = 50;
    squirtle.moves = {{"Water Gun", 30}};
    squirtle.speed = 35;
    squirtle.attack = 20;
    squirtle.defense = 20;

    performTurn(pikachu, squirtle, pikachu.moves[0], squirtle.moves[0]);

    EXPECT_EQ(pikachu.currentHp, 35);
    EXPECT_EQ(squirtle.currentHp, 10);
}

TEST(Pokemon, AttackWithDefenseAndOffense) {
    Pokemon pikachu;
    pikachu.moves = {{"Thunder Shock", 30}};
    pikachu.attack = 20;
    Pokemon squirtle;
    squirtle.defense = 20;

    // Equal Attack and Defense leave damage equal to move power.
    EXPECT_EQ(calculateDamage(pikachu, squirtle, pikachu.moves[0]), 30);

    // Doubling Attack doubles damage.
    pikachu.attack = 40;
    EXPECT_EQ(calculateDamage(pikachu, squirtle, pikachu.moves[0]), 60);

    // Reset Attack, then double Defense to halve the original damage.
    pikachu.attack = 20;
    squirtle.defense = 40;
    EXPECT_EQ(calculateDamage(pikachu, squirtle, pikachu.moves[0]), 15);
}
