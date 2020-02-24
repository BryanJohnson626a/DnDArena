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
    AttackBonus = STR + Proficiency;
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
    Actions.emplace_back(WeightedAction{weight,action});
}

void Actor::Initialize()
{
    Initiative = D20() + Stats.DEX + Stats.InitiativeBonus;
    MaxHP = 0;
    for (int i = 0; i < Stats.HDNum; ++i)
        MaxHP += Stats.HD() + Stats.CON;

    HP = MaxHP;
}

Actor::Actor(std::string Name, const StatBlock & StatBlock, int Team, std::ostream & out) :
        Stats(StatBlock), Team(Team), Name(Name), out(out)
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
    return HP > 0;
}

void Actor::TakeAction(Arena & arena)
{
    if (Alive())
    {
        const Action & action = Stats.Actions[0].Action;
        Actor & target = arena.OtherGroup(Team).FirstAlive();

        std::cout << Name << " uses " << action << " on " << target << "." << std::endl;

        action(*this, target, out);
    }
}

void Actor::TakeDamage(int damage)
{
    HP -= damage;
    if (HP <= 0)
    {
        HP = 0;
        out << Name << " died!" << std::endl;
    }
}

int Actor::CurrentHP() const
{
    return HP;
}

