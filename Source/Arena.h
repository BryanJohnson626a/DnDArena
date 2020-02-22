//
// Created by Bryan on 2/21/2020.
//

#include "Group.h"

class Arena
{
public:
    int AddTeam();
    int AddCombatant(Actor * actor, int team);
    void ResetStats();
    void DoRound();
    int getRounds() const;
private:
    std::vector<Group> Combatants;
    int Rounds;
};
