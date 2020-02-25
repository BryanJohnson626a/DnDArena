//
// Created by Bryan on 2/21/2020.
//

#pragma once

#include <string>
#include <map>

#include "Types.h"

class Die;

class Action
{
public:
    /**
     * Function that implements the usage of an Action. Must be implemented.
     * @param user The Actor using the Action.
     * @param arena The Arena in which the Action is being used.
     * @param out The output stream for writing debug info to.
     * @return True if the Action was used, False if the Action could not be used.
     */
    virtual bool operator()(Actor & user, Arena & arena) const = 0;

    Action() = default;;

    explicit Action(std::string name);

    std::string Name;

    static Action * Get(std::string name);

    static std::map<std::string, Action *> ActionMap;
};

class WeaponAttack : public Action
{
public:
    WeaponAttack() = default;;

    WeaponAttack(std::string name, Stat key_attribute, int number_of_damage_dice, int size_of_damage_dice);

    virtual bool operator()(Actor & user, Arena & arena) const;

    Stat KeyAttribute;
    int DamageDiceNum;
    Die * DamageDie;
};

extern WeaponAttack UnarmedStrike;

class MultiAction : public Action
{
public:
    MultiAction() = default;;

    virtual bool operator()(Actor & user, Arena & arena) const;

    std::vector<Action *> Actions;
};
