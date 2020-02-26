//
// Created by bryan on 2/23/20.
//

#include <ostream>
#include <utility>
#include "Action.h"
#include "Dice.h"
#include "Effect.h"
#include "Actor.h"
#include "Arena.h"
#include "Output.h"
#include "ImportJson.h"

std::map<std::string, Action *> Action::ActionMap;
WeaponAttack UnarmedStrike("Unarmed Strike", Stat::Strength, 1, 1);

ActorPtrs Action::ChooseTargets(Actor & user) const
{
    ActorPtrs targets;
    if (Target == "Self")
        targets.push_back(&user);
    else if (Target == "SelfInjured" && user.IsInjured())
        targets.push_back(&user);
    else if (Target == "Enemy")
    {
        Actor & target = user.CurrentArena.OtherGroup(user.Team).FirstConscious();
        if (target.Alive())
            targets.push_back(&target);
    }
    return targets;
}

bool WeaponAttack::operator()(Actor & user) const
{
    ActorPtrs targets = ChooseTargets(user);

    if (targets.empty())
        return false;

    Actor & target = *targets[0];

    int mod = user.GetStatMod(KeyAttribute);

    if (Out(AllActions)) Out.O() << "    " << user.Name << " attacks " << target << " with " << Name << ". ";

    int attack_mod = mod + target.Stats.Proficiency + target.Stats.AttackBonus;
    int roll = D20.Roll();

    if (Out(AllActions))
        Out.O() << "Rolled " << roll << "->" << roll + attack_mod << " vs " << target.Stats.AC << " AC ";

    if (roll >= user.Stats.Crit && roll + attack_mod > target.Stats.AC)
    {
        // crit
        target.InfoStats.CritsReceived++;
        user.InfoStats.CritsLanded++;
        target.InfoStats.AttacksReceived++;
        user.InfoStats.AttacksLanded++;

        int damage = mod + user.Stats.DamageBonus + DamageDie->Roll(DamageDiceNum * 2);

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

        int damage = mod + user.Stats.DamageBonus + DamageDie->Roll(DamageDiceNum);

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

Action * Action::Get(const std::string & name)
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

bool MultiAction::operator()(Actor & user) const
{
    bool used_action = false;
    for (Action * action : Actions)
        if ((*action)(user))
            used_action = true;

    return used_action;
}

bool SpecialAction::operator()(Actor & user) const
{
    ActorPtrs targets = ChooseTargets(user);

    if (targets.empty())
        return false;

    if (Out(AllActions) && (Target == "Self" || Target == "SelfInjured"))
        Out.O() << "    " << user.Name << " uses " << Name << "." << std::endl;
    else if (Out(AllActions) && Target == "Enemy")
        Out.O() << "    " << user.Name << " uses " << Name << " on " << targets[0] << "." << std::endl;

    for (Effect * e : Effects)
        (*e)(user, targets);

    return true;
}

SpecialAction::~SpecialAction()
{
    for (Effect * e : Effects)
        delete e;
}
