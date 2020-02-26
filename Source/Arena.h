//
// Created by Bryan on 2/21/2020.
//
#pragma once

#include <ostream>

#include "Types.h"
#include "Group.h"

class Arena
{
public:

/**
 * Adds a new team to the arena.
 * @return The index of the new team.
 */
    int AddTeam(std::string name);

/**
 * Adds a combatant to a team.
 * @param Stats
 * @param team The team to place the Stats on.
 * @return The index of the new combatant in it's team,
 * or -1 if the team doesn't exist.
 */
    int AddCombatant(std::string Name, const StatBlock & Stats, int team);

/**
 *  Reset saved statistics for arena and all combatants.
 */
    void Initialize();

/**
 * Do one full fight between all combatants.
 * @return The index of the team that won.
 */
    int DoBattle();

    [[nodiscard]] Group & OtherGroup(int team);

    [[nodiscard]] int GroupsAlive() const;

    [[nodiscard]] int GroupsConscious() const;

    [[nodiscard]] const Groups & GetCombatants() const;

    [[nodiscard]] int NumBattles() const;

    void DoBattles(int trials);

private:
    void RollInitiative();

    void DoRound();

    Groups Combatants;
    ActorPtrs InitiativeQueue;
    int Battles;
};
