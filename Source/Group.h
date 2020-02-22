//
// Created by Bryan on 2/21/2020.
//

#include <vector>

class Actor;

class Group
{
public:
    // Stores battle information about an actor.
    struct ActorInfo
    {
        Actor * Actor;

        int Kills;
        int Deaths;

        int DamageDone;
        int DamageTaken;

        int AttacksMade;
        int AttacksMissed;

        int AttacksRecieved;
        int AttacksAvoided;

        int ForcedSaves;
        int ForcedSavesMade;

        int SavesDone;
        int SavesMade;
    };

    Group();
    void ClearStats();
    int getWins() const;
    void setWins(int wins);
    int AddMember(Actor * actor);
private:
    int Wins;
    std::vector<ActorInfo> Members;
    void ClearActorInfo(Group::ActorInfo & actor);
};

