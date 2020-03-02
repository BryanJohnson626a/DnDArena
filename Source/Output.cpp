//
// Created by bryan on 2/23/20.
//

#include "Output.h"
#include "Actor.h"
#include "Action.h"
#include "Arena.h"
#include "Group.h"

Output::Output(std::ostream & out_stream, enum MessageLevel level) : MessageStream(&out_stream), Level(level)
{}

bool Output::CheckLevel(enum MessageLevel level) const
{
    return (level <= MAX_OUTPUT_LEVEL && level <= Level);
}

std::ostream & Output::GetStream()
{
    return *MessageStream;
}

bool Output::SetLevel(MessageLevel new_level)
{
    if (new_level > MAX_OUTPUT_LEVEL)
    {
        Level = MAX_OUTPUT_LEVEL;
        return false;
    }
    else
    {
        Level = new_level;
        return true;
    }
}

Output & Output::Out()
{
    static Output OutputSingleton = Output();
    return OutputSingleton;
}

std::ostream & operator<<(std::ostream & out, const ActorPtrs & actors)
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
                out << Member << " (" << Member.CurrentHP() << "/" << Member.MaxHP() << " HP)";
                break;
            case Stable:
                out << Member << "(Stable)";
                break;
            case Dying:
                out << Member << "(Dying " << Member.SuccessfulDeathSaves << "S:" << Member.FailedDeathSaves << "F)";
                break;
            case Dead:
                out << Member << "(Dead)";
                break;
        }
    }
    return out;
}

std::ostream & operator<<(std::ostream & out, enum DamageType damage_type)
{
    switch (damage_type)
    {
        case Slashing:
            out << "slashing";
            break;
        case Bludgeoning:
            out << "bludgeoning";
            break;
        case Piercing:
            out << "piercing";
            break;
        case Fire:
            out << "fire";
            break;
        case Cold:
            out << "cold";
            break;
        case Lightning:
            out << "lightning";
            break;
        case Thunder:
            out << "thunder";
            break;
        case Acid:
            out << "acid";
            break;
        case Poison:
            out << "poison";
            break;
        case Necrotic:
            out << "necrotic";
            break;
        case Radiant:
            out << "radiant";
            break;
        case Psychic:
            out << "psychic";
            break;
        case Force:
            out << "force";
            break;
        default:
            out << "invalid damage type";
            break;
    }
    return out;
}
