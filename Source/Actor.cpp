//
// Created by bryan on 2/23/20.
//

#include <cstring>
#include <utility>
#include "Arena.h"
#include "Actor.h"
#include "Action.h"
#include "Dice.h"
#include "Output.h"
#include "ImportJson.h"
#include "Effect.h"

std::map<std::string_view, std::weak_ptr<const StatBlock>> StatBlock::StatBlockMap;

void StatBlock::CalculateDerivedStats()
{
    STR = std::floor((Strength - 10) / 2.0);
    DEX = std::floor((Dexterity - 10) / 2.0);
    CON = std::floor((Constitution - 10) / 2.0);
    INT = std::floor((Intelligence - 10) / 2.0);
    WIS = std::floor((Wisdom - 10) / 2.0);
    CHA = std::floor((Charisma - 10) / 2.0);

    HD = Die::Get(HDSize);

    InitiativeMod = DEX + Proficiency + InitiativeBonus;
    AC = 10 + DEX + ArmorValue + ShieldValue;

    STRSaveMod = STR + (SaveSTR ? Proficiency : 0);
    DEXSaveMod = DEX + (SaveDEX ? Proficiency : 0);
    CONSaveMod = CON + (SaveCON ? Proficiency : 0);
    INTSaveMod = INT + (SaveINT ? Proficiency : 0);
    WISSaveMod = WIS + (SaveWIS ? Proficiency : 0);
    CHASaveMod = CHA + (SaveCHA ? Proficiency : 0);

    Actions.emplace_back(ActionInstance{Action::Get("UnarmedStrike"), -1});
}

void Actor::Initialize()
{
    Initiative = D20.Roll() + Stats->DEX + Stats->InitiativeBonus;

    SuccessfulDeathSaves = 0;
    FailedDeathSaves = 0;
    State = DeathState::Conscious;

    HPMax = 0;
    for (int i = 0; i < Stats->HDNum; ++i)
        HPMax += Stats->HD->Roll() + Stats->CON;

    HP = HPMax;

}

void Actor::FillActionQueues()
{
    ActionQueue.clear();
    for (const ActionInstance & action_instance : Stats->Actions)
        ActionQueue.push_back(ActionRep{action_instance, action_instance.Uses});

    BonusActionQueue.clear();
    for (const ActionInstance & action_instance : Stats->BonusActions)
        BonusActionQueue.push_back(ActionRep{action_instance, action_instance.Uses});
}

Actor::Actor(std::string_view name, std::shared_ptr<const StatBlock> stat_block, int team, Arena & arena) :
        Stats(std::move(stat_block)), Team(team), Name(name.data()), CurrentArena(arena)
{
    FillActionQueues();
    Initialize();
    ResetInfo();
}

const Actor::Statistics & Actor::Info()
{
    return InfoStats;
}

void Actor::ResetInfo()
{
    std::memset(&InfoStats, 0, sizeof(Statistics));
}

bool Actor::Alive() const
{
    return State != DeathState::Dead;
}

bool Actor::Conscious() const
{
    return State == DeathState::Conscious;
}

bool NoDuration(const Actor::EffectRep & e)
{
    return e.duration_remaining <= 0;
}

void Actor::DoRound()
{
    if (HP == 0 && Alive())
        DeathSave();

    if (Conscious())
    {
        TakeBonusAction();
        TakeAction();
    }
    for (auto & e : OngoingEffects)
    {
        --e.duration_remaining;
        if (NoDuration(e))
            e.effect->End(this);
    }
    if (!OngoingEffects.empty())
        OngoingEffects.erase(std::remove_if(OngoingEffects.begin(), OngoingEffects.end(), NoDuration),
                             OngoingEffects.end());
}

void Actor::TakeAction()
{
    auto action = ChooseAction(ActionQueue);
    if (action != nullptr)
    {
        bool used = action->Inst.Action->DoAction(*this, action->Inst.KeyStat);
        if (used)
            --action->UsesRemaining;
    }
}

void Actor::TakeBonusAction()
{
    auto action = ChooseAction(BonusActionQueue);
    if (action != nullptr)
    {
        bool used = action->Inst.Action->DoAction(*this, action->Inst.KeyStat);
        if (used)
            --action->UsesRemaining;
    }
}

Actor::ActionRep * Actor::ChooseAction(std::vector<ActionRep> & actions) const
{
    for (auto & action : actions)
        if (action.UsesRemaining != 0)
            return &action;

    return nullptr;
}

int Actor::TakeDamage(int damage, DamageType damage_type, Actor & damager)
{
    if (HasResistance(damage_type))
        damage /= 2;

    HP -= damage;

    damager.InfoStats.DamageDone += damage;
    InfoStats.DamageTaken += damage;

    if (State == Dying)
    {
        FailedDeathSaves += 2;
        DeathCheck();
        if (State == Dead)
        {
            ++InfoStats.Deaths;
            ++damager.InfoStats.Kills;
        }
    }
    else
    {
        if (HP <= 0)
        {
            if (-HP > HPMax)
            {
                HP = 0;
                State = DeathState::Dead;
                OUT_ALL << "        " << Name << " dies instantly!" << std::endl;

                ++InfoStats.Deaths;
                ++damager.InfoStats.Kills;
            }
            else
            {
                HP = 0;
                State = DeathState::Dying;
                OUT_ALL << "        " << Name << " has fallen!" << std::endl;

                ++InfoStats.Falls;
                ++damager.InfoStats.Downs;
            }
        }
        else
            OUT_ALL << "        " << Name << " has " << HP << "/" << HPMax << " HP remaning." << std::endl;
    }
    return damage;
}

int Actor::CurrentHP() const
{
    return HP;
}

void Actor::DeathSave()
{
    if (SuccessfulDeathSaves < 3 && FailedDeathSaves < 3)
    {
        OUT_ALL << "    " << Name << " makes a death saving throw: ";
        int roll = D20.Roll();
        if (roll == 20)
        {
            // Miracluous Recovery.
            HP = 1;
            SuccessfulDeathSaves = 0;
            FailedDeathSaves = 0;
            State = DeathState::Conscious;
            OUT_ALL << " Critical Success!" << std::endl
                    << "        " << Name << " recovers!" << std::endl;
            return;
        }
        else if (roll >= 10)
        {
            OUT_ALL << " Success." << std::endl;
            ++SuccessfulDeathSaves;
        }
        else if (roll > 1)
        {
            OUT_ALL << " Fail." << std::endl;
            ++FailedDeathSaves;
        }
        else
        {
            OUT_ALL << " Critical fail!" << std::endl;
            FailedDeathSaves += 2;
        }
    }
    DeathCheck();
}

void Actor::DeathCheck()
{
    if (FailedDeathSaves >= 3)
    {
        // Dead
        State = DeathState::Dead;
        InfoStats.Deaths++;
        OUT_ALL << "        " << Name << " died!" << std::endl;
    }
    else if (State == DeathState::Dying && SuccessfulDeathSaves >= 3)
    {
        // Stable
        State = DeathState::Stable;
        OUT_ALL << "        " << Name << " has stabilized." << std::endl;
    }
}

DeathState Actor::GetDeathState() const
{
    return State;
}

int Actor::GetStatMod(enum Stat stat) const
{
    switch (stat)
    {
        default:
            return 0;
        case Strength:
            return Stats->STR;
        case Dexterity:
            return Stats->DEX;
        case Constitution:
            return Stats->CON;
        case Intelligence:
            return Stats->INT;
        case Wisdom:
            return Stats->WIS;
        case Charisma:
            return Stats->CHA;
    }
}

int Actor::Heal(int amount)
{
    if (amount + HP > HPMax)
        amount = HPMax - HP;
    HP += amount;
    return amount;
}

bool Actor::IsInjured() const
{
    return HP < HPMax;
}

void Actor::AddEffect(const OngoingEffect * ongoing_effect)
{
    OngoingEffects.emplace_back(EffectRep{ongoing_effect, ongoing_effect->Duration});
}

int Actor::GetDamageBonus() const
{
    return TempDamageBonus + Stats->DamageBonus;
}

bool Actor::HasResistance(DamageType damage_type) const
{
    return TempResistance[damage_type] > 0;
}

int Actor::GetSave(Stat stat)
{
    switch (stat)
    {
        case Strength:
            return Stats->STRSaveMod;
        case Dexterity:
            return Stats->DEXSaveMod;
        case Constitution:
            return Stats->CONSaveMod;
        case Intelligence:
            return Stats->INTSaveMod;
        case Wisdom:
            return Stats->WISSaveMod;
        case Charisma:
            return Stats->CHASaveMod;
        default:
            return 0;
    }
}

void Actor::AddResistance(DamageType damage_type)
{
    if (damage_type < DamageTypesMax)
        ++TempResistance[damage_type];
    else
    OUT_WARNING << "Invalid damage resistance was applied to " << Name << WARNING_END;
}

void Actor::RemoveResistance(DamageType damage_type)
{
    if (damage_type < DamageTypesMax)
        --TempResistance[damage_type];
    else
    OUT_WARNING << "Invalid damage resistance was removed from " << Name << WARNING_END;
}

int Actor::MaxHP() const
{
    return HPMax;
}

std::shared_ptr<const StatBlock> StatBlock::Get(const std::string_view & name)
{
    auto iter = StatBlockMap.find(name.data());
    if (iter == StatBlockMap.end() || iter->second.expired())
    {
        std::shared_ptr<StatBlock> new_stat_block(ParseStatBlock(name));
        if (new_stat_block == nullptr)
        {
            OUT_WARNING << "Failed to load \"" << name << "\"." << WARNING_END;
            return nullptr;
        }
        else
        {
            StatBlockMap[name] = new_stat_block;
            return new_stat_block;
        }
    }
    else
        return iter->second.lock();
}

