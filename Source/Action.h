//
// Created by Bryan on 2/21/2020.
//

#pragma once

#include <string>

#include "Types.h"

class Die;

class Action
{
public:
    Action(std::string name, Stat KeyAttribute, ActionType Type, int NumDamageDice, Die & DamageDie);

    static Action UnarmedStrike;

    void operator()(Actor & user, Actor & target, std::ostream & out) const;

    std::string Name;
private:
    Stat KeyAttribute;
    ActionType Type;
    int NumDamageDice;
    Die & DamageDie;

    void Attack(Actor & user, Actor & target, std::ostream & out) const;
};
