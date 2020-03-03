//
// Created by bryan on 2/24/20.
//

#include <fstream>
#include <algorithm>
#include <filesystem>

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

DamageType ToDamageType(const std::string & s)
{
    if (s == "Slashing") return Slashing;
    else if (s == "Bludgeoning") return Bludgeoning;
    else if (s == "Piercing") return Piercing;
    else if (s == "Fire") return Fire;
    else if (s == "Cold") return Cold;
    else if (s == "Lightning") return Lightning;
    else if (s == "Thunder") return Thunder;
    else if (s == "Acid") return Acid;
    else if (s == "Poison") return Poison;
    else if (s == "Necrotic") return Necrotic;
    else if (s == "Radiant") return Radiant;
    else if (s == "Psychic") return Psychic;
    else if (s == "Force") return Force;
    else return InvalidDamageType;
}

SizeCategory ToSizeCategory(const std::string & s)
{
    if (s == "NoSize") return NoSize;
    else if (s == "Tiny") return Tiny;
    else if (s == "Small") return Small;
    else if (s == "Medium") return Medium;
    else if (s == "Large") return Large;
    else if (s == "Huge") return Huge;
    else if (s == "Gargantuan") return Gargantuan;
    else return InvalidSize;
}

Action * LoadSpecial(const nlohmann::json & js);
Action * LoadMultiAction(const nlohmann::json & js);
Action * LoadWeaponAttack(const nlohmann::json & json);

nlohmann::json OpenJson(const std::filesystem::path & path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        OUT_WARNING << "Warning: \"" << path << "\": " << strerror(errno) << WARNING_END;
        return nlohmann::json();
    }
    nlohmann::json js;
    try
    {
        js = nlohmann::json::parse(file);
    }
    catch (nlohmann::json::exception & e)
    {
        OUT_WARNING << "Warning: \"" << path << "\": " << e.what() << WARNING_END;
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

Action * LoadAction(const nlohmann::json & js);

void from_json(const nlohmann::json & js, const Action *& action)
{
    std::string name;
    Import(js, name, "Name");
    action = LoadAction(js);
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

void from_json(const nlohmann::json & js, Effect & effect)
{
    std::string type;
    Import(js, type, "Type");
}

void from_json(const nlohmann::json & js, ActionInstance & action_instance)
{
    Import(js, action_instance.Action, "Action");
    Import(js, action_instance.Uses, "Uses");
    std::string key_stat;
    Import(js, key_stat, "KeyStat");
    action_instance.KeyStat = ToStat(key_stat);
}

void from_json(const nlohmann::json & js, RiderEffect & rider_effect)
{
    Import(js, rider_effect.Effect, "Effect");
    Import(js, rider_effect.Uses, "Uses");
}

bool Invalid(const ActionInstance & a)
{
    return a.Action == nullptr;
}

void from_json(const nlohmann::json & js, StatBlock *& stat_block)
{
    delete stat_block;
    stat_block = new StatBlock;

    Import(js, stat_block->Name, "Name");
    Import(js, stat_block->Heroic, "Heroic");
    std::string size_category;
    Import(js, size_category, "Size");
    stat_block->Size = ToSizeCategory(size_category);
    Import(js, stat_block->Type, "Type");
    Import(js, stat_block->HDNum, "HDNum");
    Import(js, stat_block->HDSize, "HDSize");
    Import(js, stat_block->Proficiency, "Proficiency");
    Import(js, stat_block->InitiativeBonus, "InitiativeBonus");
    Import(js, stat_block->ArmorValue, "ArmorValue");
    Import(js, stat_block->ShieldValue, "ShieldValue");
    Import(js, stat_block->AttackBonus, "AttackBonus");
    Import(js, stat_block->DamageBonus, "DamageBonus");
    Import(js, stat_block->Strength, "Strength");
    Import(js, stat_block->Dexterity, "Dexterity");
    Import(js, stat_block->Constitution, "Constitution");
    Import(js, stat_block->Intelligence, "Intelligence");
    Import(js, stat_block->Wisdom, "Wisdom");
    Import(js, stat_block->Charisma, "Charisma");
    Import(js, stat_block->SaveSTR, "SaveSTR");
    Import(js, stat_block->SaveDEX, "SaveDEX");
    Import(js, stat_block->SaveCON, "SaveCON");
    Import(js, stat_block->SaveINT, "SaveINT");
    Import(js, stat_block->SaveWIS, "SaveWIS");
    Import(js, stat_block->SaveCHA, "SaveCHA");
    Import(js, stat_block->Crit, "Crit");
    Import(js, stat_block->Actions, "Actions");
    Import(js, stat_block->BonusActions, "BonusActions");
    Import(js, stat_block->HitRiders, "HitRiders");


    // Remove any invalid actions or bonus actions.
    stat_block->Actions.erase(std::remove_if(stat_block->Actions.begin(), stat_block->Actions.end(), Invalid),
                              stat_block->Actions.end());
    stat_block->BonusActions.erase(
            std::remove_if(stat_block->BonusActions.begin(), stat_block->BonusActions.end(), Invalid),
            stat_block->BonusActions.end());

    stat_block->CalculateDerivedStats();
}

StatBlock * ParseStatBlock(std::string_view name)
{
    OUT_INFO << "Loading \"" << name << "\"." << INFO_END

    std::string path = "./Resources/StatBlocks/";

    for (const auto & entry : std::filesystem::directory_iterator(path))
    {
        nlohmann::json js = OpenJson(entry.path());
        try
        {
            StatBlock * p = nullptr;
            Import(js, p, name.data());
            if (p != nullptr)
                return p;
        }
        catch (nlohmann::json::exception & e)
        {
            OUT_ERROR << name << ": " << e.what() << ERROR_END;
            return nullptr;
        }
    }
    OUT_WARNING << name << " not found." << WARNING_END;
    return nullptr;
}

const Action * ParseAction(std::string_view name)
{
    OUT_INFO << "Loading \"" << name << "\"." << INFO_END

    std::string path = "./Resources/Actions/";

    for (const auto & entry : std::filesystem::directory_iterator(path))
    {
        nlohmann::json js = OpenJson(entry.path());
        try
        {
            const Action * p = nullptr;
            Import(js, p, name.data());
            if (p != nullptr)
                return p;
        }
        catch (nlohmann::json::exception & e)
        {
            OUT_ERROR << "Error: \"" << name << "\": " << e.what() << ERROR_END;
            return nullptr;
        }
    }
    OUT_WARNING << "Warning: \"" << name << "\" not found." << WARNING_END;
    return nullptr;
}

// Load Actions ***********************************************************************************


void from_json(const nlohmann::json & js, WeaponAttack & action)
{
    Import(js, action.DamageDiceNum, "DamageDiceNum");
    int die_size;
    Import(js, die_size, "DamageDiceSize");
    action.DamageDie = Die::Get(die_size);
    std::string damage_type;
    Import(js, damage_type, "DamageType");
    action.DamageType = ToDamageType(damage_type);
}

Action * LoadWeaponAttack(const nlohmann::json & js)
{
    auto * weapon_attack = new WeaponAttack();
    js.get_to(*weapon_attack);
    return weapon_attack;
}

void from_json(const nlohmann::json & js, MultiAction & action)
{
    Import(js, action.Actions, "Actions");
}

Action * LoadMultiAction(const nlohmann::json & js)
{
    auto * multi_action = new MultiAction();
    js.get_to(*multi_action);
    return multi_action;
}

void from_json(const nlohmann::json & js, SpecialAction & action)
{
}

Action * LoadSpecial(const nlohmann::json & js)
{
    auto * special = new SpecialAction();
    js.get_to(*special);
    return special;
}

void from_json(const nlohmann::json & js, Spell & action)
{

    std::string stat_name;
    Import(js, stat_name, "SavingThrow");
    action.SavingThrow = ToStat(stat_name);
    Import(js, action.SpellAttack, "SpellAttack");
    Import(js, action.Concentration, "Concentration");
}

Action * LoadSpell(const nlohmann::json & js)
{
    auto * spell = new Spell();
    js.get_to(*spell);
    return spell;
}

Action * LoadAction(const nlohmann::json & js)
{
    std::string type;
    Import(js, type, "Type");

    Action * action = nullptr;
    if (type == "Spell") action = LoadSpell(js);
    else if (type == "WeaponAttack") action = LoadWeaponAttack(js);
    else if (type == "Special") action = LoadSpecial(js);
    else if (type == "MultiAction") action = LoadMultiAction(js);

    if (action == nullptr)
    {
        OUT_WARNING << "Unrecognized action type \"" << type << "\"." << WARNING_END;
        return nullptr;
    }

    Import(js, action->Name, "Name");
    Import(js, action->Target, "Target");
    Import(js, action->Area, "Area");
    Import(js, action->Duration, "Duration");
    Import(js, action->HitEffects, "HitEffects");
    Import(js, action->MissEffects, "MissEffects");

    // Scale down raw scale into likely number of targets hit.
    if (action->Area > 1)
        action->Area = (int) std::round(std::pow(float(action->Area) / 3.14f, 0.5f) * 2);

    return action;
}

// Load Effects ***********************************************************************************


void from_json(const nlohmann::json & js, EffectHealing & effect)
{
    Import(js, effect.HealingDieNum, "HealingDieNum");
    int die_size;
    Import(js, die_size, "HealingDieSize");
    effect.HealingDie = Die::Get(die_size);
    Import(js, effect.HealingBonus, "HealingBonus");
    Import(js, effect.AddLevelMod, "AddLevelMod");
}

Effect * LoadEffectHealing(const nlohmann::json & js)
{
    auto * effect = new EffectHealing();
    js.get_to(*effect);
    return effect;
}

Effect * LoadEffectImmediateExtraAction(const nlohmann::json & js)
{
    auto * effect = new EffectExtraActions();
    js.get_to(*effect);
    return effect;
}

void from_json(const nlohmann::json & js, EffectExtraActions & effect)
{
    Import(js, effect.ExtraActions, "ExtraActions");
    Import(js, effect.ExtraBonusActions, "ExtraBonusActions");
}

Effect * LoadEffectUsableAction(const nlohmann::json & js)
{
    auto * effect = new UsableAction();
    js.get_to(*effect);
    return effect;
}

void from_json(const nlohmann::json & js, UsableAction & effect)
{
    auto * action_inst = new ActionInstance;
    Import(js, action_inst->Action, "Action");
    Import(js, action_inst->Uses, "Uses");
    effect.ActionInst = action_inst;

    Import(js, effect.ActionType, "ActionType");
}

void from_json(const nlohmann::json & js, OngoingDamageBonus & effect)
{
    Import(js, effect.BonusDamage, "BonusDamage");
}

Effect * LoadEffectOngoingDamageBonus(const nlohmann::json & js)
{
    auto * effect = new OngoingDamageBonus();
    js.get_to(*effect);
    return effect;
}

void from_json(const nlohmann::json & js, OngoingResistance & effect)
{
    std::string damage_type;
    Import(js, damage_type, "ResistanceType");
    effect.ResistanceType = ToDamageType(damage_type);
}

Effect * LoadEffectOngoingResistance(const nlohmann::json & js)
{
    auto * effect = new OngoingResistance();
    js.get_to(*effect);
    return effect;
}

void from_json(const nlohmann::json & js, EffectDamage & effect)
{
    Import(js, effect.DamageDieNum, "DamageDieNum");
    int die_size;
    Import(js, die_size, "DamageDieSize");
    effect.DamageDie = Die::Get(die_size);
    Import(js, effect.DamageBonus, "DamageBonus");
    Import(js, effect.DamageMultiplier, "DamageMultiplier");
    std::string damage_type;
    Import(js, damage_type, "DamageType");
    effect.DamageType = ToDamageType(damage_type);
}

Effect * LoadEffectDamage(const nlohmann::json & js)
{
    auto * effect = new EffectDamage();
    js.get_to(*effect);
    return effect;
}

Effect * LoadEffect(const nlohmann::json & js)
{
    std::string type;
    Import(js, type, "Type");

    Effect * effect = nullptr;
    if (type == "Damage") effect = LoadEffectDamage(js);
    else if (type == "Healing") effect = LoadEffectHealing(js);
    else if (type == "ImmediateExtraActions") effect = LoadEffectImmediateExtraAction(js);
    else if (type == "DamageBonus") effect = LoadEffectOngoingDamageBonus(js);
    else if (type == "Resistance") effect = LoadEffectOngoingResistance(js);
    else if (type == "UsableAction") effect = LoadEffectUsableAction(js);

    if (effect == nullptr)
    {
        OUT_WARNING << "Unrecognized effect type \"" << type << "\"." << WARNING_END;
        return effect;
    }

    return effect;
}
