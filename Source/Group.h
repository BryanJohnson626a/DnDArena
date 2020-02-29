//
// Created by Bryan on 2/21/2020.
//

#pragma once

#include <vector>
#include <string>

#include "Types.h"
#include "Actor.h"

using Actors = std::vector<Actor>;

class Group
{
public:

    Group(std::string_view name, int team, Arena & arena);

    void ClearStats();

    int AddActor(std::string_view name, std::shared_ptr<const StatBlock> stat_block);

    [[nodiscard]] int MembersAlive() const;

    [[nodiscard]] int MembersConscious() const;

    Actor & FirstAlive();

    Actor & FirstConscious();

    std::string Name;
    int Wins;
    int Team;
    Actors Members;
    Arena & CurrentArena;
};

