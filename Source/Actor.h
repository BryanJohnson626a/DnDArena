//
// Created by Bryan on 2/21/2020.
//

#pragma once

#include <vector>
#include <string>

#include "Types.h"

class Action;
class Die;

struct WeightedAction
{
    float Weight;
    const Action & Action;
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

    int HDNum;
    Die & HD;

    int Proficiency;

    int InitiativeBonus;

    // AC Bonus
    int ArmorValue, ShieldValue;

    // Bonuses from magic items/effects
    int AttackBonus, DamageBonus;

    // Stat bonus
    int STR, DEX, CON, INT, WIS, CHA;

    // Save proficiency
    bool SaveSTR, SaveDEX, SaveCON,
            SaveINT, SaveWIS, SaveCHA;

    // Derived stats
    int InitiativeMod, AC,
            STRSaveMod, DEXSaveMod, CONSaveMod,
            INTSaveMod, WISSaveMod, CHASaveMod;

    std::vector<WeightedAction> Actions;
    float ActionsTotalWeight;
    std::vector<WeightedAction> BonusActions;
    float BonusActionsTotalWeight;

    std::vector<std::string> Immunities;
    std::vector<std::string> Resistances;
    std::vector<std::string> Weaknesses;
    std::vector<std::string> ConditionImmunities;

    void AddAction(Action action, float weight);
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

    Actor(std::string name, const StatBlock & stat_block, int team, std::ostream & out);

    void Initialize();
    void ResetInfo();
    const Statistics & Info();
    [[nodiscard]] const Action & ChooseAction() const;
    void TakeAction(Arena & arena);
    void TakeDamage(int damage);
    void DeathSave();
    const StatBlock & Stats;

    std::string Name;
    int Initiative;
    int Team;

    DeathState GetDeathState() const;
    bool Alive() const;
    bool Conscious() const;
    int CurrentHP() const;
    int SuccessfulDeathSaves, FailedDeathSaves;

    Statistics InfoStats;
private:
    int HP, MaxHP;
    DeathState State;

    std::ostream & out;

    void DeathCheck();
};
