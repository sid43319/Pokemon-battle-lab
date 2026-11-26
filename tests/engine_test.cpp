#include <gtest/gtest.h>
#include "engine/pokemon.hpp"
#include "engine/battle.hpp"
#include <random>
// This checks the test setup; battle-rule tests will come later.
TEST(Setup, BasicAssertion) {
    EXPECT_EQ(2 + 2, 4);
}

TEST(Battle, SameSeedProducesSameMoveSequence) {
    Pokemon squirtle;
    squirtle.moves = {{"Water Gun", 35}, {"Tackle", 20}, {"Bite", 25}};
    std::mt19937 firstRng{42};
    std::mt19937 secondRng{42};
    bool sawDifferentMove = false;
    int previousIndex = -1;

    for (int draw = 0; draw < 50; ++draw) {
        const int firstIndex = chooseRandomMove(squirtle, firstRng);
        const int secondIndex = chooseRandomMove(squirtle, secondRng);

        ASSERT_GE(firstIndex, 0);
        ASSERT_LT(firstIndex, static_cast<int>(squirtle.moves.size()));
        EXPECT_EQ(firstIndex, secondIndex) << "Draw " << draw;
        if (draw > 0 && firstIndex != previousIndex) {
            sawDifferentMove = true;
        }
        previousIndex = firstIndex;
    }

    // Catch implementations that always return one index or reseed on every call.
    EXPECT_TRUE(sawDifferentMove);
}

TEST(Battle, RandomMoveHandlesEmptyAndSingleMoveLists) {
    Pokemon squirtle;
    std::mt19937 rng{42};
    EXPECT_EQ(chooseRandomMove(squirtle, rng), -1);

    squirtle.moves = {{"Water Gun", 35}};
    EXPECT_EQ(chooseRandomMove(squirtle, rng), 0);
}


TEST(Battle, GuaranteedAccuracyOutcomesDoNotConsumeRandomness) {
    Pokemon attacker;
    attacker.name = "Squirtle";
    attacker.currentHp = 50;
    attacker.attack = 20;
    Pokemon defender;
    defender.currentHp = 50;
    defender.defense = 20;
    std::mt19937 rng{42};
    const auto initialRng = rng;

    for (int accuracy : {-10, 0}) {
        const Move move{"Water Gun", 30, accuracy};
        const auto result = performAttack(attacker, defender, move, rng);
        EXPECT_FALSE(result.hit);
        EXPECT_EQ(result.damage, 0);
        EXPECT_EQ(result.attackerName, "Squirtle");
        EXPECT_EQ(result.moveName, "Water Gun");
        EXPECT_EQ(defender.currentHp, 50);
        EXPECT_TRUE(rng == initialRng);
    }

    for (int accuracy : {100, 110}) {
        defender.currentHp = 50;
        const Move move{"Water Gun", 30, accuracy};
        const auto result = performAttack(attacker, defender, move, rng);
        EXPECT_TRUE(result.hit);
        EXPECT_EQ(result.damage, 30);
        EXPECT_EQ(defender.currentHp, 20);
        EXPECT_TRUE(rng == initialRng);
    }
}

TEST(Battle, PartialAccuracyIsRepeatableAndUsesTheRoll) {
    Pokemon attacker;
    attacker.currentHp = 50;
    attacker.attack = 20;
    Pokemon firstDefender;
    firstDefender.defense = 20;
    Pokemon secondDefender = firstDefender;
    const Move move{"Water Gun", 30, 50};
    std::mt19937 firstRng{42};
    std::mt19937 secondRng{42};
    std::mt19937 expectedRng{42};
    std::uniform_int_distribution<int> roll(1, 100);
    bool sawHit = false;
    bool sawMiss = false;

    for (int attempt = 0; attempt < 50; ++attempt) {
        firstDefender.currentHp = secondDefender.currentHp = 50;
        const bool expectedHit = roll(expectedRng) <= move.accuracy;
        const auto first = performAttack(attacker, firstDefender, move, firstRng);
        const auto second = performAttack(attacker, secondDefender, move, secondRng);
        EXPECT_EQ(first.hit, expectedHit);
        EXPECT_EQ(first.hit, second.hit);
        EXPECT_EQ(first.damage, expectedHit ? 30 : 0);
        EXPECT_EQ(first.damage, second.damage);
        EXPECT_EQ(firstDefender.currentHp, expectedHit ? 20 : 50);
        EXPECT_EQ(firstDefender.currentHp, secondDefender.currentHp);
        sawHit = sawHit || first.hit;
        sawMiss = sawMiss || !first.hit;
    }
    EXPECT_TRUE(sawHit);
    EXPECT_TRUE(sawMiss);
}

TEST(Battle, MissStillAllowsOpponentToAttack) {
    Pokemon first;
    first.currentHp = 50;
    first.attack = first.defense = 20;
    first.speed = 30;
    Pokemon second = first;
    second.speed = 20;
    const Move miss{"Miss", 100, 0};
    const Move hit{"Tackle", 20, 100};
    std::mt19937 rng{42};

    const auto results = performTurn(first, second, miss, hit, rng);
    ASSERT_EQ(results.size(), 2u);
    EXPECT_FALSE(results[0].hit);
    EXPECT_EQ(results[0].damage, 0);
    EXPECT_TRUE(results[1].hit);
    EXPECT_EQ(first.currentHp, 30);
    EXPECT_EQ(second.currentHp, 50);
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

    std::mt19937 rng{42};
    performAttack(squirtle, pikachu, squirtle.moves[0], rng);

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

    std::mt19937 rng{42};
    performAttack(squirtle, pikachu, squirtle.moves[0], rng);

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

    std::mt19937 rng{42};
    const auto results = performTurn(pikachu, squirtle, pikachu.moves[0], squirtle.moves[0], rng);

    EXPECT_EQ(pikachu.currentHp, 0);
    EXPECT_EQ(squirtle.currentHp, 50);
    ASSERT_EQ(results.size(), 1u);
    EXPECT_EQ(results[0].moveName, "Water Gun");
    EXPECT_EQ(results[0].damage, 65);
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

    std::mt19937 rng{42};
    const auto results = performTurn(pikachu, squirtle, pikachu.moves[0], squirtle.moves[0], rng);

    EXPECT_EQ(pikachu.currentHp, 35);
    EXPECT_EQ(squirtle.currentHp, 10);
    ASSERT_EQ(results.size(), 2u);
    EXPECT_EQ(results[0].moveName, "Water Gun");
    EXPECT_EQ(results[0].damage, 30);
    EXPECT_EQ(results[1].moveName, "Thunder Shock");
    EXPECT_EQ(results[1].damage, 40);
}

TEST(Battle, SpeedTieKnockoutPreventsCounterattack) {
    Pokemon first;
    first.maxHp = first.currentHp = 50;
    first.attack = first.defense = 20;
    first.speed = 30;
    first.moves = {{"Tackle", 50}};
    Pokemon second = first;

    // Repeat the same starting battle with matching seeds.
    Pokemon replayFirst = first;
    Pokemon replaySecond = second;
    std::mt19937 rng{42};
    std::mt19937 replayRng{42};

    performTurn(first, second, first.moves[0], second.moves[0], rng);
    performTurn(replayFirst, replaySecond,
                replayFirst.moves[0], replaySecond.moves[0], replayRng);

    // Exactly one faints; the winner takes no counterattack damage.
    EXPECT_NE(first.isFainted(), second.isFainted());
    EXPECT_EQ(first.currentHp + second.currentHp, 50);
    EXPECT_EQ(first.currentHp, replayFirst.currentHp);
    EXPECT_EQ(second.currentHp, replaySecond.currentHp);
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

TEST(Battle, HigherPriorityOverridesSpeedAndPreventsCounterattack) {
    Pokemon faster;
    faster.name = "Charmander";
    faster.maxHp = faster.currentHp = 50;
    faster.attack = faster.defense = 20;
    faster.speed = 40;
    Pokemon slower = faster;
    slower.name = "Squirtle";
    slower.speed = 20;
    const Move normal{"Scratch", 50, 100, MoveCategory::Physical, 0};
    const Move priority{"Quick Strike", 70, 100, MoveCategory::Physical, 1};

    // Check priority works regardless of which argument holds the slower Pokemon.
    for (bool slowerIsFirst : {false, true}) {
        Pokemon fast = faster;
        Pokemon slow = slower;
        std::mt19937 rng{42};
        const auto initialRng = rng;
        const auto results = slowerIsFirst
            ? performTurn(slow, fast, priority, normal, rng)
            : performTurn(fast, slow, normal, priority, rng);

        EXPECT_TRUE(fast.isFainted());
        EXPECT_EQ(slow.currentHp, 50);
        ASSERT_EQ(results.size(), 1u);
        EXPECT_EQ(results[0].attackerName, "Squirtle");
        EXPECT_EQ(results[0].moveName, "Quick Strike");
        EXPECT_EQ(results[0].damage, 50);
        EXPECT_TRUE(results[0].hit);
        EXPECT_TRUE(rng == initialRng);
    }
}

TEST(Battle, MoveCategorySelectsPhysicalOrSpecialStats) {
    Pokemon attacker;
    attacker.attack = 20;
    attacker.specialAttack = 40;
    Pokemon defender;
    defender.defense = 20;
    defender.specialDefense = 10;
    const Move physical{"Scratch", 30, 100, MoveCategory::Physical};
    const Move special{"Flamethrower", 30, 100, MoveCategory::Special};

    EXPECT_EQ(calculateDamage(attacker, defender, physical), 30);
    EXPECT_EQ(calculateDamage(attacker, defender, special), 120);

    // Physical stats must not affect a special move, or vice versa.
    attacker.attack = 0;
    defender.defense = 100;
    EXPECT_EQ(calculateDamage(attacker, defender, special), 120);
    attacker.attack = 20;
    defender.defense = 20;
    attacker.specialAttack = 0;
    defender.specialDefense = 100;
    EXPECT_EQ(calculateDamage(attacker, defender, physical), 30);
}


