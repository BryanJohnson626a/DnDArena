//
// Created by Bryan on 2/21/2020.
// Documentation at: https://docs.google.com/document/d/1iu3Bahh_et_-0ri-1uIWw-tLclGHUdNYj-Xb8JPR2fc/edit?usp=sharing

#include <iostream>

#include "Source/Arena.h"
#include "Source/ImportJson.h"
#include "Source/Output.h"

int main()
{
    Out.Level = AllActions;
    StatBlock * Fighter = ParseStatBlock("Fighter5");
    if (Fighter == nullptr)
        return 0;

    auto * BG = new Arena();

    BG->AddTeam("Team A");
    BG->AddTeam("Team B");
    BG->AddCombatant("Alice", *Fighter, 0);
    BG->AddCombatant("Bob", *Fighter, 1);

    BG->Initialize();

    BG->DoBattles(1);

    return 0;
}
