//
// Created by Bryan on 2/21/2020.
// Documentation at: https://docs.google.com/document/d/1iu3Bahh_et_-0ri-1uIWw-tLclGHUdNYj-Xb8JPR2fc/edit?usp=sharing

#include <iostream>

#include "Source/Arena.h"
#include "Source/Output.h"

int main()
{
    Out.Level = AllActions;

    Arena BG;
    BG.AddTeam("Team A");
    BG.AddCombatant("Alice", "Fighter5", 0);

    BG.AddTeam("Team B");
    BG.AddCombatant("Bob", "Barbarian5", 1);

    BG.Initialize();

    BG.DoBattles(1);

    return 0;
}
