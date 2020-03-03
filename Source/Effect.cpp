//
// Created by bryan on 2/25/20.
//

#include "Actor.h"
#include "Action.h"
#include "Effect.h"
#include "Dice.h"
#include "Output.h"

// Instantaneous Effects ***********************************************************************************************

void EffectHealing::DoEffect(Actor & user, Actor * target, bool critical, Stat key_stat) const
{
    OUT_ALL << "        " << user.Name << " recovers ";
    int mod = HealingBonus + user.GetStatMod(KeyAttribute) + int(AddLevelMod * float(user.Stats->HDNum));
    int roll = HealingDie->RollMod(mod, HealingDieNum);
    OUT_ALL << " HP.";
    int heal_amount = target->Heal(mod + roll);
    if (heal_amount != mod + roll) OUT_ALL << " Reduced to " << heal_amount << ".";
    OUT_ALL << std::endl;

}

void EffectExtraActions::DoEffect(Actor & user, Actor * target, bool critical, Stat key_stat) const
{
    for (int i = 0; i < ExtraActions; ++i)
        user.TakeAction();
    for (int i = 0; i < ExtraBonusActions; ++i)
        user.TakeBonusAction();
}

void EffectDamage::DoEffect(Actor & user, Actor * target, bool critical, Stat key_stat) const
{
    OUT_ALL << "        " << target->Name << " takes ";
    int mod = DamageBonus;

    int damage = 0;
    if (critical)
        damage = DamageDie->RollMod(mod, 2 * DamageDieNum);
    else
        damage = DamageDie->RollMod(mod, DamageDieNum);

    if (DamageMultiplier != 1)
    {
        damage = std::floor(float(damage) * DamageMultiplier);
        OUT_ALL << " scaled to " << damage;
    }
    if (target->HasResistance(DamageType))
        OUT_ALL << " resisted to " << damage / 2;
    OUT_ALL << " " << DamageType << " damage." << std::endl;

    if (damage < 0)
    OUT_ERROR << "Negative damage dealt." << ERROR_END;

    target->TakeDamage(damage, DamageType, user);
}

// Ongoing Effects *****************************************************************************************************

void OngoingDamageBonus::DoEffect(Actor & user, Actor * target, bool critical, Stat key_stat) const
{
    target->TempDamageBonus += BonusDamage;
}

void OngoingDamageBonus::EndEffect(Actor * effected) const
{
    effected->TempDamageBonus -= BonusDamage;
    OUT_ALL << "        " << effected->Name << " loses damage bonus of " << BonusDamage;
}

void OngoingResistance::DoEffect(Actor & user, Actor * target, bool critical, Stat key_stat) const
{
    target->AddResistance(ResistanceType);
}

void OngoingResistance::EndEffect(Actor * effected) const
{
    effected->RemoveResistance(ResistanceType);
    OUT_ALL << "        " << effected->Name << " loses resistance to " << ResistanceType;
}

UsableAction::~UsableAction()
{
    // TODO Can't currently track the action instances it creates to delete them.
    delete ActionInst;
}

void UsableAction::DoEffect(Actor & user, Actor * target, bool critical, Stat key_stat) const
{
    auto action_instance = new ActionInstance{ActionInst->Action, ActionInst->Uses, key_stat};
    if (ActionType == "Action")
        target->ActionQueue.push_front(ActionInstance{*action_instance});
    else if (ActionType == "Bonus")
        target->BonusActionQueue.push_front(ActionInstance{*action_instance});

    OUT_ALL << "        " << target->Name << " can now use " << ActionInst->Action->Name << "." << std::endl;
}

bool RemoveFirstAction(ActionList & list, const std::shared_ptr<const Action> & action)
{
    auto prev = list.before_begin();
    auto curr = list.begin();
    while (curr != list.end())
    {
        if (curr->Action == action)
        {
            list.erase_after(prev);
            return true;
        }
        prev = curr++;
    }
    return false;
}

void UsableAction::EndEffect(Actor * effected) const
{
    if (ActionType == "Action")
        RemoveFirstAction(effected->ActionQueue, ActionInst->Action);
    else if (ActionType == "Bonus")
        RemoveFirstAction(effected->BonusActionQueue, ActionInst->Action);
    OUT_ALL << "        " << effected->Name << " can no longer use " << ActionInst->Action->Name << "." << std::endl;
}
