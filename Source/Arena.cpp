//
// Created by Bryan on 2/21/2020.
//

#include <algorithm>
#include <chrono>
#include <iomanip>

#include "Arena.h"
#include "Actor.h"
#include "Action.h"
#include "Dice.h"
#include "Output.h"

void Arena::DoBattles(int trials)
{
    for (int i = 0; i < trials; ++i)
        DoBattle();

    OUT_TRIALS << std::endl;

    for (Group & group : Combatants)
        OUT_RESULTS << group.Name << " won " << group.Wins << " times (" << 100 * float(group.Wins) / float(trials)
                    << "%)." << std::endl;
}

int Arena::DoBattle()
{
    RollInitiative();
    ++Battles;

    OUT_ALL << *this << std::endl << std::endl << InitiativeQueue << std::endl;

    Die::dice_output = true;

    while (GroupsConscious() > 1)
    {
        OUT_ALL << *this << std::endl;
        DoRound();
    }
    Die::dice_output = false;

    OUT_TRIALS << *this << std::endl;
    OUT_ALL << std::endl;

    for (Group & group: Combatants)
        if (group.MembersAlive() > 0)
        {
            ++group.Wins;
            return group.Team;
        }

    // Tie, no winner.
    return -1;
}

void Arena::DoRound()
{
    for (auto actor : InitiativeQueue)
        actor->DoRound();
}

void Arena::Initialize()
{
    Battles = 0;
    for (Group & group: Combatants)
        group.ClearStats();

    // Remake the initiative queue as pointers may have expired.
    InitiativeQueue.clear();
    for (Group & group: Combatants)
        for (Actor & actor : group.Members)
            InitiativeQueue.push_back(&actor);
}

int Arena::AddCombatant(std::string_view name, std::string_view stat_block, int team)
{
    auto stats = StatBlock::Get(stat_block);

    if (Combatants.size() <= team || stats == nullptr)
        return -1;

    int index = Combatants[team].AddActor(name, stats);

    return index;
}

int Arena::AddTeam(std::string_view name)
{
    int team = Combatants.size();
    Combatants.emplace_back(Group(name, team, *this));
    return team;
}

bool CompareInitiative(const Actor * A, const Actor * B)
{
    return A->Initiative > B->Initiative;
}

void Arena::RollInitiative()
{
    for (Actor * actor : InitiativeQueue)
        actor->Initialize();

    std::sort(InitiativeQueue.begin(), InitiativeQueue.end(), CompareInitiative);
}

int Arena::GroupsAlive() const
{
    int alive = 0;
    for (const auto & group : Combatants)
        if (group.MembersAlive() > 0)
            ++alive;

    return alive;
}

int Arena::GroupsConscious() const
{
    int conscious = 0;
    for (const auto & group : Combatants)
        if (group.MembersConscious() > 0)
            ++conscious;

    return conscious;
}

Group & Arena::OtherGroup(int team)
{
    for (auto & group : Combatants)
        if (group.Team != team)
            return group;

    return Combatants[0];
}

const Groups & Arena::GetCombatants() const
{
    return Combatants;
}

int Arena::NumBattles() const
{
    return Battles;
}

float Arena::Report(enum ReportType type, std::string_view target)
{
    bool found = false;
    float total = 0;
    for (const auto & g : Combatants)
        for (const auto & a : g.Members)
            if (target == "All" || a.Name.find(target) != std::string::npos)
            {
                found = true;
                switch (type)
                {
                    case FinishedOff:
                        total += (float) a.InfoStats.Kills;
                        OUT_RESULTS << a.Name << " finished off " << a.InfoStats.Kills << " enemies." << std::endl;
                        break;
                    case Kills:
                        total += (float) a.InfoStats.Downs;
                        OUT_RESULTS << a.Name << " killed " << a.InfoStats.Downs << " enemies." << std::endl;
                        break;
                    case Deaths:
                        total += (float) a.InfoStats.Deaths;
                        OUT_RESULTS << a.Name << " died " << a.InfoStats.Deaths << " times." << std::endl;
                        break;
                    case DamageDone:
                        total += (float) a.InfoStats.DamageDone;
                        OUT_RESULTS << a.Name << " did " << a.InfoStats.DamageDone << " damage." << std::endl;
                        break;
                    case DamageTaken:
                        total += (float) a.InfoStats.DamageTaken;
                        OUT_RESULTS << a.Name << " took " << a.InfoStats.DamageTaken << " damage." << std::endl;
                        break;
                    case AttacksLanded:
                        total += (float) a.InfoStats.AttacksLanded;
                        OUT_RESULTS << a.Name << " Hit " << a.InfoStats.AttacksLanded << " times." << std::endl;
                        break;
                    case AttacksMissed:
                        total += (float) a.InfoStats.AttacksMissed;
                        OUT_RESULTS << a.Name << " Missed " << a.InfoStats.AttacksMissed << " times." << std::endl;
                        break;
                    case AttacksReceived:
                        total += (float) a.InfoStats.AttacksReceived;
                        OUT_RESULTS << a.Name << " was hit " << a.InfoStats.AttacksReceived << " times." << std::endl;
                        break;
                    case AttacksAvoided:
                        total += (float) a.InfoStats.AttacksAvoided;
                        OUT_RESULTS << a.Name << " was missed " << a.InfoStats.AttacksAvoided << " times." << std::endl;
                        break;
                    case CritsLanded:
                        total += (float) a.InfoStats.CritsLanded;
                        OUT_RESULTS << a.Name << " critically hit " << a.InfoStats.CritsLanded << " times."
                                    << std::endl;
                        break;
                    case CritsReceived:
                        total += (float) a.InfoStats.CritsReceived;
                        OUT_RESULTS << a.Name << " was critically hit " << a.InfoStats.CritsReceived << " times."
                                    << std::endl;
                        break;
                    case SurvivalRate:
                        total += 1 - (float(a.InfoStats.Deaths) / float(NumBattles()));
                        OUT_RESULTS << a.Name << " survived "
                                    << (1 - (float(a.InfoStats.Deaths) / float(NumBattles()))) * 100 << "% of battles."
                                    << std::endl;
                        break;
                    case KillDeathRatio:
                        if (a.InfoStats.Deaths > 0)
                        {
                            float kd = float(a.InfoStats.Kills) / float(a.InfoStats.Deaths);
                            total += kd;
                            if (kd > 1)
                            {
                                OUT_RESULTS << a.Name << "'s K/D was " << std::setprecision(2) << kd << "/1."
                                            << std::endl;
                            }
                            else
                            {
                                OUT_RESULTS << a.Name << "'s K/D was 1/" << std::setprecision(2) << 1.f / kd << "."
                                            << std::endl;
                            }
                        }
                        else if (a.InfoStats.Kills > 0)
                        { OUT_RESULTS << a.Name << "'s K/D was perfect." << std::endl; }
                        else
                        { OUT_RESULTS << a.Name << "'s K/D was nothing." << std::endl; }
                        break;
                    default:
                    OUT_WARNING << "Invalid report request: " << type << WARNING_END
                        return 0;
                }
            }

    if (!found)
    OUT_WARNING << "Could not find \"" << target << "\" for report." << WARNING_END;

    return total;
}
