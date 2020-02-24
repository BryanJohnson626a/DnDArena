//
// Created by Bryan on 2/22/2020.
//

#pragma once

#include <vector>

enum Stat
{
    Strength,
    Dexterity,
    Constitution,
    Intelligence,
    Wisdom,
    Charisma
};

enum ActionType
{
    MeleeAttack,
    RangedAttack,
    Spell,
    Special
};

class Arena;
class Group;
class Actor;
class Action;

using Groups = std::vector<Group>;
using ActorQueue = std::vector<Actor *>;
