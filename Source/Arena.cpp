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
    {
        int winner = DoBattle();
        if (winner == 0)
        {
            //std::cin.get();
        }
    }

    OUT_TRIALS ALL_ENDL

    for (Group & group : Combatants)
    OUT_RESULTS << group.Name << " won " << group.Wins << " times (" << 100 * float(group.Wins) / float(trials)
                << "%)." ALL_ENDL
}

int Arena::DoBattle()
{
    RollInitiative();
    ++Battles;

    while (GroupsConscious() > 1)
    {
        OUT_ALL << *this ALL_ENDL
        DoRound();
    }

    OUT_TRIALS << *this ALL_ENDL
    OUT_ALL ALL_ENDL

    // Last group standing wins.
    for (Group & group: Combatants)
        if (group.MembersConscious() > 0)
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
        actor->TakeTurn();
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

    for (Actor * actor : InitiativeQueue)
        actor->RollHealth();

    for (Actor * actor : InitiativeQueue)
        actor->RollInitiative();

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

std::string Comma(int num)
{
    std::stringstream ss;
    ss.imbue(std::locale(""));
    ss << std::fixed << num;
    return ss.str();
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
                    case Kills:
                        total += (float) a.InfoStats.Kills;
                        OUT_RESULTS << a.Name << " killed " << Comma(a.InfoStats.Kills) << " enemies." ALL_ENDL
                        break;
                    case KOs:
                        total += (float) a.InfoStats.KOs;
                        OUT_RESULTS << a.Name << " KOed " << Comma(a.InfoStats.KOs) << " enemies." ALL_ENDL
                        break;
                    case Deaths:
                        total += (float) a.InfoStats.Deaths;
                        OUT_RESULTS << a.Name << " died " << Comma(a.InfoStats.Deaths) << " times." ALL_ENDL
                        break;
                    case KOed:
                        total += (float) a.InfoStats.KOed;
                        OUT_RESULTS << a.Name << " was KOed " << Comma(a.InfoStats.KOed) << " times." ALL_ENDL
                        break;
                    case DamageDone:
                        total += (float) a.InfoStats.DamageDone;
                        OUT_RESULTS << a.Name << " did " << Comma(a.InfoStats.DamageDone) << " damage." ALL_ENDL
                        break;
                    case DamageTaken:
                        total += (float) a.InfoStats.DamageTaken;
                        OUT_RESULTS << a.Name << " took " << Comma(a.InfoStats.DamageTaken) << " damage." ALL_ENDL
                        break;
                    case AttacksLanded:
                        total += (float) a.InfoStats.AttacksLanded;
                        OUT_RESULTS << a.Name << " Hit " << Comma(a.InfoStats.AttacksLanded) << " times." ALL_ENDL
                        break;
                    case AttacksMissed:
                        total += (float) a.InfoStats.AttacksMissed;
                        OUT_RESULTS << a.Name << " Missed " << Comma(a.InfoStats.AttacksMissed) << " times." ALL_ENDL
                        break;
                    case AttacksReceived:
                        total += (float) a.InfoStats.AttacksReceived;
                        OUT_RESULTS << a.Name << " was hit " << Comma(a.InfoStats.AttacksReceived) << " times." ALL_ENDL
                        break;
                    case AttacksAvoided:
                        total += (float) a.InfoStats.AttacksAvoided;
                        OUT_RESULTS << a.Name << " was missed " << Comma(a.InfoStats.AttacksAvoided)
                                    << " times." ALL_ENDL
                        break;
                    case CritsLanded:
                        total += (float) a.InfoStats.CritsLanded;
                        OUT_RESULTS << a.Name << " critically hit " << Comma(a.InfoStats.CritsLanded)
                                    << " times." ALL_ENDL
                        break;
                    case CritsReceived:
                        total += (float) a.InfoStats.CritsReceived;
                        OUT_RESULTS << a.Name << " was critically hit " << Comma(a.InfoStats.CritsReceived) << " times."
                        ALL_ENDL
                        break;
                    case SurvivalRate:
                        total += 1 - (float(a.InfoStats.Deaths) / float(NumBattles()));
                        OUT_RESULTS << a.Name << " survived "
                                    << (1 - (float(a.InfoStats.Deaths) / float(NumBattles()))) * 100
                                    << "% of battles." ALL_ENDL
                        break;
                    case KOsKOedRatio:
                        if (a.InfoStats.KOed > 0)
                        {
                            float kd = float(a.InfoStats.KOs) / float(a.InfoStats.KOed);
                            total += kd;
                            if (kd > 1)
                            {
                                OUT_RESULTS << a.Name << "'s KOs/KOed was " << std::setprecision(2) << kd
                                            << "/1."
                                ALL_ENDL
                            }
                            else
                            {
                                OUT_RESULTS << a.Name << "'s KOs/KOed was 1/" << std::setprecision(2)
                                            << 1.f / kd << "."
                                ALL_ENDL
                            }
                        }
                        else if (a.InfoStats.KOs > 0)
                        OUT_RESULTS << a.Name << "'s KOs/KOed was perfect." ALL_ENDL
                        else
                        OUT_RESULTS << a.Name << "'s KOs/KOed was nothing." ALL_ENDL
                        break;
                    case KDRatio:
                        if (a.InfoStats.Deaths > 0)
                        {
                            float kd = float(a.InfoStats.Kills) / float(a.InfoStats.Deaths);
                            total += kd;
                            if (kd > 1)
                            OUT_RESULTS << a.Name << "'s K/D was " << std::setprecision(2) << kd << "/1."
                            ALL_ENDL
                            else
                            OUT_RESULTS << a.Name << "'s K/D was 1/" << std::setprecision(2) << 1.f / kd << "."
                            ALL_ENDL
                        }
                        else if (a.InfoStats.Kills > 0)
                        OUT_RESULTS << a.Name << "'s K/D was perfect." ALL_ENDL
                        else
                        OUT_RESULTS << a.Name << "'s K/D was nothing." ALL_ENDL
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
