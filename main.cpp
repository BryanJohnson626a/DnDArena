//
// Created by Bryan on 2/21/2020.
// Documentation at: https://docs.google.com/document/d/1iu3Bahh_et_-0ri-1uIWw-tLclGHUdNYj-Xb8JPR2fc/edit?usp=sharing

#include <iostream>
#include "Source/Arena.h"
#include "Source/Actor.h"
#include "Source/Dice.h"
#include "Source/Action.h"

int main()
{
    Arena BG(std::cout);

    StatBlock Commoner{"Commoner", "Humanoid", 1, D8, 2};
    Commoner.CalculateDerivedStats();
    Action Club{"Club", Stat::Strength, ActionType::MeleeAttack, 1, D4};
    Commoner.AddAction(Club, 1);

    BG.AddTeam(std::string());
    BG.AddTeam(std::string());
    BG.AddCombatant("Alice", Commoner, 0);
    BG.AddCombatant("Bob", Commoner, 1);

    BG.DoBattle();

    return 0;
}
