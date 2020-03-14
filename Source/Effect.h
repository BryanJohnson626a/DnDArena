//
// Created by bryan on 2/25/20.
//
#pragma once

#include "Types.h"

class Effect
{
public:
    virtual ~Effect() = default;
    virtual bool CanEffect(const Actor & user, const Actor * target) const = 0;
    virtual bool DoEffect(Actor & user, Actor * target, bool critical, Stat key_stat) const = 0;
    // Undoes whatever modifiers the effect granted.
    virtual void EndEffect(Actor * effected) const = 0;
};

class EffectHealing : public Effect
{
public:
    bool CanEffect(const Actor & user, const Actor * target) const override;
    bool DoEffect(Actor & user, Actor * target, bool critical, Stat key_stat) const override;
    void EndEffect(Actor * effected) const override;

    int HealingDieNum{0};
    Die * HealingDie{nullptr};
    int HealingBonus{0};
    Stat KeyAttribute{None};
    float AddLevelMod{0};
};

class EffectExtraActions : public Effect
{
public:
    bool CanEffect(const Actor & user, const Actor * target) const override;
    bool DoEffect(Actor & user, Actor * target, bool critical, Stat key_stat) const override;
    void EndEffect(Actor * effected) const override;

    int ExtraActions{0};
    int ExtraBonusActions{0};
};

class EffectDamage : public Effect
{
public:
    bool CanEffect(const Actor & user, const Actor * target) const override;
    bool DoEffect(Actor & user, Actor * target, bool critical, Stat key_stat) const override;
    void EndEffect(Actor * effected) const override;

    int DamageDieNum{0};
    Die * DamageDie{nullptr};
    int DamageBonus{0};
    float DamageMultiplier{1};
    DamageType DamageType;
};

class DamageBonusEffect : public Effect
{
public:
    bool CanEffect(const Actor & user, const Actor * target) const override;
    bool DoEffect(Actor & user, Actor * target, bool critical, Stat key_stat) const override;
    void EndEffect(Actor * effected) const override;

    int BonusDamage{0};
};

class ResistanceEffect : public Effect
{
public:
    bool CanEffect(const Actor & user, const Actor * target) const override;
    bool DoEffect(Actor & user, Actor * target, bool critical, Stat key_stat) const override;
    void EndEffect(Actor * effected) const override;

    DamageType ResistanceType{InvalidDamageType};
};

class UsableAction : public Effect
{
public:
    ~UsableAction() override;
    bool CanEffect(const Actor & user, const Actor * target) const override;
    bool DoEffect(Actor & user, Actor * target, bool critical, Stat key_stat) const override;
    void EndEffect(Actor * effected) const override;

    ActionInstance * ActionInst;
    std::string ActionType;

};

class SaveEffect : public Effect
{
public:
    ~SaveEffect() override;
    bool CanEffect(const Actor & user, const Actor * target) const override;
    bool DoEffect(Actor & user, Actor * target, bool critical, Stat key_stat) const override;
    void EndEffect(Actor * effected) const override;

    int DC{0};
    Stat SavingThrow{None};
    std::vector<Effect *> HitEffects;
    std::vector<Effect *> MissEffects;
};

class ConditionEffect : public Effect
{
public:
    bool CanEffect(const Actor & user, const Actor * target) const override;
    bool DoEffect(Actor & user, Actor * target, bool critical, Stat key_stat) const override;
    void EndEffect(Actor * effected) const override;

    Condition InflictedCondition;
};

class RepeatingEffect : public Effect
{
public:
    bool CanEffect(const Actor & user, const Actor * target) const override;
    bool DoEffect(Actor & user, Actor * target, bool critical, Stat key_stat) const override;
    void EndEffect(Actor * effected) const override;

    Actor * Instigator{nullptr};
    int Duration{0};
    int DC{0};
    Stat SavingThrow{None};
    std::string Timing;
    // These effects happen once and then are undone when the effect ends.
    std::vector<Effect *> DurationEffects;
    // These effects happen every turn and aren't undone.
    std::vector<Effect *> RepeatingEffects;
};

