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
    Die(unsigned high);

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
     * Rolls the die.
     * @return The number rolled on the die.
     */
    int operator()();

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
