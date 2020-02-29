//
// Created by bryan on 2/25/20.
//

#include "Actor.h"
#include "Effect.h"
#include "Dice.h"
#include "Output.h"

// Instantaneous Effects ***********************************************************************************************

void EffectHealing::DoEffect(Actor & user, std::vector<Actor *> & targets) const
{
    for (Actor * target : targets)
    {
        OUT_ALL << "        " << user.Name << " recovers ";
        int mod = HealingBonus + user.GetStatMod(KeyAttribute) + int(AddLevelMod * float(user.Stats->HDNum));
        int roll = HealingDie->RollMod(mod, HealingDieNum);
        OUT_ALL << " HP.";
        int heal_amount = target->Heal(mod + roll);
        if (heal_amount != mod + roll) OUT_ALL << " Reduced to " << heal_amount << ".";
        OUT_ALL << std::endl;
    }
}

void EffectExtraActions::DoEffect(Actor & user, std::vector<Actor *> & targets) const
{
    for (int i = 0; i < ExtraActions; ++i)
        user.TakeAction();
    for (int i = 0; i < ExtraBonusActions; ++i)
        user.TakeBonusAction();
}

// Ongoing Effects *****************************************************************************************************

void OngoingDamageBonus::DoEffect(Actor & user, std::vector<Actor *> & targets) const
{
    for (Actor * target : targets)
    {
        target->AddEffect(this);
        target->TempDamageBonus += BonusDamage;
    }
}

void OngoingDamageBonus::End(Actor * target) const
{
    target->TempDamageBonus -= BonusDamage;
    OUT_ALL << "        " << target->Name << " loses damage bonus of " << BonusDamage;
}

void OngoingResistance::DoEffect(Actor & user, std::vector<Actor *> & targets) const
{
    for (Actor * target : targets)
    {
        target->AddEffect(this);
        target->AddResistance(ResistanceType);
    }
}

void OngoingResistance::End(Actor * target) const
{
    OUT_ALL << "        " << target->Name << " loses resistance to " << ResistanceType;
}

void EffectDamage::DoEffect(Actor & user, std::vector<Actor *> & targets) const
{
    for (Actor * target : targets)
    {
        OUT_ALL << "        " << target->Name << " takes ";
        int mod = DamageBonus;
        int damage = DamageDie->RollMod(mod, DamageDieNum);
        OUT_ALL << " damage." << std::endl;

        if (damage < 0)
        OUT_ERROR << "Negative damage dealt." << ERROR_END;
        target->TakeDamage(damage, DamageType);
    }
}
