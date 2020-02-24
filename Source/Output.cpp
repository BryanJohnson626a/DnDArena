//
// Created by bryan on 2/23/20.
//

#include "Output.h"
#include "Actor.h"
#include "Action.h"
#include "Arena.h"
#include "Group.h"

std::ostream & operator<<(std::ostream & out, const ActorQueue & actors)
{
    out << "Initiative order:" << std::endl;
    for (const Actor * actor : actors)
        out << actor->Initiative << " " << *actor << std::endl;

    return out;
}

std::ostream & operator<<(std::ostream & out, const Actor & actor)
{
    out << actor.Name;
    return out;
}

std::ostream & operator<<(std::ostream & out, const Action & action)
{
    out << action.Name;
    return out;
}

std::ostream & operator<<(std::ostream & out, const Arena & arena)
{
    out << "Battle " << arena.NumBattles() << ": " << arena.GetCombatants();
    return out;
}

std::ostream & operator<<(std::ostream & out, const Groups & groups)
{
    bool first = true;
    for (const auto & group : groups)
        if (group.MembersAlive() > 0)
        {
            if (first)
                first = false;
            else
                out << " vs ";

            out << group;
        }
    return out;
}

std::ostream & operator<<(std::ostream & out, const Group & group)
{
    bool first = true;
    for (const auto & Member : group.Members)
    {
        if (first)
            first = false;
        else
            out << ", ";

        switch (Member.GetDeathState())
        {

            case Conscious:
                out << Member << "(" << Member.CurrentHP() << " HP)";
                break;
            case Stable:
                out << Member << "(Stable)";
                break;
            case Dying:
                out << Member << "(Dying " << Member.SuccessfulDeathSaves << ":" << Member.FailedDeathSaves << ")";
                break;
            case Dead:
                out << Member << "(Dead)";
                break;
        }
    }
    return out;
}
