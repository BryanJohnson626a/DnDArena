//
// Created by bryan on 2/23/20.
//

#pragma once

#include <random>

using RNG = std::mt19937_64;

class Die
{
public:
    /**
     * Creates a die that rolls between 1 and high.
     * @param high The highest result possible.
     */
    explicit Die(unsigned high);

    /**
     * Creates a die that rolls between two values.
     * @param low The lowest result possible.
     * @param high The highest result possible.
     */
    Die(int low, int high);

    /**
     * Sets the seed used by all dice.
     * @param seed
     */
    static void SetSeed(unsigned seed);

    /**
     * Roll the die once.
     * @return The result of the die roll.
     */
    [[nodiscard]] int Roll();

    /**
     * Roll the die multiple times.
     * @param times How many times to roll.
     * @return The total of all dice rolled.
     */
    [[nodiscard]] int Roll(int times);

    /**
     * Roll the die multiple times and keep only some.
     * @param times How many times to roll.
     * @param keep How many dice to keep at the end.
     * @param low If true, keeps the lowest dice instead of the highest.
     * @return The total of all dice rolled that were kept.
     */
    [[nodiscard]] int Roll(int times, int keep, bool low = false);

    [[nodiscard]] int RollMod(int bonus, int times = 1, int keep = 0, bool low = false);

    [[nodiscard]] static Die * Get(int size);

    const int Low, High;

    // When true, dice output their results
    static bool dice_output;
private:
    static RNG generator;
    std::uniform_int_distribution<int> roller;
};


extern Die D0;
extern Die D1;
extern Die D2;
extern Die D3;
extern Die D4;
extern Die D6;
extern Die D8;
extern Die D10;
extern Die D12;
extern Die D20;
extern Die D100;
