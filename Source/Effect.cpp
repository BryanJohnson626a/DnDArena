//
// Created by bryan on 2/25/20.
//

#include "Actor.h"
#include "Effect.h"
#include "Dice.h"
#include "Output.h"


void EffectHealing::operator()(Actor & user, std::vector<Actor *> & targets) const
{
    for (Actor * target : targets)
    {
        int heal_amount = HealingBonus + user.GetStatMod(KeyAttribute)
                          + AddLevelMod * user.Stats->HDNum
                          + HealingDie->Roll(HealingDieNum);
        heal_amount = target->Heal(heal_amount);
        if (Out(AllActions)) Out.O() << "        " << user.Name << " recovers " << heal_amount << " HP." << std::endl;
    }
}

void EffectImmediateExtraActions::operator()(Actor & user, std::vector<Actor *> & targets) const
{
    for (int i = 0; i < ExtraActions; ++i)
        user.TakeAction();
    for (int i = 0; i < ExtraBonusActions; ++i)
        user.TakeBonusAction();
}

void OngoingDamageBonus::operator()(Actor & user, std::vector<Actor *> & targets) const
{
    for(Actor * target : targets)
    {
        target->AddEffect(this);
        target->TempDamageBonus += BonusDamage;
    }
}

void OngoingDamageBonus::End(Actor * target) const
{
    target->TempDamageBonus -= BonusDamage;
}

void OngoingResistance::operator()(Actor & user, std::vector<Actor *> & targets) const
{
    for(Actor * target : targets)
    {
        target->AddEffect(this);
        target->TempResistance += 1;
    }
}

void OngoingResistance::End(Actor * target) const
{
    target->TempResistance -= 1;
}
