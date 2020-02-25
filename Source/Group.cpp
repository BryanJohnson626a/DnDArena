//
// Created by Bryan on 2/21/2020.
//

#include <cstring>
#include "Group.h"
#include "Actor.h"

Group::Group(std::string name, int team) : Name(name), Team(team), Wins(0)
{}

void Group::ClearStats()
{
    Wins = 0;
    for (Actor & Member: Members)
        Member.ResetInfo();
}


int Group::AddActor(std::string name, const StatBlock & actor)
{
    int index = Members.size();
    Members.emplace_back(Actor(name, actor, Team));
    return index;
}

int Group::MembersAlive() const
{
    int alive = 0;
    for (auto actor : Members)
        if (actor.Alive())
            ++alive;
    return alive;
}

int Group::MembersConscious() const
{
    int conscious = 0;
    for (auto actor : Members)
        if (actor.Conscious())
            ++conscious;
    return conscious;
}

Actor & Group::FirstAlive()
{
    for (auto & actor : Members)
        if (actor.Alive())
            return actor;

    return Members[0];
}

Actor & Group::FirstConscious()
{
    for (auto & actor : Members)
        if (actor.Conscious())
            return actor;

    return FirstAlive();
}
