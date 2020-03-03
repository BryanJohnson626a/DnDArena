//
// Created by Bryan on 2/21/2020.
// Documentation at: https://docs.google.com/document/d/1iu3Bahh_et_-0ri-1uIWw-tLclGHUdNYj-Xb8JPR2fc/edit?usp=sharing

#include <iostream>
#include <sstream>

#include "Source/Arena.h"
#include "Source/Output.h"

int main()
{
    {
        Output::Out().SetLevel(Results);
        Arena BG;
        BG.AddTeam("Team A");
        BG.AddCombatant("Player Alice", "BattleMasterFighter5", 0);
        BG.AddCombatant("Player Bob", "Barbarian5", 0);
        BG.AddCombatant("Player Carol", "Cleric5", 0);

        BG.AddTeam("Team B");
        std::string enemy_type = "Ogre";
        for (int i = 0; i < 10; ++i)
        {
            std::stringstream name;
            name << enemy_type << " " << i + 1;
            BG.AddCombatant(name.str(), enemy_type, 1);
        }

        BG.Initialize();

        BG.DoBattles(10000);
        //BG.Report(KOsKOedRatio, "Player");
        //BG.Report(KDRatio, "Player");
        BG.Report(KOs, "Player");
        BG.Report(KOed, "Player");
        BG.Report(Kills, "Player");
        BG.Report(Deaths, "Player");
        BG.Report(DamageDone, "Alice");
        BG.Report(DamageDone, "Bob");
        BG.Report(DamageDone, "Carol");
        BG.Report(DamageDone, enemy_type);

        Output::Out().SetLevel(NoOutput);

    }
    return 0;
}
