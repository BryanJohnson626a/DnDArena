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

    Action Club{"Club", Stat::Strength, ActionType::MeleeAttack, 1, D4};
    Action Greataxe{"Greataxe", Stat::Strength, ActionType::MeleeAttack, 1, D12};
    Action Javelin{"Javelin", Stat::Strength, ActionType::RangedAttack, 1, D6};
    Action Scimitar{"Scimitar", Stat::Dexterity, ActionType::MeleeAttack, 1, D6};
    Action Shortbow{"Shortbow", Stat::Dexterity, ActionType::RangedAttack, 1, D6};
    Action LGreatclub{"Large Greatclub", Stat::Strength, ActionType::MeleeAttack, 2, D8};
    Action LJavelin{"Large Javelin", Stat::Strength, ActionType::RangedAttack, 2, D6};

    StatBlock Commoner{"Commoner", "Humanoid", 1, D8, 2};
    Commoner.CalculateDerivedStats();
    Commoner.AddAction(Club, 1);

    StatBlock Orc{"Orc", "Humanoid", 2, D8, 2,
                  0, 2, 0, 0, 0,
                  3, 1, 3, -2, 0, 0};
    Orc.CalculateDerivedStats();
    Orc.AddAction(Greataxe, 5);
    Orc.AddAction(Javelin, 1);

    StatBlock Goblin{"Goblin", "Humanoid", 2, D6, 2,
                     0, 1, 2, 0, 0,
                     -1, 2, 0, 0, -1, -1};
    Goblin.CalculateDerivedStats();
    Goblin.AddAction(LGreatclub, 5);
    Goblin.AddAction(LJavelin, 1);


    StatBlock Ogre{"Ogre", "Giant", 7, D10, 2,
                   0, 2, 0, 0, 0,
                   4, -1, 3, -3, -2, -2};
    Ogre.CalculateDerivedStats();
    Ogre.AddAction(Greataxe, 5);
    Ogre.AddAction(Javelin, 1);

    BG.AddTeam("Dirty Peasants");
    BG.AddTeam("Disney");

    BG.AddCombatant("Dirty Peasant #1", Commoner, 0);
    BG.AddCombatant("Dirty Peasant #2", Commoner, 0);
    BG.AddCombatant("Dirty Peasant #3", Commoner, 0);
    BG.AddCombatant("Dirty Peasant #4", Commoner, 0);
    BG.AddCombatant("Dirty Peasant #5", Commoner, 0);
    BG.AddCombatant("Dirty Peasant #6", Commoner, 0);
    BG.AddCombatant("Dirty Peasant #7", Commoner, 0);
    BG.AddCombatant("Dirty Peasant #8", Commoner, 0);

    BG.AddCombatant("Shrek", Ogre, 1);

    BG.Initialize();
    BG.DoBattles(100000);

    return 0;
}
