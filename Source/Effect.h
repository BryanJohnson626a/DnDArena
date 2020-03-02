//
// Created by bryan on 2/25/20.
//
#pragma once

#include "Types.h"

class Effect
{
public:
    virtual ~Effect() = default;
    virtual void DoEffect(Actor & user, Actor * target, bool critical) const = 0;
};

class EffectHealing : public Effect
{
public:
    void DoEffect(Actor & user, Actor * target, bool critical) const override;

    int HealingDieNum{0};
    Die * HealingDie{nullptr};
    int HealingBonus{0};
    Stat KeyAttribute{None};
    float AddLevelMod{0};
};

class EffectExtraActions : public Effect
{
public:
    void DoEffect(Actor & user, Actor * target, bool critical) const override;

    int ExtraActions{0};
    int ExtraBonusActions{0};
};

class EffectDamage : public Effect
{
public:
    void DoEffect(Actor & user, Actor * target, bool critical) const override;

    int DamageDieNum{0};
    Die * DamageDie{nullptr};
    int DamageBonus{0};
    float DamageMultiplier{1};
    DamageType DamageType;
};

class OngoingEffect : public Effect
{
public:
    // Undoes whatever modifiers the effect granted.
    virtual void End(Actor * effected) const = 0;

    // How many rounds the effect lasts.
    int Duration{0};
};

class OngoingDamageBonus : public OngoingEffect
{
public:
    void DoEffect(Actor & user, Actor * target, bool critical) const override;
    void End(Actor * effected) const override;

    int BonusDamage{0};
};

class OngoingResistance : public OngoingEffect
{
public:
    void DoEffect(Actor & user, Actor * target, bool critical) const override;
    void End(Actor * effected) const override;
    DamageType ResistanceType{InvalidDamageType};
};

