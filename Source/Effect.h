//
// Created by bryan on 2/25/20.
//
#pragma once

#include "Types.h"

class Effect
{
public:
    virtual ~Effect() = default;
    virtual void DoEffect(Actor & user, Actor * target, bool critical, Stat key_stat) const = 0;
    // Undoes whatever modifiers the effect granted.
    virtual void EndEffect(Actor * effected) const = 0;
};

class EffectHealing : public Effect
{
public:
    void DoEffect(Actor & user, Actor * target, bool critical, Stat key_stat) const override;
    void EndEffect(Actor * effected) const override {};

    int HealingDieNum{0};
    Die * HealingDie{nullptr};
    int HealingBonus{0};
    Stat KeyAttribute{None};
    float AddLevelMod{0};
};

class EffectExtraActions : public Effect
{
public:
    void DoEffect(Actor & user, Actor * target, bool critical, Stat key_stat) const override;
    void EndEffect(Actor * effected) const override {};

    int ExtraActions{0};
    int ExtraBonusActions{0};
};

class EffectDamage : public Effect
{
public:
    void DoEffect(Actor & user, Actor * target, bool critical, Stat key_stat) const override;
    void EndEffect(Actor * effected) const override {};

    int DamageDieNum{0};
    Die * DamageDie{nullptr};
    int DamageBonus{0};
    float DamageMultiplier{1};
    DamageType DamageType;
};

class OngoingDamageBonus : public Effect
{
public:
    void DoEffect(Actor & user, Actor * target, bool critical, Stat key_stat) const override;
    void EndEffect(Actor * effected) const override;

    int BonusDamage{0};
};

class OngoingResistance : public Effect
{
public:
    void DoEffect(Actor & user, Actor * target, bool critical, Stat key_stat) const override;
    void EndEffect(Actor * effected) const override;

    DamageType ResistanceType{InvalidDamageType};
};

class UsableAction : public Effect
{
public:
    ~UsableAction() override;
    void DoEffect(Actor & user, Actor * target, bool critical, Stat key_stat) const override;
    void EndEffect(Actor * effected) const override;

    ActionInstance * ActionInst;
    std::string ActionType;
};

