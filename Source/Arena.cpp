//
// Created by Bryan on 2/21/2020.
//

#include "Arena.h"

struct ActorInstance
{
    Actor * Data;
    int HP;
    int Team;
    bool ShieldUp;
};

/**
 * Do the processing for a single battle.
 */
void Arena::DoRound()
{

}

/**
 *  Reset saved statistics for arena and all combatants.
 */
void Arena::ResetStats()
{
    Rounds = 0;
    for (Group & group : Combatants)
        group.ClearStats();
}

/**
 * Adds a combatant to a team.
 * @param actor
 * @param team The team to place the actor on.
 * @return The index of the new combatant in it's team, or -1 if the team doesn't exist.
 */
int Arena::AddCombatant(Actor * actor, int team)
{
    if (Combatants.size() <= team)
        return -1;
    else
        return Combatants[team].AddMember(actor);
}

/**
 * Adds a new team to the arena.
 * @return The index of the new team.
 */
int Arena::AddTeam()
{
    int index = Combatants.size();
    Combatants.emplace_back(Group());
    return index;
}

int Arena::getRounds() const
{
    return Rounds;
}
