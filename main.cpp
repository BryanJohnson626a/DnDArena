//
// Created by Bryan on 2/21/2020.
// Documentation at: https://docs.google.com/document/d/1iu3Bahh_et_-0ri-1uIWw-tLclGHUdNYj-Xb8JPR2fc/edit?usp=sharing

#include <iostream>
#include <sstream>

#include "Source/Arena.h"
#include "Source/Output.h"

int main()
{
    Output::Out().SetLevel(AllActions);
    {
        Arena BG;
        BG.AddTeam("Team A");
        BG.AddCombatant("Player Alice", "Fighter5", 0);
        BG.AddCombatant("Player Bob", "Barbarian5", 0);
        BG.AddCombatant("Player Carol", "Cleric5", 0);

        BG.AddTeam("Team B");
        for (int i = 0; i < 20; ++i)
        {
            std::stringstream name;
            name << "Goblin " << i + 1;
            BG.AddCombatant(name.str(), "Goblin", 1);
        }

        BG.Initialize();

        BG.DoBattles(1);
        BG.Report(KillDeathRatio, "Player");
        BG.Report(Kills, "Player");
        BG.Report(FinishedOff, "Player");
        BG.Report(Falls, "Player");
        BG.Report(Deaths, "Player");
        BG.Report(KillDeathRatio, "Goblin");
    }
    return 0;
}
