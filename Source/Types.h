//
// Created by Bryan on 2/22/2020.
//

#pragma once

#include <vector>
#include <forward_list>
#include <bitset>

enum DamageType
{
    InvalidDamageType,
    Slashing,
    MagicalSlashing,
    Bludgeoning,
    MagicalBludgeoning,
    Piercing,
    MagicalPiercing,
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
    Untyped
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
    InvalidStat
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

enum PropertyFlag
{
    IsSpell,
    MaxSaveFlags
};

enum Condition
{
    Blinded,
    Charmed,
    Deafened,
    Frightened,
    Grappled,
    Incapacitated,
    Invisible,
    Paralyzed,
    Petrified,
    Poisoned,
    Prone,
    Restrained,
    Stunned,
    Unconscious,
    NoHealing,
    MaxConditions,
    InvalidCondition
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
class RepeatingEffect;
class Die;

using Groups = std::vector<Group>;
using ActorPtrs = std::vector<Actor *>;
using ActionList = std::forward_list<ActionInstance>;
using PropertyField = std::bitset<MaxSaveFlags>;
