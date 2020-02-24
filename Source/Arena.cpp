//
// Created by Bryan on 2/21/2020.
//

#include <algorithm>

#include "Arena.h"
#include "Actor.h"
#include "Action.h"
#include "Dice.h"
#include "Output.h"

int Arena::DoBattle()
{
    RollInitiative();

    std::cout << *this << std::endl;


    out << InitiativeQueue << std::endl;

    while (GroupsAlive() > 1)
    {
        DoRound();
        out << std::endl << *this << std::endl << std::endl;
    }

    return 0;
}

void Arena::ResetStats()
{
    Rounds = 0;
    for (Group & group : Combatants)
        group.ClearStats();
}

int Arena::AddCombatant(std::string Name, const StatBlock & Stats, int team)
{
    if (Combatants.size() <= team)
        return -1;
    int index = Combatants[team].Members.size();

    Combatants[team].AddActor(Name, Stats);
    InitiativeQueue.emplace_back(&Combatants[team].Members[index]);

    return index;
}

int Arena::AddTeam(std::string name)
{
    int team = Combatants.size() - 1;
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

void Arena::DoRound()
{
    for (auto actor : InitiativeQueue)
        actor->TakeAction(*this);
}

Group & Arena::OtherGroup(int team)
{
    for (auto & group : Combatants)
        if (group.Team != team)
            return group;

    return Combatants[0];
}

Arena::Arena(std::ostream & out) : out(out), Rounds(0)
{}

const Groups & Arena::GetCombatants() const
{
    return Combatants;
}
