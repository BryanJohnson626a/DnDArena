//
// Created by Bryan on 2/22/2020.
//

#pragma once

#include <vector>
#include <forward_list>

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

enum SizeCategory
{
    NoSize,
    Tiny,
    Small,
    Medium,
    Large,
    Huge,
    Gargantuan,
    InvalidSize
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
class Spell;
class OngoingAction;
class ActionInstance;
class Effect;
class Die;

using Groups = std::vector<Group>;
using ActorPtrs = std::vector<Actor *>;
using ActionList = std::forward_list<ActionInstance>;
