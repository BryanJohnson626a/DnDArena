//
// Created by bryan on 2/23/20.
//

#include <cstring>
#include "Arena.h"
#include "Actor.h"
#include "Action.h"
#include "Dice.h"
#include "Output.h"


void StatBlock::CalculateDerivedStats()
{
    STR = std::floor((Strength - 10) / 2.0);
    DEX = std::floor((Dexterity - 10) / 2.0);
    CON = std::floor((Constitution - 10) / 2.0);
    INT = std::floor((Intelligence - 10) / 2.0);
    WIS = std::floor((Wisdom - 10) / 2.0);
    CHA = std::floor((Charisma - 10) / 2.0);

    HD = Die::Get(HDSize);

    InitiativeMod = DEX + Proficiency + InitiativeBonus;
    AC = 10 + DEX + ArmorValue + ShieldValue;

    STRSaveMod = STR + (SaveSTR ? Proficiency : 0);
    DEXSaveMod = DEX + (SaveDEX ? Proficiency : 0);
    CONSaveMod = CON + (SaveCON ? Proficiency : 0);
    INTSaveMod = INT + (SaveINT ? Proficiency : 0);
    WISSaveMod = WIS + (SaveWIS ? Proficiency : 0);
    CHASaveMod = CHA + (SaveCHA ? Proficiency : 0);

    Actions.emplace_back(ActionInstance{&UnarmedStrike, -1});
}

void Actor::Initialize()
{
    Initiative = D20() + Stats.DEX + Stats.InitiativeBonus;

    SuccessfulDeathSaves = 0;
    FailedDeathSaves = 0;
    State = DeathState::Conscious;

    MaxHP = 0;
    for (int i = 0; i < Stats.HDNum; ++i)
        MaxHP += (*Stats.HD)() + Stats.CON;

    HP = MaxHP;

}

void Actor::FillActionQueues()
{
    ActionQueue.clear();
    for (const ActionInstance & weighted_action : Stats.Actions)
        ActionQueue.push_back(ActionRep{weighted_action.Action, weighted_action.Uses});

    BonusActionQueue.clear();
    for (const ActionInstance & weighted_action : Stats.BonusActions)
        BonusActionQueue.push_back(ActionRep{weighted_action.Action, weighted_action.Uses});
}

Actor::Actor(std::string name, const StatBlock & stat_block, int team) :
        Stats(stat_block), Team(team), Name(name)
{
    FillActionQueues();
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

void Actor::DoRound(Arena & arena)
{
    if (Conscious())
    {
        TakeAction(arena);
        TakeBonusAction(arena);
    }
    else if (Alive())
        DeathSave();
}

void Actor::TakeAction(Arena & arena)
{
    int action_index = ChooseAction(ActionQueue);
    if (action_index != -1)
    {
        bool used = (*ActionQueue[action_index].Action)(*this, arena);
        if (used)
            --ActionQueue[action_index].Uses;
    }
}

void Actor::TakeBonusAction(Arena & arena)
{

    int action_index = ChooseAction(BonusActionQueue);
    if (action_index != -1)
    {
        bool used = (*BonusActionQueue[action_index].Action)(*this, arena);
        if (used)
            --BonusActionQueue[action_index].Uses;
    }
}

int Actor::ChooseAction(const std::vector<ActionRep> & actions) const
{
    for (int i = 0; i < actions.size(); ++i)
        if (actions[i].Uses != 0)
            return i;

    return -1;
}

void Actor::TakeDamage(int damage)
{
    if (HP <= 0)
    {
        FailedDeathSaves += 2;
        DeathCheck();
    }
    else
    {
        HP -= damage;
        InfoStats.DamageTaken += damage;
        if (HP <= 0)
        {
            HP = 0;
            State = DeathState::Dying;
            if (Out(AllActions)) Out.O() << "        " << Name << " has fallen!" << std::endl;
        }
    }
}

int Actor::CurrentHP() const
{
    return HP;
}

void Actor::DeathSave()
{
    if (SuccessfulDeathSaves < 3 && FailedDeathSaves < 3)
    {
        int roll = D20();
        if (Out(AllActions)) Out.O() << "    " << Name << " makes a death saving throw: " << roll;
        if (roll == 20)
        {
            // Miracluous Recovery.
            HP = 1;
            SuccessfulDeathSaves = 0;
            FailedDeathSaves = 0;
            State = DeathState::Conscious;
            if (Out(AllActions)) Out.O() << " Critical Success!" << std::endl;
            if (Out(AllActions)) Out.O() << Name << " recovers!" << std::endl;
            return;
        }
        else if (roll >= 10)
        {
            if (Out(AllActions)) Out.O() << " Success." << std::endl;
            ++SuccessfulDeathSaves;
        }
        else if (roll > 1)
        {
            if (Out(AllActions)) Out.O() << " Fail." << std::endl;
            ++FailedDeathSaves;
        }
        else
        {
            if (Out(AllActions)) Out.O() << " Critical fail!" << std::endl;
            FailedDeathSaves += 2;
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
        if (Out(AllActions)) Out.O() << "    " << Name << " died!" << std::endl;
    }
    else if (State == DeathState::Dying && SuccessfulDeathSaves >= 3)
    {
        // Stable
        State = DeathState::Stable;
        if (Out(AllActions)) Out.O() << "    " << Name << " has stabilized." << std::endl;
    }
}

DeathState Actor::GetDeathState() const
{
    return State;
}
