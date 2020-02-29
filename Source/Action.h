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
    virtual ~Action() = default;

    static std::shared_ptr<const Action> Get(const std::string & name);
    ActorPtrs ChooseTargets(Actor & user) const;

    std::string Name;
    std::string Target;

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
    WeaponAttack() = default;

    bool DoAction(Actor & user, Stat KeyStat) const override;

    int DamageDiceNum{0};
    Die * DamageDie{0};
    DamageType DamageType;
};

class MultiAction : public Action
{
public:
    MultiAction() = default;

    bool DoAction(Actor & user, Stat KeyStat) const override;

    std::vector<std::shared_ptr<const Action>> Actions;
};

class SpecialAction : public Action
{
public:
    SpecialAction() = default;

    // Special actions are responsible for cleaning up their effects.
    ~SpecialAction() override;

    bool DoAction(Actor & user, Stat KeyStat) const override;

    std::vector<Effect *> Effects;
};

class Spell : public Action
{
public:
    Spell() = default;

    // Spells are responsible for cleaning up their effects.
    ~Spell() override;

    bool DoAction(Actor & user, Stat KeyStat) const override;

    Stat SavingThrow{None};
    bool SpellAttack{false};
    std::vector<Effect *> HitEffects;
    std::vector<Effect *> MissEffects;
};
