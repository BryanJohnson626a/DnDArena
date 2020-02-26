//
// Created by Bryan on 2/21/2020.
//

#pragma once

#include <string>
#include <map>

#include "Types.h"

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
    virtual bool operator()(Actor & user) const = 0;

    Action() = default;
    virtual ~Action() = default;

    explicit Action(std::string name);

    std::string Name;
    std::string Target;

    static Action * Get(const std::string & name);

    static std::map<std::string, Action *> ActionMap;

    ActorPtrs ChooseTargets(Actor & user) const;
};

class WeaponAttack : public Action
{
public:
    WeaponAttack() = default;

    WeaponAttack(std::string name, Stat key_attribute, int number_of_damage_dice, int size_of_damage_dice);

    virtual bool operator()(Actor & user) const;

    Stat KeyAttribute;
    int DamageDiceNum;
    Die * DamageDie;
};

extern WeaponAttack UnarmedStrike;

class MultiAction : public Action
{
public:
    MultiAction() = default;

    bool operator()(Actor & user) const override;

    std::vector<Action *> Actions;
};

class SpecialAction : public Action
{
public:
    SpecialAction() = default;

    // Special actions are responsible for cleaning up their effects.
    ~SpecialAction() override;

    bool operator()(Actor & user) const override;

    std::vector<Effect *> Effects;
};
