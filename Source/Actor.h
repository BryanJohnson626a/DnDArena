//
// Created by Bryan on 2/21/2020.
//

#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <forward_list>

#include "Types.h"

// Instance of an action for a Stat Block.
// Uses and Key Stat can be different for
// different creatures using the same action.
class ActionInstance
{
public:
    std::shared_ptr<const Action> Action;
    int Uses;
    Stat KeyStat{None};
};

class RiderEffect
{
public:
    Effect * Effect;
    int Uses;
};

/**
 * Holds the base, unchanging stats of an actor.
 */
struct StatBlock
{

    void CalculateDerivedStats();

    std::string Name;
    bool Heroic;
    SizeCategory Size;
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
    std::vector<ActionInstance> BonusActions;
    std::vector<RiderEffect> HitRiders;

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
        long long Kills; // Times this actor has killed another actor.
        long long KOs; // Times this actor has knocked another actor unconscious.
        long long Deaths; // Times this actor has died.
        long long KOed; // Times this actor has been knocked unconscious
        long long DamageDone; // How much damage this actor has caused.
        long long DamageTaken; // How much damage has been caused to this actor.
        long long AttacksLanded; // How many weapon or spell attacks this actor has hit with.
        long long AttacksMissed; // How many weapon or spell attacks this actor has missed with.
        long long AttacksReceived; // How many weapon or spell attacks this actor has been hit by.
        long long AttacksAvoided; // How many weapon or spell attacks this actor has been missed by.
        long long CritsLanded; // How many critical weapon or spell attacks this actor has made.
        long long CritsReceived; // How many critical weapon or spell attacks have been made against this actor.
        long long ForcedSavesFailed; // How many of the saving throws that this actor forced another actor to make were failures.
        long long ForcedSavesMade; // How many of the saving throws that this actor forced another actor to make were successful.
        long long SavesFailed; // How many saving throws made by this actor wew failures.
        long long SavesMade; // How many saving throws made by this actor were successful.
    };

    Actor(std::string_view name, std::shared_ptr<const StatBlock> stat_block, int team, Arena & arena);

    ~Actor();

    void Initialize();

    void ResetInfo();

    void TakeTurn();

    bool TakeAction();

    bool TakeBonusAction();

    int TakeDamage(int damage, DamageType damage_type, Actor & damager);

    void DeathSave();

    [[nodiscard]] ActionInstance * ChooseAction(ActionList & actions) const;
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
    [[nodiscard]] int GetSave(Stat stat);
    [[nodiscard]] bool CanConcentrate() const;

    void AddResistance(DamageType damage_type);
    void RemoveResistance(DamageType damage_type);

    bool AddOngoingAction(const Action * action, ActorPtrs hit_targets,
                          ActorPtrs missed_targets, bool concentration = false);

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
    ActionList ActionQueue;
    ActionList BonusActionQueue;
    std::vector<RiderEffect> HitRiders;
    int SuccessfulDeathSaves{0}, FailedDeathSaves{0};
    Statistics InfoStats{};
    Arena & CurrentArena;
    int TempDamageBonus = 0;
    OngoingAction * ConcentrationSpell{nullptr};
private:
    int TempResistance[DamageTypesMax]{0};
    std::vector<OngoingAction *> OngoingActions;
    int HP{}, HPMax{};
    DeathState State{DeathState::Conscious};

    void DeathCheck();

};
