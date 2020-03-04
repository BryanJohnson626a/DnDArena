//
// Created by Bryan on 2/21/2020.
//

#include <cstring>
#include <utility>
#include "Group.h"
#include "Actor.h"

Group::Group(std::string_view name, int team, Arena & arena) :
Name(name), Team(team), Wins(0), CurrentArena(arena)
{}

void Group::ClearStats()
{
    Wins = 0;
    for (Actor & Member: Members)
        Member.ResetInfo();
}


int Group::AddActor(std::string_view name, std::shared_ptr<const StatBlock> stat_block)
{
    int index = Members.size();
    Members.emplace_back(Actor(name, stat_block, Team, CurrentArena));
    return index;
}

int Group::MembersAlive() const
{
    int alive = 0;
    for (const auto& actor : Members)
        if (actor.Alive())
            ++alive;
    return alive;
}

int Group::MembersConscious() const
{
    int conscious = 0;
    for (const auto& actor : Members)
        if (actor.Conscious())
            ++conscious;
    return conscious;
}

Actor * Group::FirstAlive()
{
    for (auto & actor : Members)
        if (actor.Alive())
            return &actor;

    return nullptr;
}

ActorPtrs Group::AllAlive()
{
    ActorPtrs alive;
    for (auto & actor : Members)
        if (actor.Alive())
            alive.emplace_back(&actor);

    return alive;
}

Actor * Group::FirstConscious()
{
    for (auto & actor : Members)
        if (actor.Conscious())
            return &actor;

    return nullptr;
}

ActorPtrs Group::AllConscious()
{
    ActorPtrs conscious;
    for (auto & actor : Members)
        if (actor.Conscious())
            conscious.emplace_back(&actor);

    return conscious;
}
