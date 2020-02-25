//
// Created by bryan on 2/24/20.
//

#include <fstream>
#include <algorithm>

#include "ImportJson.h"
#include "Actor.h"
#include "Action.h"
#include "Dice.h"
#include "Output.h"

nlohmann::json OpenJson(std::string_view file_name)
{
    if (Out(Results)) Out.O() << "Loading \"" << file_name << "\"." << std::endl;

    std::string path = std::string("./Resources/") + std::string(file_name) + std::string(".json");

    std::ifstream file(path);
    if (!file.is_open())
    {
        if (Out(Warnings)) Out.O() << "Warning: \"" << path << "\": " << strerror(errno) << std::endl;
        return nlohmann::json();
    }
    nlohmann::json js;
    try
    {
        js = nlohmann::json::parse(file);
    }
    catch (nlohmann::json::exception & e)
    {
        if (Out(Warnings)) Out.O() << "Warning: \"" << file_name << "\": " << e.what() << std::endl;
        return nlohmann::json();
    }
    return js;
}

template<class T>
void Import(const nlohmann::json & js, T & target, const char * key)
{
    try
    {
        js.at(key).get_to(target);
    }
    catch (nlohmann::json::exception & e)
    {
        target = T();
    }
}

void from_json(const nlohmann::json & js, const Action *& action)
{
    std::string name;
    Import(js, name, "Name");
    action = Action::Get(name);
}

void from_json(const nlohmann::json & js, Action *& action)
{
    std::string name;
    Import(js, name, "Name");
    action = Action::Get(name);
}

void from_json(const nlohmann::json & js, WeaponAttack & weapon_attack)
{
    Import(js, weapon_attack.Name, "Name");
    Import(js, weapon_attack.KeyAttribute, "KeyAttribute");
    Import(js, weapon_attack.DamageDiceNum, "DamageDiceNum");
    int die_size;
    Import(js, die_size, "DamageDiceSize");
    weapon_attack.DamageDie = Die::Get(die_size);
}

void from_json(const nlohmann::json & js, MultiAction & multi_action)
{
    Import(js, multi_action.Name, "Name");
    Import(js, multi_action.Actions, "Actions");
}

void from_json(const nlohmann::json & js, ActionInstance & weighted_action)
{
    Import(js, weighted_action.Action, "Action");
    Import(js, weighted_action.Uses, "Uses");
}

bool Invalid(const ActionInstance & a)
{
    return a.Action == nullptr;
}

void from_json(const nlohmann::json & js, StatBlock & stat_block)
{
    Import(js, stat_block.Name, "Name");
    Import(js, stat_block.Type, "Type");
    Import(js, stat_block.HDNum, "HDNum");
    Import(js, stat_block.HDSize, "HDSize");
    Import(js, stat_block.Proficiency, "Proficiency");
    Import(js, stat_block.InitiativeBonus, "InitiativeBonus");
    Import(js, stat_block.ArmorValue, "ArmorValue");
    Import(js, stat_block.ShieldValue, "ShieldValue");
    Import(js, stat_block.AttackBonus, "AttackBonus");
    Import(js, stat_block.DamageBonus, "DamageBonus");
    Import(js, stat_block.Strength, "Strength");
    Import(js, stat_block.Dexterity, "Dexterity");
    Import(js, stat_block.Constitution, "Constitution");
    Import(js, stat_block.Intelligence, "Intelligence");
    Import(js, stat_block.Wisdom, "Wisdom");
    Import(js, stat_block.Charisma, "Charisma");
    Import(js, stat_block.SaveSTR, "SaveSTR");
    Import(js, stat_block.SaveDEX, "SaveDEX");
    Import(js, stat_block.SaveCON, "SaveCON");
    Import(js, stat_block.SaveINT, "SaveINT");
    Import(js, stat_block.SaveWIS, "SaveWIS");
    Import(js, stat_block.SaveCHA, "SaveCHA");
    Import(js, stat_block.Crit, "Crit");
    Import(js, stat_block.Actions, "Actions");
    Import(js, stat_block.BonusActions, "BonusActions");


    // Remove any invalid actions or bonus actions.
    stat_block.Actions.erase(std::remove_if(stat_block.Actions.begin(), stat_block.Actions.end(), Invalid),
                             stat_block.Actions.end());
    stat_block.BonusActions.erase(
            std::remove_if(stat_block.BonusActions.begin(), stat_block.BonusActions.end(), Invalid),
            stat_block.BonusActions.end());

    stat_block.CalculateDerivedStats();
}

StatBlock * ParseStatBlock(std::string_view file_name)
{
    nlohmann::json js = OpenJson(file_name);

    StatBlock * block = new StatBlock();

    js.get_to(*block);

    return block;
}

Action * ParseAction(std::string_view file_name)
{
    nlohmann::json js = OpenJson(file_name);

    if (js.empty())
        return nullptr;

    std::string type;
    Import(js, type, "Type");
    Action * action = nullptr;
    if (type == "WeaponAttack")
    {
        auto * weapon_attack = new WeaponAttack();
        js.get_to(*weapon_attack);
        action = weapon_attack;
    }
    else if (type == "MultiAction")
    {
        auto * multi_action = new MultiAction();
        js.get_to(*multi_action);
        action = multi_action;
    }
    else
    {
        if (Out(Warnings)) Out.O() << "Unrecognized action type \"" << type << "\"." << std::endl;
    }

    return action;
}
