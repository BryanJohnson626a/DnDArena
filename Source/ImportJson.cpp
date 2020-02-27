//
// Created by bryan on 2/24/20.
//

#include <fstream>
#include <algorithm>

#include "ImportJson.h"
#include "Actor.h"
#include "Action.h"
#include "Effect.h"
#include "Dice.h"
#include "Output.h"

Stat ToStat(const std::string & s)
{
    if (s == "Strength") return Strength;
    else if (s == "Dexterity") return Dexterity;
    else if (s == "Constitution") return Constitution;
    else if (s == "Intelligence")return Intelligence;
    else if (s == "Wisdom")return Wisdom;
    else if (s == "Charisma") return Charisma;
    else return None;
}

Action * LoadSpecial(const nlohmann::json & js);

Action * LoadMultiAction(const nlohmann::json & js);

Action * LoadWeaponAttack(const nlohmann::json & json);

nlohmann::json OpenJson(std::string_view file_name)
{
    if (Out(Info)) Out.O() << "Loading \"" << file_name << "\"." << std::endl;

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

void from_json(const nlohmann::json & js, std::shared_ptr<const Action> & action)
{
    std::string name;
    Import(js, name, "Name");
    action = Action::Get(name);
}

Effect * LoadEffect(const nlohmann::json & js);

void from_json(const nlohmann::json & js, const Effect *& effect)
{
    std::string name;
    Import(js, name, "Name");
    effect = LoadEffect(js);
}

void from_json(const nlohmann::json & js, Effect *& effect)
{
    std::string name;
    Import(js, name, "Name");
    effect = LoadEffect(js);
}

void from_json(const nlohmann::json & js, WeaponAttack & action)
{
    Import(js, action.Name, "Name");
    Import(js, action.Target, "Target");
    std::string s;
    Import(js, s, "KeyAttribute");
    action.KeyAttribute = ToStat(s);
    Import(js, action.DamageDiceNum, "DamageDiceNum");
    int die_size;
    Import(js, die_size, "DamageDiceSize");
    action.DamageDie = Die::Get(die_size);
}

void from_json(const nlohmann::json & js, MultiAction & action)
{
    Import(js, action.Name, "Name");
    Import(js, action.Target, "Target");
    Import(js, action.Actions, "Actions");
}

void from_json(const nlohmann::json & js, SpecialAction & action)
{
    Import(js, action.Name, "Name");
    Import(js, action.Target, "Target");
    Import(js, action.Effects, "Effects");
}

void from_json(const nlohmann::json & js, ActionInstance & weighted_action)
{
    Import(js, weighted_action.Action, "Action");
    Import(js, weighted_action.Uses, "Uses");
}

void from_json(const nlohmann::json & js, Effect & effect)
{
    std::string type;
    Import(js, type, "Type");

}

void from_json(const nlohmann::json & js, EffectHealing & effect)
{
    Import(js, effect.HealingDieNum, "HealingDieNum");
    int die_size;
    Import(js, die_size, "HealingDieSize");
    effect.HealingDie = Die::Get(die_size);
    Import(js, effect.HealingBonus, "HealingBonus");
    std::string s;
    Import(js, s, "KeyAttribute");
    effect.KeyAttribute = ToStat(s);
    Import(js, effect.AddLevelMod, "AddLevelMod");
}

void from_json(const nlohmann::json & js, OngoingDamageBonus & effect)
{
    Import(js, effect.Duration, "Duration");
    Import(js, effect.BonusDamage, "BonusDamage");
}

void from_json(const nlohmann::json & js, OngoingResistance & effect)
{
    Import(js, effect.Duration, "Duration");
}

void from_json(const nlohmann::json & js, EffectImmediateExtraActions & effect)
{
    Import(js, effect.ExtraActions, "ExtraActions");
    Import(js, effect.ExtraBonusActions, "ExtraBonusActions");
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

Action * LoadWeaponAttack(const nlohmann::json & js)
{
    auto * weapon_attack = new WeaponAttack();
    js.get_to(*weapon_attack);
    return weapon_attack;
}

Action * LoadMultiAction(const nlohmann::json & js)
{
    auto * multi_action = new MultiAction();
    js.get_to(*multi_action);
    return multi_action;
}

Action * LoadSpecial(const nlohmann::json & js)
{
    auto * special = new SpecialAction();
    js.get_to(*special);
    return special;
}

const Action * ParseAction(std::string_view file_name)
{
    nlohmann::json js = OpenJson(file_name);

    if (js.empty())
        return nullptr;

    std::string type;
    Import(js, type, "Type");

    if (type == "Special") return LoadSpecial(js);
    if (type == "WeaponAttack") return LoadWeaponAttack(js);
    if (type == "MultiAction") return LoadMultiAction(js);

    if (Out(Warnings)) Out.O() << "Unrecognized action type \"" << type << "\"." << std::endl;
    return nullptr;
}

Effect * LoadEffectHealing(const nlohmann::json & js)
{
    auto * effect = new EffectHealing();
    js.get_to(*effect);
    return effect;
}

Effect * LoadEffectImmediateExtraAction(const nlohmann::json & js)
{
    auto * effect = new EffectImmediateExtraActions();
    js.get_to(*effect);
    return effect;
}

OngoingEffect * LoadEffectOngoingDamageBonus(const nlohmann::json & js)
{
    auto * effect = new OngoingDamageBonus();
    js.get_to(*effect);
    return effect;
}

OngoingEffect * LoadEffectOngoingResistance(const nlohmann::json & js)
{
    auto * effect = new OngoingResistance();
    js.get_to(*effect);
    return effect;
}

Effect * LoadEffect(const nlohmann::json & js)
{
    std::string type;
    Import(js, type, "Type");

    if (type == "Healing") return LoadEffectHealing(js);
    if (type == "ImmediateExtraActions") return LoadEffectImmediateExtraAction(js);
    if (type == "DamageBonus") return LoadEffectOngoingDamageBonus(js);
    if (type == "Resistance") return LoadEffectOngoingResistance(js);

    if (Out(Warnings)) Out.O() << "Unrecognized effect type \"" << type << "\"." << std::endl;
    return nullptr;
}
