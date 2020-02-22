//
// Created by Bryan on 2/21/2020.
//

#include "Group.h"

Group::Group() : Wins(0) {}

/**
 * Clears battle info for the group and each member.
 */
void Group::ClearStats()
{
    Wins = 0;
    for (Group::ActorInfo & member : Members)
        ClearActorInfo(member);
}


/**
 * Sets all values in an ActorInfo to 0 except the actor pointer.
 * @param actor
 */
void Group::ClearActorInfo(Group::ActorInfo & actor)
{
    memset(&actor + sizeof(Actor *), 0, sizeof(Group::ActorInfo) - sizeof(Actor *));
}

int Group::getWins() const
{
    return Wins;
}

void Group::setWins(int wins)
{
    Wins = wins;
}

int Group::AddMember(Actor * actor)
{
    int index = Members.size();
    Members.push_back((Group::ActorInfo{}));
    Members[index].Actor = actor;
    return index;
}
