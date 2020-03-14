//
// Created by Bryan on 2/21/2020.
// Documentation at: https://docs.google.com/document/d/1iu3Bahh_et_-0ri-1uIWw-tLclGHUdNYj-Xb8JPR2fc/edit?usp=sharing

#include <iostream>
#include <sstream>

#include "Source/Arena.h"
#include "Source/Output.h"
#include "Source/Dice.h"

int main()
{
    Output::Out().SetLevel(AllOutput);
    Output::Out().SetDelay(250);

    Arena BG;
    BG.AddTeam("Team A");
    BG.AddCombatant("Player Alice", "BattleMasterFighter5", 0);
    BG.AddCombatant("Player Bob", "Barbarian5", 0);
    BG.AddCombatant("Player Carol", "Cleric5", 0);

    BG.AddTeam("Team B");

    std::vector<std::pair<int, std::string>> enemies;
    enemies.emplace_back(std::make_pair(4, "Lemure"));
    enemies.emplace_back(std::make_pair(3, "BeardedDevil"));
    enemies.emplace_back(std::make_pair(2, "SpinedDevil"));
    enemies.emplace_back(std::make_pair(1, "Imp"));
    for (auto & p : enemies)
    {
        for (int i = 0; i < p.first; ++i)
        {
            std::stringstream name;
            name << p.second << " " << i + 1;
            BG.AddCombatant(name.str(), p.second, 1);
        }
    }
    BG.Initialize();
    BG.DoBattles(1);

    BG.Report(DamageDone, "Alice");
    BG.Report(DamageDone, "Bob");
    BG.Report(DamageDone, "Carol");

    std::cout << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    Output::Out().SetDelay(0);
    Output::Out().SetLevel(Results);

    BG.Initialize();
    BG.DoBattles(100000);

    std::cout << std::endl;
    Output::Out().SetDelay(500);
    BG.Report(KOsKOedRatio, "Player");
    BG.Report(KDRatio, "Player");
    BG.Report(KOs, "Player");
    BG.Report(KOed, "Player");
    BG.Report(Kills, "Player");
    BG.Report(Deaths, "Player");
    BG.Report(DamageDone, "Alice");
    BG.Report(DamageDone, "Bob");
    BG.Report(DamageDone, "Carol");
    //BG.Report(DamageDone, enemy_type);

    // No output needed for destructors.
    Output::Out().SetLevel(NoOutput);

    return 0;
}
