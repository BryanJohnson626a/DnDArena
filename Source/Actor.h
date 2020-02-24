//
// Created by Bryan on 2/21/2020.
//

#pragma once

#include <vector>
#include <string>

#include "Types.h"

class Action;

class Die;

/**
 * Holds the base, unchanging stats of an actor.
 */
struct StatBlock
{
    void CalculateDerivedStats();

    struct WeightedAction
    {
        float Weight;
        const Action & Action;
    };

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
    std::vector<WeightedAction> BonusActions;

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
        int AttacksMade;
        int AttacksMissed;
        int AttacksReceived;
        int AttacksAvoided;
        int ForcedSaves;
        int ForcedSavesMade;
        int SavesDone;
        int SavesMade;
    };

    Actor(std::string Name, const StatBlock & StatBlock, int Team, std::ostream & out);

    void Initialize();
    void ResetInfo();
    const Statistics & Info();
    void TakeAction(Arena & arena);
    void TakeDamage(int damage);
    const StatBlock & Stats;

    std::string Name;
    int Initiative;
    int Team;

    bool Alive() const;
    int CurrentHP() const;

private:
    int HP, MaxHP;

    Statistics InfoStats;

    std::ostream & out;
};
