//
// Created by bryan on 2/23/20.
//

#include <cstring>
#include "Arena.h"
#include "Actor.h"
#include "Action.h"
#include "Dice.h"
#include "Output.h"


Action::Action(std::string name, Stat KeyAttribute, ActionType Type, int NumDamageDice, Die & DamageDie) :
        Name(name), KeyAttribute(KeyAttribute), Type(Type), NumDamageDice(NumDamageDice), DamageDie(DamageDie)
{}


void StatBlock::CalculateDerivedStats()
{
    InitiativeMod = DEX + Proficiency + InitiativeBonus;
    AC = 10 + DEX + ArmorValue + ShieldValue;

    STRSaveMod = STR + (SaveSTR ? Proficiency : 0);
    DEXSaveMod = DEX + (SaveDEX ? Proficiency : 0);
    CONSaveMod = CON + (SaveCON ? Proficiency : 0);
    INTSaveMod = INT + (SaveINT ? Proficiency : 0);
    WISSaveMod = WIS + (SaveWIS ? Proficiency : 0);
    CHASaveMod = CHA + (SaveCHA ? Proficiency : 0);

    Actions.emplace_back(WeightedAction{0, Action::UnarmedStrike});
}

void StatBlock::AddAction(Action action, float weight)
{
    std::pow(weight, 2);
    Actions.emplace_back(WeightedAction{weight, action});
    ActionsTotalWeight += weight;
}

void Actor::Initialize()
{
    Initiative = D20() + Stats.DEX + Stats.InitiativeBonus;

    SuccessfulDeathSaves = 0;
    FailedDeathSaves = 0;
    State = DeathState::Conscious;

    MaxHP = 0;
    for (int i = 0; i < Stats.HDNum; ++i)
        MaxHP += Stats.HD() + Stats.CON;

    HP = MaxHP;
}

Actor::Actor(std::string name, const StatBlock & stat_block, int team, std::ostream & out) :
        Stats(stat_block), Team(team), Name(name), out(out)
{
    Initialize();
    ResetInfo();
}

const Actor::Statistics & Actor::Info()
{
    return InfoStats;
}

void Actor::ResetInfo()
{
    std::memset(&InfoStats, 0, sizeof(Statistics));
}

bool Actor::Alive() const
{
    return State != DeathState::Dead;
}

bool Actor::Conscious() const
{
    return State == DeathState::Conscious;
}

void Actor::TakeAction(Arena & arena)
{
    if (Conscious())
    {
        const Action & action = ChooseAction();
        Actor & target = arena.OtherGroup(Team).FirstConscious();

        if (OUTPUT_LEVEL > 1) out << Name << " uses " << action << " on " << target << ". ";

        action(*this, target, out);
    } else if (Alive())
        DeathSave();
}

void Actor::TakeDamage(int damage)
{
    if (HP <= 0)
    {
        FailedDeathSaves += 2;
        DeathCheck();
    } else
    {
        HP -= damage;
        InfoStats.DamageTaken += damage;
        if (HP <= 0)
        {
            HP = 0;
            State = DeathState::Dying;
            if (OUTPUT_LEVEL > 1) out << Name << " has fallen!" << std::endl;
        }
    }
}

int Actor::CurrentHP() const
{
    return HP;
}

const Action & Actor::ChooseAction() const
{
    float distance = D100() * Stats.ActionsTotalWeight / 100;
    for (const WeightedAction & action : Stats.Actions)
    {
        distance -= action.Weight;
        if (distance < 0)
            return action.Action;
    }
    return Stats.Actions[0].Action;
}

void Actor::DeathSave()
{
    if (SuccessfulDeathSaves < 3 && FailedDeathSaves < 3)
    {
        int roll = D20();
        if (OUTPUT_LEVEL > 1) out << Name << " makes a death saving throw: " << roll;
        if (roll == 20)
        {
            // Miracluous Recovery.
            HP = 1;
            SuccessfulDeathSaves = 0;
            FailedDeathSaves = 0;
            State = DeathState::Conscious;
            if (OUTPUT_LEVEL > 1) out << " Critical Success!" << std::endl;
            if (OUTPUT_LEVEL > 1) out << Name << " recovers!" << std::endl;
            return;
        } else if (roll >= 10)
        {
            if (OUTPUT_LEVEL > 1) out << " Success." << std::endl;
            ++SuccessfulDeathSaves;
        } else if (roll > 1)
        {
            if (OUTPUT_LEVEL > 1) out << " Fail." << std::endl;
            ++FailedDeathSaves;
        }
        else
        {
            if (OUTPUT_LEVEL > 1) out << " Critical fail!" << std::endl;
            FailedDeathSaves +=2;
        }
    }
    DeathCheck();
}

void Actor::DeathCheck()
{
    if (FailedDeathSaves >= 3)
    {
        // Dead
        State = DeathState::Dead;
        InfoStats.Deaths++;
        if (OUTPUT_LEVEL > 1) out << Name << " died!" << std::endl;
    } else if (State == DeathState::Dying && SuccessfulDeathSaves >= 3)
    {
        // Stable
        State = DeathState::Stable;
        if (OUTPUT_LEVEL > 1) out << Name << " has stabilized." << std::endl;
    }
}

DeathState Actor::GetDeathState() const
{
    return State;
}

