//
// Created by Bryan on 2/22/2020.
//

#pragma once

#include <vector>

enum DamageType
{
    Slashing,
    Bludgeoning,
    Piercing,
    Fire,
    Cold,
    Lightning,
    Thunder,
    Acid,
    Poison,
    Necrotic,
    Radiant,
    Psychic,
    Force,
    DamageTypesMax,
    InvalidDamageType,
};

enum Stat
{
    None,
    Strength,
    Dexterity,
    Constitution,
    Intelligence,
    Wisdom,
    Charisma,
};

enum DeathState
{
    Conscious,
    Stable,
    Dying,
    Dead,
};

class StatBlock;
class Arena;
class Group;
class Actor;
class Action;
class Effect;
class OngoingEffect;
class Die;

using Groups = std::vector<Group>;
using ActorPtrs = std::vector<Actor *>;
