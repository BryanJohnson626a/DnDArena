//
// Created by Bryan on 2/21/2020.
//

#include <vector>
#include <xstring>

class Action;

class Actor
{
    struct WeightedAction
    {
        float Weight;
        Action * Data;
    };

    std::string Name;
    std::string Type;
    int HitDiceNumber;
    int HitDiceSize;
    int Speed;
    int Proficiency;
    int AC;
    bool HasShield;
    int STR, DEX, CON, INT, WIS, CHA;
    int SaveSTR, SaveDEX, SaveCON, SaveINT, SaveWIS, SaveCHA;

    std::vector<WeightedAction> Actions;
    std::vector<WeightedAction> BonusActions;

    std::vector<std::string> Immunities;
    std::vector<std::string> Resistances;
    std::vector<std::string> Weaknesses;
    std::vector<std::string> ConditionImmunities;
};
