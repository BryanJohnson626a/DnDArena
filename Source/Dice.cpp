//
// Created by bryan on 2/23/20.
//

#include <random>
#include <chrono>

#include "Dice.h"

Die D0(0,0);
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

RNG Die::generator(std::chrono::system_clock::now().time_since_epoch().count());

Die::Die(unsigned high) : Die(1, static_cast<int>(high))
{}

Die::Die(int low, int high) : roller(low, high)
{}

void Die::SetSeed(unsigned seed)
{
    generator = RNG(seed);
}

int Die::operator()()
{
    return roller(generator);
}
