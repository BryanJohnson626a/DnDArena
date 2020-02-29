//
// Created by Bryan on 2/21/2020.
//

#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>

#include "Types.h"

class Action;

class Die;

// Instance of an action for a Stat Block.
// Uses and Key Stat can be different for
// different creatures using the same action.
struct ActionInstance
{
    std::shared_ptr<const Action> Action;
    int Uses;
    Stat KeyStat{None};
};

/**
 * Holds the base, unchanging stats of an actor.
 */
struct StatBlock
{

    void CalculateDerivedStats();

    std::string Name;
    std::string Type;

    // Intrinsic Stats

    int HDNum, HDSize;

    int Proficiency;

    int InitiativeBonus;
    int Crit;

    // AC Bonus
    int ArmorValue, ShieldValue;

    // Bonuses from magic items/effects
    int AttackBonus, DamageBonus;

    // Stat score
    int Strength, Dexterity, Constitution, Intelligence, Wisdom, Charisma;

    // Save proficiency
    bool SaveSTR, SaveDEX, SaveCON,
            SaveINT, SaveWIS, SaveCHA;

    // Derived stats

    // Stat Modifier
    int STR, DEX, CON, INT, WIS, CHA;

    // Die used for rolling hit points.
    Die * HD;

    int InitiativeMod, AC,
            STRSaveMod, DEXSaveMod, CONSaveMod,
            INTSaveMod, WISSaveMod, CHASaveMod;

    std::vector<ActionInstance> Actions;
    float ActionsTotalWeight;
    std::vector<ActionInstance> BonusActions;
    float BonusActionsTotalWeight;


    static std::shared_ptr<const StatBlock> Get(const std::string_view & name);

    static std::map<std::string_view, std::weak_ptr<const StatBlock>> StatBlockMap;
};

/**
 * An individual instance of an actor.
 */
class Actor
{
public:
    struct Statistics
    {
        int Kills;
        int Deaths;
        int DamageDone;
        int DamageTaken;
        int AttacksLanded;
        int AttacksMissed;
        int AttacksReceived;
        int AttacksAvoided;
        int CritsLanded;
        int CritsReceived;
        int ForcedSaves;
        int ForcedSavesMade;
        int SavesDone;
        int SavesMade;
    };
    // Action representative that tracks active usage.
    struct ActionRep
    {
        const ActionInstance & Inst;
        int UsesRemaining;
    };
    struct EffectRep
    {
        const OngoingEffect * effect;
        int duration_remaining;
    };

    Actor(std::string_view name, std::shared_ptr<const StatBlock> stat_block, int team, Arena & arena);

    void Initialize();

    void ResetInfo();

    void DoRound();

    void TakeAction();

    void TakeBonusAction();

    int TakeDamage(int damage, DamageType damage_type);

    void DeathSave();

    [[nodiscard]] ActionRep * ChooseAction(std::vector<ActionRep> & actions) const;

    [[nodiscard]] DeathState GetDeathState() const;

    [[nodiscard]] bool Alive() const;

    [[nodiscard]] bool Conscious() const;

    [[nodiscard]] int CurrentHP() const;

    [[nodiscard]] int MaxHP() const;

    [[nodiscard]] bool IsInjured() const;

    [[nodiscard]] const Statistics & Info();

    [[nodiscard]] int GetStatMod(enum Stat stat) const;

    [[nodiscard]] int GetDamageBonus() const;

    [[nodiscard]] bool HasResistance(DamageType damage_type) const;

    void AddResistance(DamageType damage_type);

    void RemoveResistance(DamageType damage_type);

    [[nodiscard]]int GetSave(Stat stat);

    void AddEffect(const OngoingEffect * ongoing_effect);

    /**
     * Restores health to the actor, limited by max HP.
     * @param amount How much to heal by.
     * @return The actual amount healed.
     */
    int Heal(int amount);

    std::string Name;
    std::shared_ptr<const StatBlock> Stats;
    int Initiative{0};
    int Team{0};
    std::vector<ActionRep> ActionQueue;
    std::vector<ActionRep> BonusActionQueue;
    int SuccessfulDeathSaves{0}, FailedDeathSaves{0};
    Statistics InfoStats{};
    Arena & CurrentArena;
    int TempDamageBonus = 0;
private:
    int TempResistance[DamageTypesMax]{0};
    std::vector<EffectRep> OngoingEffects;
    int HP{}, HPMax{};
    DeathState State{DeathState::Conscious};

    void DeathCheck();

    void FillActionQueues();
};
