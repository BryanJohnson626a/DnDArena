//
// Created by Bryan on 2/21/2020.
//

#pragma once

#include <vector>
#include <string>

#include "Types.h"

class Action;

class Die;

struct ActionInstance
{
    const Action * Action;
    int Uses;
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
    struct ActionRep
    {
        const Action * Action;
        int Uses;
    };

    Actor(std::string name, const StatBlock & stat_block, int team, Arena & arena);

    void Initialize();

    void ResetInfo();

    void DoRound();

    void TakeAction();

    void TakeBonusAction();

    void TakeDamage(int damage);

    void DeathSave();

    [[nodiscard]] int ChooseAction(const std::vector<ActionRep> & actions) const;

    [[nodiscard]] DeathState GetDeathState() const;

    [[nodiscard]] bool Alive() const;

    [[nodiscard]] bool Conscious() const;

    [[nodiscard]] int CurrentHP() const;

    [[nodiscard]] bool IsInjured() const;

    [[nodiscard]] const Statistics & Info();

    [[nodiscard]] int GetStatMod(enum Stat stat) const;

    std::string Name;
    const StatBlock & Stats;
    int Initiative;
    int Team;
    std::vector<ActionRep> ActionQueue;
    std::vector<ActionRep> BonusActionQueue;
    int SuccessfulDeathSaves, FailedDeathSaves;
    Statistics InfoStats;
    Arena & CurrentArena;

    /**
     * Retores health to the actor, limited by max HP.
     * @param amount How much to heal by.
     * @return The actual amount healed.
     */
    int Heal(int amount);

private:
    int HP, MaxHP;
    DeathState State;

    void DeathCheck();

    void FillActionQueues();
};
