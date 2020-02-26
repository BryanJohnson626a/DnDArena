//
// Created by bryan on 2/25/20.
//
#pragma once

#include "Types.h"


class Effect
{
public:
    virtual ~Effect() = default;
    virtual void operator()(Actor & user, std::vector<Actor*> & targets) const = 0;
};

class EffectHealing : public Effect
{
public:
    void operator()(Actor & user, std::vector<Actor*> & targets) const override ;

    int HealingDieNum;
    Die * HealingDie;
    int HealingBonus;
    Stat KeyAttribute;
    float AddLevelMod;
};

class EffectImmediateExtraActions : public Effect
{
public:
    void operator()(Actor & user, std::vector<Actor*> & targets) const override ;

    int ExtraActions;
    int ExtraBonusActions;
};

