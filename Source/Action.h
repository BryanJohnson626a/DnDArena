//
// Created by Bryan on 2/21/2020.
//

#pragma once

#include <string>
#include <map>
#include <memory>

#include "Types.h"

class Action
{
public:
    virtual bool DoAction(Actor & user, Stat KeyStat) const = 0;

    Action() = default;
    virtual ~Action();

    static std::shared_ptr<const Action> Get(const std::string & name);
    ActorPtrs ChooseTargets(Actor & user) const;

    std::string Name;
    std::string Target;
    int Area{1};
    int Duration{0};
    std::vector<Effect *> HitEffects;
    std::vector<Effect *> MissEffects;
    PropertyField Properties;

    static std::map<std::string, std::weak_ptr<const Action>> ActionMap;
    static std::shared_ptr<const Action> GetWeaponAttackProxy();
};

class AttackProxy : public Action
{
public:
    AttackProxy() = default;
    bool DoAction(Actor & user, Stat KeyStat) const override;
};

class WeaponAttack : public Action
{
public:
    bool DoAction(Actor & user, Stat KeyStat) const override;

    int DamageDiceNum{0};
    Die * DamageDie{0};
    int DamageBonus{0};
    int HitBonus{0};
    DamageType DamageType{InvalidDamageType};
};

class MultiAction : public Action
{
public:
    bool DoAction(Actor & user, Stat KeyStat) const override;

    std::vector<std::shared_ptr<const Action>> Actions;
};

class SpecialAction : public Action
{
public:
    bool DoAction(Actor & user, Stat KeyStat) const override;
};

class Spell : public Action
{
public:
    bool DoAction(Actor & user, Stat KeyStat) const override;

    Stat SavingThrow{None};
    bool SpellAttack{false};
    bool Concentration{false};
};

// A ongoing action is created for actions that have a duration
// in order to manage their ongoing effects.
class OngoingAction
{
public:
    ~OngoingAction();
    const Action * Data;
    int DurationRemaining;
    ActorPtrs HitTargets;
    ActorPtrs MissedTargets;
};
