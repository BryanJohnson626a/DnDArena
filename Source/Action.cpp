//
// Created by bryan on 2/23/20.
//

#include <ostream>
#include <utility>
#include "Action.h"
#include "Dice.h"
#include "Actor.h"
#include "Arena.h"
#include "Output.h"
#include "ImportJson.h"

std::map<std::string, Action *> Action::ActionMap;
WeaponAttack UnarmedStrike("Unarmed Strike", Stat::Strength, 1, 1);


bool WeaponAttack::operator()(Actor & user, Arena & arena) const
{
    Actor & target = arena.OtherGroup(user.Team).FirstConscious();
    if (!target.Alive())
        return false;

    int mod = 0;
    switch (KeyAttribute)
    {
        case Strength:
            mod += user.Stats.STR;
            break;
        case Dexterity:
            mod += user.Stats.DEX;
            break;
        case Constitution:
            mod += user.Stats.CON;
            break;
        case Intelligence:
            mod += user.Stats.INT;
            break;
        case Wisdom:
            mod += user.Stats.WIS;
            break;
        case Charisma:
            mod += user.Stats.CHA;
            break;
    }

    if (Out(AllActions)) Out.O() << "    " << user.Name << " attacks " << target << " with " << Name << ". ";

    int attack_mod = mod + target.Stats.Proficiency + target.Stats.AttackBonus;
    int roll = D20();

    if (Out(AllActions))
        Out.O() << "Rolled " << roll << "->" << roll + attack_mod << " vs " << target.Stats.AC << " AC ";

    if (roll >= user.Stats.Crit && roll + attack_mod > target.Stats.AC)
    {
        // crit
        target.InfoStats.CritsReceived++;
        user.InfoStats.CritsLanded++;
        target.InfoStats.AttacksReceived++;
        user.InfoStats.AttacksLanded++;

        int damage = mod + user.Stats.DamageBonus;
        for (int i = 0; i < DamageDiceNum * 2; ++i)
            damage += Roll(DamageDie);
        if (Out(AllActions)) Out.O() << "critical hit dealing " << damage << " damage!" << std::endl;

        target.TakeDamage(damage);
        user.InfoStats.DamageDone += damage;
        if (!target.Alive()) ++user.InfoStats.Kills;
    }
    else if ((roll + attack_mod > target.Stats.AC || roll == 20) && roll != 1)
    {
        // hit
        target.InfoStats.AttacksReceived++;
        user.InfoStats.AttacksLanded++;
        int damage = mod + user.Stats.DamageBonus;
        for (int i = 0; i < DamageDiceNum; ++i)
            damage += Roll(DamageDie);

        if (Out(AllActions)) Out.O() << "dealing " << damage << " damage." << std::endl;

        target.TakeDamage(damage);
        user.InfoStats.DamageDone += damage;
        if (!target.Alive()) ++user.InfoStats.Kills;
    }
    else
    {
        // miss
        target.InfoStats.AttacksAvoided++;
        user.InfoStats.AttacksMissed++;
        if (Out(AllActions)) Out.O() << "Miss! " << std::endl;
    }
    return true;
}

WeaponAttack::WeaponAttack(std::string name, Stat key_attribute, int number_of_damage_dice, int size_of_damage_dice) :
        Action(std::move(name)), KeyAttribute(key_attribute), DamageDiceNum(number_of_damage_dice),
        DamageDie(Die::Get(size_of_damage_dice))
{}

Action::Action(std::string name) : Name(std::move(name))
{
    ActionMap.insert({Name, this});
}

Action * Action::Get(std::string name)
{
    auto iter = ActionMap.find(name);
    if (iter == ActionMap.end())
    {
        Action * new_action = ParseAction(name);
        if (new_action == nullptr)
            return nullptr;
        else
        {
            ActionMap.insert({name, new_action});
            return new_action;
        }
    }
    else
        return iter->second;
}

bool MultiAction::operator()(Actor & user, Arena & arena) const
{
    for (Action * action : Actions)
        (*action)(user, arena);
    return true;
}
