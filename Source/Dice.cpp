//
// Created by bryan on 2/23/20.
//

#include <random>
#include <chrono>
#include <map>

#include "Dice.h"
#include "Output.h"

bool Die::dice_output = false;

Die D0(0, 0);
Die D1(1);
Die D2(2);
Die D3(3);
Die D4(4);
Die D6(6);
Die D8(8);
Die D10(10);
Die D12(12);
Die D20(20);
Die D100(100);

std::map<int, Die *> DieMap = {
        {0,   &D0},
        {1,   &D1},
        {2,   &D2},
        {3,   &D3},
        {4,   &D4},
        {6,   &D6},
        {8,   &D8},
        {10,  &D10},
        {12,  &D12},
        {20,  &D20},
        {100, &D100}
};

RNG Die::generator(std::chrono::system_clock::now().time_since_epoch().count());

Die::Die(unsigned high) : Die(1, static_cast<int>(high))
{}

Die::Die(int low, int high) : roller(low, high), Low(low), High(high)
{}

void Die::SetSeed(unsigned seed)
{
    generator = RNG(seed);
}

Die * Die::Get(int size)
{
    return DieMap.at(size);
}

int Die::Roll()
{
    int result = roller(generator);
    if (dice_output) OUT_ALL << "[" << result << "]";
    return result;
}

int Die::Roll(int times)
{
    int result = 0;
    if (dice_output) OUT_ALL << "[";
    for (int i = 0; i < times; ++i)
    {
        int roll = roller(generator);
        if (i > 0)
            if (dice_output) OUT_ALL << " ";
        if (dice_output) OUT_ALL << roll;
        result += roll;
    }
    if (dice_output) OUT_ALL << "]";
    return result;
}

int Die::Roll(int times, int keep, bool low)
{
    if (times <= keep)
    {
        OUT_ERROR << "Invalid die roll, too many dice kept." << ERROR_END;
        return 0;
    }
    if (dice_output) OUT_ALL << "[";

    // Roll all dice.
    std::vector<int> rolls;
    rolls.reserve(times);
    for (int i = 0; i < times; ++i)
        rolls.emplace_back(roller(generator));

    // Pull out dice to be kept.
    std::vector<int> kept;
    for (int i = 0; i < keep; ++i)
    {
        // Find the highest (or lowest) die left.
        int high_index = 0;
        for (int j = 1; j < rolls.size(); ++j)
        {
            if (rolls[j] > rolls[high_index] != low)
                high_index = j;
        }
        // Move that die to kept dice.
        kept.emplace_back(rolls[high_index]);
        rolls.erase(rolls.begin() + high_index);
    }

    int result = 0;
    for (int i = 0; i < kept.size(); ++i)
    {
        result += kept[i];
        if (i > 0)
            if (dice_output) OUT_ALL << " ";
        if (dice_output) OUT_ALL << kept[i];
    }

    if (dice_output) OUT_ALL << "|";

    for (int i = 0; i < rolls.size(); ++i)
    {
        if (i > 0)
            if (dice_output) OUT_ALL << " ";
        if (dice_output) OUT_ALL << rolls[i];
    }

    if (dice_output) OUT_ALL << "]";
    return result;
}

int Die::RollMod(int bonus, int times, int keep, bool low)
{
    int roll = 0;
    if (times == 1)
        roll = Roll();
    else if (keep == 0)
        roll = Roll(times);
    else
        roll = Roll(times, keep, low);

    if (dice_output && bonus > 0)
        OUT_ALL << "+" << bonus << "=" << roll + bonus;
    return roll + bonus;
}
