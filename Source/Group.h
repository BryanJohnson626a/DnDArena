//
// Created by Bryan on 2/21/2020.
//

#pragma once

#include <vector>
#include <string>

#include "Types.h"

class StatBlock;
class Actor;

using Actors = std::vector<Actor>;

class Group
{
public:

    Group(std::string name, int team, std::ostream & out);

    void ClearStats();
    int AddActor(std::string name, const StatBlock & actor);
    Actor & FirstAlive();

    std::string Name;
    int Wins;
    int Team;
    Actors Members;

    int MembersAlive() const;

    std::ostream & out;
};

