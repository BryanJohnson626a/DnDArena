//
// Created by bryan on 2/25/20.
//

#include "Actor.h"
#include "Effect.h"
#include "Dice.h"
#include "Output.h"

// Instantaneous Effects ***********************************************************************************************

void EffectHealing::DoEffect(Actor & user, Actor * target, bool critical) const
{
    OUT_ALL << "        " << user.Name << " recovers ";
    int mod = HealingBonus + user.GetStatMod(KeyAttribute) + int(AddLevelMod * float(user.Stats->HDNum));
    int roll = HealingDie->RollMod(mod, HealingDieNum);
    OUT_ALL << " HP.";
    int heal_amount = target->Heal(mod + roll);
    if (heal_amount != mod + roll) OUT_ALL << " Reduced to " << heal_amount << ".";
    OUT_ALL << std::endl;

}

void EffectExtraActions::DoEffect(Actor & user, Actor * target, bool critical) const
{
    for (int i = 0; i < ExtraActions; ++i)
        user.TakeAction();
    for (int i = 0; i < ExtraBonusActions; ++i)
        user.TakeBonusAction();
}

// Ongoing Effects *****************************************************************************************************

void OngoingDamageBonus::DoEffect(Actor & user, Actor * target, bool critical) const
{
    target->AddEffect(this);
    target->TempDamageBonus += BonusDamage;

}

void OngoingDamageBonus::End(Actor * effected) const
{
    effected->TempDamageBonus -= BonusDamage;
    OUT_ALL << "        " << effected->Name << " loses damage bonus of " << BonusDamage;
}

void OngoingResistance::DoEffect(Actor & user, Actor * target, bool critical) const
{
    target->AddEffect(this);
    target->AddResistance(ResistanceType);
}

void OngoingResistance::End(Actor * effected) const
{
    OUT_ALL << "        " << effected->Name << " loses resistance to " << ResistanceType;
}

void EffectDamage::DoEffect(Actor & user, Actor * target, bool critical) const
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
