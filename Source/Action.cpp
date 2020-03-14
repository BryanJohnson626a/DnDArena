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
    if (Target == "Self" || (Target == "SelfInjured" && user.IsBloodied()))
        targets.push_back(&user);
    else if (Target == "Enemy")
    {
        // Get conscious enemies first
        targets = user.CurrentArena.OtherGroup(user.Team).AllConscious();

        // Add unconscious but alive enemies after.
        ActorPtrs additional_targets = user.CurrentArena.OtherGroup(user.Team).AllAlive();
        for (Actor * additional_target : additional_targets)
            if (!additional_target->Conscious())
                targets.emplace_back(additional_target);

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

Action::~Action()
{
    for (Effect * e : HitEffects)
        delete e;
    for (Effect * e : MissEffects)
        delete e;
}

bool WeaponAttack::DoAction(Actor & user, Stat KeyStat) const
{
    ActorPtrs targets = ChooseTargets(user);

    if (targets.empty())
        return false;

    for (Actor * target_ptr : targets)
    {
        Actor & target = *target_ptr;

        // Don't try to attack something immune to our damage.
        if (target.HasImmunity(DamageType))
            continue;

        OUT_ALL << "    " << user.Name << " attacks " << target << " with " << Name << ". " ALL_CONT

        int stat_mod = user.GetStatMod(KeyStat);
        int attack_mod = stat_mod + user.Stats->Proficiency + user.Stats->AttackBonus + HitBonus;
        int roll = 0;

        OUT_ALL << "Rolled " ALL_CONT
        if (!target.Conscious()) // Advantage if target is not conscious.
            roll = D20.Roll(2, 1);
        else
            roll = D20.Roll();
        OUT_ALL << "+" << attack_mod << "=" << roll + attack_mod << " vs " << target.Stats->AC << " AC " ALL_CONT

        if ((roll == 20 || roll + attack_mod >= target.Stats->AC) && roll != 1)
        {

            target.InfoStats.AttacksReceived++;
            user.InfoStats.AttacksLanded++;

            int dice_rolled_mult = 1;
            bool crit = false;

            // Larger creatures use bigger weapons.
            if (user.Stats->Size > Medium)
            {
                if (user.Stats->Size == Large)
                    dice_rolled_mult *= 2;
                else if (user.Stats->Size == Huge)
                    dice_rolled_mult *= 3;
                else if (user.Stats->Size == Gargantuan)
                    dice_rolled_mult *= 4;
            }

            if (roll >= user.Stats->Crit && roll + attack_mod >= target.Stats->AC)
            {
                // crit
                crit = true;
                target.InfoStats.CritsReceived++;
                user.InfoStats.CritsLanded++;
                OUT_ALL << "critical hit dealing " ALL_CONT
                dice_rolled_mult *= 2;
            }

            int damage_bonus = stat_mod + user.GetDamageBonus() + DamageBonus;
            int damage = DamageDie->RollMod(damage_bonus, DamageDiceNum * dice_rolled_mult);

            if (target.HasResistance(DamageType))
                OUT_ALL << " resisted to " << damage / 2 ALL_CONT
            OUT_ALL << " " << DamageType << " damage." ALL_ENDL

            if (damage < 0)
            OUT_ERROR << "Negative damage dealt." << ERROR_END;

            damage = target.TakeDamage(damage, DamageType, user);

            for (auto & hit_rider : user.HitRiders)
            {
                hit_rider.Effect->DoEffect(user, &target, crit, KeyStat);
                --hit_rider.Uses;
            }

            for (auto effect : HitEffects)
                effect->DoEffect(user, &target, crit, KeyStat);
        }
        else
        {
            // miss
            target.InfoStats.AttacksAvoided++;
            user.InfoStats.AttacksMissed++;
            OUT_ALL << "Miss! " ALL_ENDL

            for (auto effect : MissEffects)
                effect->DoEffect(user, &target, false, KeyStat);
        }
        return true;
    }
    return false;
}

bool MultiAction::DoAction(Actor & user, Stat KeyStat) const
{
    // OUT_ALL << "    " << user.Name << " uses " << Name << "." << std::endl;
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

    for (auto target : targets)
    {
        OUT_ALL << "    " << user.Name << " uses " << Name ALL_CONT
        if (targets.size() > 1 || target != &user)
            OUT_ALL << " on " << target->Name ALL_CONT
        OUT_ALL << "." ALL_ENDL

        for (Effect * e : HitEffects)
            e->DoEffect(user, target, false, KeyStat);

        user.AddOngoingAction(this, targets, ActorPtrs(), false);

        return true;
    }
    return false;
}

bool AttackProxy::DoAction(Actor & user, Stat KeyStat) const
{
    for (const auto & ActionRep : user.ActionQueue)
    {
        auto action = dynamic_cast<const WeaponAttack *>(ActionRep.Action.get());
        if (action != nullptr)
            return action->DoAction(user, ActionRep.KeyStat);
    }
    return false;
}

bool Spell::DoAction(Actor & user, Stat KeyStat) const
{
    // Don't cast a concentration spell if you already have one active.
    if (Concentration && !user.CanConcentrate())
        return false;

    ActorPtrs targets = ChooseTargets(user);
    ActorPtrs hit_targets;
    ActorPtrs missed_targets;

    if (targets.empty())
        return false;

    int affected = Area;
    for (Actor * target : targets)
    {
        OUT_ALL << "    " << user.Name << " casts " << Name ALL_CONT
        if (targets.size() > 1 || target != &user)
            OUT_ALL << " on " << target->Name ALL_CONT
        OUT_ALL << "." ALL_ENDL

        if (SavingThrow != None)
        {
            if (!target->Conscious() && (SavingThrow == Strength || SavingThrow == Dexterity))
            {
                OUT_ALL << "        " << target->Name << " can't move." ALL_ENDL
                for (Effect * e : HitEffects)
                    e->DoEffect(user, target, false, KeyStat);

                target->InfoStats.SavesFailed++;
                user.InfoStats.ForcedSavesFailed++;
                hit_targets.emplace_back(target);
            }
            else
            {
                int dc = 8 + user.GetStatMod(KeyStat) + user.Stats->Proficiency;

                bool made_save = target->MakeSave(SavingThrow, dc, user, Properties);

                if (made_save)
                {
                    for (Effect * e : MissEffects)
                        e->DoEffect(user, target, false, KeyStat);
                    missed_targets.emplace_back(target);
                }
                else
                {
                    for (Effect * e : HitEffects)
                        e->DoEffect(user, target, false, KeyStat);
                    hit_targets.emplace_back(target);
                }
            }
        }
        else if (SpellAttack)
        {
            OUT_ALL << "        " << user.Name << " rolled " ALL_CONT
            int roll = D20.RollMod(user.GetStatMod(KeyStat) + user.Stats->Proficiency);
            OUT_ALL << " vs " << target->Stats->AC << " AC" ALL_CONT

            if (roll == 20 && roll >= target->Stats->AC)
            {
                OUT_ALL << " Crit!" ALL_ENDL
                for (Effect * e : HitEffects)
                    e->DoEffect(user, target, true, KeyStat);
                hit_targets.emplace_back(target);
            }
            if (roll >= target->Stats->AC)
            {
                OUT_ALL << " Hit." ALL_ENDL
                for (Effect * e : HitEffects)
                    e->DoEffect(user, target, false, KeyStat);
                hit_targets.emplace_back(target);
            }
            else
            {
                OUT_ALL << " Miss!" ALL_ENDL
                for (Effect * e : MissEffects)
                    e->DoEffect(user, target, false, KeyStat);
                missed_targets.emplace_back(target);
            }
        }
        else
        {
            for (Effect * e : HitEffects)
                e->DoEffect(user, target, false, KeyStat);
            hit_targets.emplace_back(target);
        }

        if (--affected == 0)
            break;
    }

    user.AddOngoingAction(this, hit_targets, ActorPtrs(), Concentration);

    return true;
}

OngoingAction::~OngoingAction()
{
    // When a spell instance expires, it must remove any effects it placed on other actors.

    for (Effect * e : Data->HitEffects)
        for (auto a : HitTargets)
            e->EndEffect(a);

    for (Effect * e : Data->MissEffects)
        for (auto a : MissedTargets)
            e->EndEffect(a);
}
