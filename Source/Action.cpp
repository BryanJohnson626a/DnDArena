//
// Created by bryan on 2/23/20.
//

#include <iostream>
#include "Action.h"
#include "Dice.h"
#include "Actor.h"
#include "Arena.h"
#include "Output.h"

Action Action::UnarmedStrike{"Unarmed Strike", Stat::Strength, ActionType::MeleeAttack, 1, D1};

void Action::operator()(Actor & user, Actor & target, std::ostream & out) const
{
    switch (Type)
    {
        case MeleeAttack:
        case RangedAttack:
            Attack(user, target, out);
            break;
        case Spell:
            break;
        case Special:
            break;
    }
}

void Action::Attack(Actor & user, Actor & target, std::ostream & out) const
{
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

    int attack_mod = mod + target.Stats.Proficiency + target.Stats.AttackBonus;
    int roll = D20();

    out << "Rolled " << roll << "+" << attack_mod << "(" << roll + attack_mod << ") vs " << target.Stats.AC << " AC ";

    if (roll + attack_mod > target.Stats.AC)
    {
        // hit
        int damage = mod + target.Stats.DamageBonus;
        for (int i = 0; i < NumDamageDice; ++i)
            damage += DamageDie();

        out << "dealing " << damage << " damage." << std::endl;

        target.TakeDamage(damage);
    } else
    {
        // miss
        out << "Miss! " << std::endl;
    }
}
