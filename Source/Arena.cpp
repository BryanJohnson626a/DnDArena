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
        DoBattle();

    if (OUTPUT_LEVEL > 0)
        for (Group & group : Combatants)
            out << group.Name << " won " << group.Wins << " times (" << 100 * float(group.Wins) / trials << "%)." << std::endl;
}

int Arena::DoBattle()
{
    RollInitiative();
    ++Battles;

    if (OUTPUT_LEVEL > 1)
        out << *this << std::endl;

    if (OUTPUT_LEVEL > 1)
        out << InitiativeQueue << std::endl;

    while (GroupsAlive() > 1)
    {
        DoRound();
        if (OUTPUT_LEVEL > 1)
            out << std::endl << *this << std::endl << std::endl;
    }

    for(Group & group: Combatants)
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
        actor->TakeAction(*this);
}

void Arena::Initialize()
{
    Battles = 0;
    for (Group & group : Combatants)
        group.ClearStats();

    // Remake the initiative queue as pointers may have expired.
    InitiativeQueue.empty();
    for (Group & group : Combatants)
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
    Combatants.emplace_back(Group(name, team, out));
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

Arena::Arena(std::ostream & output_stream) : out(output_stream), Battles(0)
{}

const Groups & Arena::GetCombatants() const
{
    return Combatants;
}

int Arena::NumBattles() const
{
    return Battles;
}
