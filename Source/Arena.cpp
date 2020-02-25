//
// Created by Bryan on 2/21/2020.
//

#include <algorithm>

#include "Arena.h"
#include "Actor.h"
#include "Action.h"
#include "Dice.h"
#include "Output.h"

void Arena::DoBattles(int trials)
{
    for (int i = 0; i < trials; ++i)
    {
        DoBattle();
    }

    if (Out(TrialResults)) Out.O() << std::endl;
    if (Out(Results))
        for (Group & group : Combatants)
            Out.O() << group.Name << " won " << group.Wins <<
                    " times (" << float(group.Wins) / float(trials) << "%)." << std::endl;
}

int Arena::DoBattle()
{
    RollInitiative();
    ++Battles;
    if (Out(AllActions))
        RollInitiative();

    if (Out(AllActions)) Out.O() << *this << std::endl;

    if (Out(AllActions)) Out.O() << std::endl;
    if (Out(AllActions)) Out.O() << InitiativeQueue << std::endl;

    while (GroupsAlive() > 1)
    {
        if (Out(OL::AllActions)) Out.O() << *this << std::endl;
        DoRound();
    }

    if (Out(TrialResults)) Out.O() << *this << std::endl;
    if (Out(AllActions)) Out.O() << std::endl;

    for (Group & group: Combatants)
        if (group.MembersAlive() > 0)
        {
            ++group.Wins;
            return group.Team;
        }

    return 0;
}

void Arena::DoRound()
{
    for (auto actor : InitiativeQueue)
        actor->DoRound(*this);
}

void Arena::Initialize()
{
    Battles = 0;
    for (Group & group: Combatants)
        group.ClearStats();

    // Remake the initiative queue as pointers may have expired.
    InitiativeQueue.empty();
    for (Group & group: Combatants)
        for (Actor & actor : group.Members)
            InitiativeQueue.push_back(&actor);
}

int Arena::AddCombatant(std::string name, const StatBlock & stats, int team)
{
    if (Combatants.size() <= team)
        return -1;

    int index = Combatants[team].AddActor(name, stats);

    return index;
}

int Arena::AddTeam(std::string name)
{
    int team = Combatants.size();
    Combatants.emplace_back(Group(name, team));
    return team;
}

bool CompareInitiative(const Actor * A, const Actor * B)
{
    return A->Initiative > B->Initiative;
}

void Arena::RollInitiative()
{
    for (Actor * actor : InitiativeQueue)
        actor->Initialize();

    std::sort(InitiativeQueue.begin(), InitiativeQueue.end(), CompareInitiative);
}

int Arena::GroupsAlive() const
{
    int alive = 0;
    for (auto group : Combatants)
        if (group.MembersAlive() > 0)
            ++alive;

    return alive;
}

int Arena::GroupsConscious() const
{
    int conscious = 0;
    for (auto group : Combatants)
        if (group.MembersConscious() > 0)
            ++conscious;

    return conscious;
}

Group & Arena::OtherGroup(int team)
{
    for (auto & group : Combatants)
        if (group.Team != team)
            return group;

    return Combatants[0];
}

const Groups & Arena::GetCombatants() const
{
    return Combatants;
}

int Arena::NumBattles() const
{
    return Battles;
}
