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

std::map<std::string, std::weak_ptr<const Action>> Action::ActionMap;

ActorPtrs Action::ChooseTargets(Actor & user) const
{
    ActorPtrs targets;
    if (Target == "Self" || (Target == "SelfInjured" && user.IsInjured()))
        targets.push_back(&user);
    else if (Target == "Enemy")
    {
        Actor & target = user.CurrentArena.OtherGroup(user.Team).FirstConscious();
        if (target.Alive())
            targets.push_back(&target);
    }
    return targets;
}

std::shared_ptr<const Action> Action::Get(const std::string & name)
{
    if (name == "WeaponAttack")
        return Action::GetWeaponAttackProxy();

    auto iter = ActionMap.find(name);
    if (iter == ActionMap.end() || iter->second.expired())
    {
        std::shared_ptr<const Action> new_action(ParseAction(name));
        if (new_action == nullptr)
            return nullptr;
        else
        {
            ActionMap[name] = new_action;
            return new_action;
        }
    }
    else
        return iter->second.lock();
}

std::shared_ptr<const Action> Action::GetWeaponAttackProxy()
{
    static std::shared_ptr<const Action> WeaponAttackProxy = std::shared_ptr<const Action>(new AttackProxy());
    return WeaponAttackProxy;
}

bool WeaponAttack::DoAction(Actor & user, Stat KeyStat) const
{
    ActorPtrs targets = ChooseTargets(user);

    if (targets.empty())
        return false;

    Actor & target = *targets[0];
    OUT_ALL << "    " << user.Name << " attacks " << target << " with " << Name << ". ";

    int stat_mod = user.GetStatMod(KeyStat);
    int attack_mod = stat_mod + user.Stats->Proficiency + user.Stats->AttackBonus;
    int roll = 0;

    OUT_ALL << "Rolled ";
    if (!target.Conscious()) // Advantage if target is not conscious.
        roll = D20.Roll(2, 1);
    else
        roll = D20.Roll();
    OUT_ALL << "+" << attack_mod << "=" << roll + attack_mod << " vs " << target.Stats->AC << " AC ";

    if (roll >= user.Stats->Crit && roll + attack_mod >= target.Stats->AC)
    {
        // crit
        target.InfoStats.CritsReceived++;
        user.InfoStats.CritsLanded++;
        target.InfoStats.AttacksReceived++;
        user.InfoStats.AttacksLanded++;

        OUT_ALL << "critical hit dealing ";

        int damage = DamageDie->RollMod(stat_mod + user.GetDamageBonus(), DamageDiceNum * 2);
        if (target.HasResistance(DamageType))
            OUT_ALL << " reduced to " << damage / 2;
        OUT_ALL << " damage." << std::endl;

        if (damage < 0) OUT_ERROR << "Negative damage dealt." << ERROR_END;

        damage = target.TakeDamage(damage, DamageType);
        user.InfoStats.DamageDone += damage;
        if (!target.Alive()) ++user.InfoStats.Kills;
    }
    else if ((roll + attack_mod >= target.Stats->AC || roll == 20) && roll != 1)
    {
        // hit
        target.InfoStats.AttacksReceived++;
        user.InfoStats.AttacksLanded++;

        int damage = DamageDie->RollMod(stat_mod + user.GetDamageBonus(), DamageDiceNum);

        if (target.HasResistance(DamageType))
            OUT_ALL << " reduced to " << damage / 2;
        OUT_ALL << " damage." << std::endl;

        if (damage < 0) OUT_ERROR << "Negative damage dealt." << ERROR_END;

        damage = target.TakeDamage(damage, DamageType);
        user.InfoStats.DamageDone += damage;
        if (!target.Alive()) ++user.InfoStats.Kills;
    }
    else
    {
        // miss
        target.InfoStats.AttacksAvoided++;
        user.InfoStats.AttacksMissed++;
        OUT_ALL << "Miss! " << std::endl;
    }
    return true;
}

bool MultiAction::DoAction(Actor & user, Stat KeyStat) const
{
    OUT_ALL << "    " << user.Name << " uses " << Name << "." << std::endl;
    bool used_action = false;
    for (const std::shared_ptr<const Action> & action : Actions)
        if (action->DoAction(user, KeyStat))
            used_action = true;

    return used_action;
}

bool SpecialAction::DoAction(Actor & user, Stat KeyStat) const
{
    ActorPtrs targets = ChooseTargets(user);

    if (targets.empty())
        return false;

    OUT_ALL << "    " << user.Name << " uses " << Name;
    if (targets[0]->Name != user.Name || targets.size() > 1)
    {
        if (!targets.empty())
            OUT_ALL << " on ";
        for (int i = 0; i < targets.size(); ++i)
        {
            if (i > 0)
                OUT_ALL << ", ";
            OUT_ALL << targets[i]->Name;
        }
    }
    OUT_ALL << "." << std::endl;

    for (Effect * e : Effects)
        e->DoEffect(user, targets);

    return true;
}

SpecialAction::~SpecialAction()
{
    for (Effect * e : Effects)
        delete e;
}

bool AttackProxy::DoAction(Actor & user, Stat KeyStat) const
{
    for (const auto & ActionRep : user.ActionQueue)
    {
        auto action = dynamic_cast<const WeaponAttack *>(ActionRep.Inst.Action.get());
        if (action != nullptr)
        {
            action->DoAction(user, ActionRep.Inst.KeyStat);
            return true;
        }
    }
    return false;
}

bool Spell::DoAction(Actor & user, Stat KeyStat) const
{
    ActorPtrs targets = ChooseTargets(user);

    if (targets.empty())
        return false;

    ActorPtrs actors_hit;
    ActorPtrs actors_missed;

    OUT_ALL << "    " << user.Name << " casts " << Name;
    if (targets[0]->Name != user.Name || targets.size() > 1)
    {
        if (!targets.empty())
            OUT_ALL << " on ";
        for (int i = 0; i < targets.size(); ++i)
        {
            if (i > 0)
                OUT_ALL << ", ";
            OUT_ALL << targets[i]->Name;
        }
    }
    OUT_ALL << ".";

    for (Actor * target : targets)
    {
        if (SavingThrow != None)
        {
            if (!target->Conscious() && (SavingThrow == Strength || SavingThrow == Dexterity))
            {
                actors_hit.emplace_back(target);
                OUT_ALL << std::endl;
            }
            else
            {
                int dc = 8 + user.GetStatMod(KeyStat) + user.Stats->Proficiency;

                OUT_ALL << " Rolled ";
                int roll = D20.RollMod(target->GetSave(SavingThrow));
                OUT_ALL << " against DC " << dc;

                if (roll < dc)
                {
                    OUT_ALL << " Failed." << std::endl;
                    actors_hit.emplace_back(target);
                }
                else
                {
                    OUT_ALL << " Saved!" << std::endl;
                    actors_missed.emplace_back(target);
                }
            }
        }
        else if (SpellAttack)
        {
            OUT_ALL << " Rolled ";
            int roll = D20.RollMod(user.GetStatMod(KeyStat) + user.Stats->Proficiency);
            OUT_ALL << " vs " << target->Stats->AC << " AC";

            if (roll < target->Stats->AC)
            {
                OUT_ALL << " Miss!" << std::endl;
                actors_missed.emplace_back(target);
            }
            else
            {
                OUT_ALL << std::endl;
                actors_hit.emplace_back(target);
            }
        }
        else
        {
            OUT_ALL << std::endl;
            actors_hit = targets;
        }
    }

    if (targets.empty())
        OUT_ALL << std::endl;

    for (Effect * e : HitEffects)
        e->DoEffect(user, actors_hit);

    for (Effect * e : MissEffects)
        e->DoEffect(user, actors_missed);

    return true;
}

Spell::~Spell()
{
    for (Effect * e : HitEffects)
        delete e;
    for (Effect * e : MissEffects)
        delete e;
}
