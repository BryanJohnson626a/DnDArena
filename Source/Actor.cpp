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

std::map<std::string, std::weak_ptr<const StatBlock>> StatBlock::StatBlockMap;

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

Actor::Actor(std::string_view name, std::shared_ptr<const StatBlock> stat_block, int team, Arena & arena) :
        Stats(std::move(stat_block)), Team(team), Name(name.data()), CurrentArena(arena)
{
    Initialize();
    Initialize();
    ResetInfo();
}

void Actor::Initialize()
{
    Initiative = 0;
    HPMax = int(std::ceil(float(Stats->HDSize) / 2.f)) * Stats->HDNum;
    HP = HPMax;

    SuccessfulDeathSaves = 0;
    FailedDeathSaves = 0;
    State = DeathState::Conscious;
    ConcentrationSpell = nullptr;

    // Remove any spell instances still active.
    for (auto & spell_inst : OngoingActions)
        delete spell_inst;
    OngoingActions.clear();

    ActionQueue.clear();
    for (auto iter = Stats->Actions.rbegin(); iter != Stats->Actions.rend(); ++iter)
        ActionQueue.push_front(ActionInstance{*iter});

    BonusActionQueue.clear();
    for (auto iter = Stats->BonusActions.rbegin(); iter != Stats->BonusActions.rend(); ++iter)
        BonusActionQueue.push_front(ActionInstance{*iter});

    HitRiders.clear();
    for (auto hit_rider : Stats->HitRiders)
        HitRiders.emplace_back(hit_rider);
}

void Actor::RollInitiative()
{
    OUT_ALL << Name << " rolls initiative: ";
    Initiative = D20.RollMod(Stats->DEX + Stats->InitiativeBonus);
    OUT_ALL << std::endl;
}

void Actor::RollHealth()
{

    OUT_ALL << Name << " rolls health: ";
    if (Stats->Heroic)
    {
        HPMax = Stats->HDSize + Stats->CON;
        HPMax += Stats->HD->RollMod(Stats->CON * (Stats->HDNum - 1) + Stats->HDSize, Stats->HDNum - 1);
        HP = HPMax;
    }
    else
    {
        HPMax = 0;
        HPMax += Stats->HD->RollMod(Stats->CON * Stats->HDNum, Stats->HDNum);
        HP = HPMax;
    }
    OUT_ALL << std::endl;
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

bool Expired(const OngoingAction & spell)
{
    return spell.DurationRemaining <= 0;
}

void Actor::TakeTurn()
{
    if (HP == 0 && Alive())
        DeathSave();

    if (Conscious())
    {
        // Try to use a bonus action, then use an action, then try to use a
        // bonus action again since an action might create or allow a bonus action.
        bool used_bonus_action = TakeBonusAction();
        TakeAction();
        if (!used_bonus_action)
            TakeBonusAction();
    }

    // Tick down the duration of any active spells.
    for (auto & spell_inst : OngoingActions)
    {
        --spell_inst->DurationRemaining;
        if (spell_inst->DurationRemaining <= 0)
        {
            delete spell_inst;
            spell_inst = nullptr;
        }
    }

    // Clean up expired spells.
    if (!OngoingActions.empty())
        OngoingActions.erase(std::remove(OngoingActions.begin(), OngoingActions.end(), nullptr),
                             OngoingActions.end());
}

bool Actor::TakeAction()
{
    for (auto & action : ActionQueue)
        if (action.Uses != 0)
        {
            bool used = action.Action->DoAction(*this, action.KeyStat);
            if (used)
            {
                --action.Uses;
                return true;
            }
        }
    OUT_ALL << "    " << Name << " can't do anything useful." << std::endl;
    return false;
}

bool Actor::TakeBonusAction()
{
    for (auto & action : BonusActionQueue)
        if (action.Uses != 0)
        {
            bool used = action.Action->DoAction(*this, action.KeyStat);
            if (used)
            {
                --action.Uses;
                return true;
            }
        }
    return false;
}

bool Actor::DoWeaponAttack()
{
    for (auto & action : ActionQueue)
        if (dynamic_cast<const WeaponAttack *>(&(*action.Action)) && action.Uses != 0)
        {
            bool used = action.Action->DoAction(*this, action.KeyStat);
            if (used)
            {
                --action.Uses;
                return true;
            }
        }
    return false;
}

ActionInstance * Actor::ChooseAction(ActionList & actions) const
{
    for (auto & action : actions)
        return &action;

    return nullptr;
}

int Actor::TakeDamage(int damage, DamageType damage_type, Actor & damager)
{
    if (HasImmunity(damage_type))
        return 0;

    if (State == Dying)
    {
        FailedDeathSaves += 2;
        DeathCheck();
        if (State == Dead)
        {
            ++InfoStats.Deaths;
            ++damager.InfoStats.Kills;
        }
        return 0;
    }
    // Can't damage something that's already dead.
    if (State == Dead)
    {
        OUT_ALL << "            " << Name << " is already dead." << std::endl;
        return 0;
    }

    if (HasResistance(damage_type))
        damage /= 2;

    HP -= damage;

    if (HP <= 0)
    {
        if (-HP > HPMax || !Stats->Heroic)
        {
            // Damage over what HP was left doesn't count.
            damage += HP;

            HP = 0;
            State = DeathState::Dead;
            if (Stats->Heroic)
            { OUT_ALL << "            " << Name << " dies instantly!" << std::endl; }
            else
            { OUT_ALL << "            " << Name << " dies." << std::endl; }

            ++InfoStats.Deaths;
            ++damager.InfoStats.Kills;
            ++InfoStats.KOed;
            ++damager.InfoStats.KOs;
        }
        else
        {
            // Damage over what HP was left doesn't count.
            damage += HP;

            HP = 0;
            State = DeathState::Dying;
            OUT_ALL << "            " << Name << " has fallen!" << std::endl;

            ++InfoStats.KOed;
            ++damager.InfoStats.KOs;
        }
    }
    else
    {
        OUT_HP << "            " << Name << " has " << HP << "/" << HPMax << " HP remaning." << std::endl;
    }

    damager.InfoStats.DamageDone += damage;
    InfoStats.DamageTaken += damage;

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
        OUT_ALL << "            " << Name << " died!" << std::endl;
    }
    else if (State == DeathState::Dying && SuccessfulDeathSaves >= 3)
    {
        // Stable
        State = DeathState::Stable;
        OUT_ALL << "            " << Name << " has stabilized." << std::endl;
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

bool Actor::IsBloodied() const
{
    return HP < HPMax / 2;
}

bool Actor::AddOngoingAction(const Action * action, ActorPtrs hit_targets,
                             ActorPtrs missed_targets, bool concentration)
{
    if (concentration && !CanConcentrate())
        return false;

    auto a = new OngoingAction{action, action->Duration, std::move(hit_targets), std::move(missed_targets)};

    OngoingActions.emplace_back(a);

    if (concentration)
        ConcentrationSpell = a;

    return true;
}

int Actor::GetDamageBonus() const
{
    return TempDamageBonus + Stats->DamageBonus;
}

template<class T>
bool ItemExists(const std::vector<T> & list, const T & item)
{
    for (const T & i : list)
        if (i == item)
            return true;
    return false;
}

bool Actor::HasResistance(DamageType damage_type) const
{
    return TempResistance[damage_type] > 0 || ItemExists(Stats->Resistances, damage_type);
}

bool Actor::HasImmunity(DamageType damage_type) const
{
    return TempImmunity[damage_type] > 0 || ItemExists(Stats->Immunities, damage_type);
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

Actor::~Actor()
{
    // Active spells are built for specific actors and so must be cleaned up.
    for (auto & spell_inst : OngoingActions)
        delete spell_inst;
}

bool Actor::CanConcentrate() const
{
    return ConcentrationSpell == nullptr;
}

bool Actor::MakeSave(Stat stat, int dc, Actor & instigator, PropertyField properties)
{

    OUT_ALL << "        " << Name << " rolled ";
    int roll;
    if (properties.test(IsSpell) && Stats->MagicResistance)
        roll = D20.RollMod(GetSave(stat), 2, 1);
    else
        roll = D20.RollMod(GetSave(stat));
    OUT_ALL << " against DC " << dc;

    if (roll < dc)
    {
        InfoStats.SavesFailed++;
        instigator.InfoStats.ForcedSavesFailed++;

        OUT_ALL << " Failed." << std::endl;
        return false;
    }
    else
    {
        InfoStats.SavesMade++;
        instigator.InfoStats.ForcedSavesMade++;

        OUT_ALL << " Saved!" << std::endl;
        return true;
    }
}

std::shared_ptr<const StatBlock> StatBlock::Get(const std::string_view & name)
{
    auto iter = StatBlockMap.find(name.data());

    if (iter == StatBlockMap.end() || iter->second.expired())
    {
        std::shared_ptr<const StatBlock> new_stat_block(ParseStatBlock(name));
        if (new_stat_block == nullptr)
        {
            OUT_WARNING << "Failed to load \"" << name << "\"." << WARNING_END;
            return nullptr;
        }
        else
        {
            StatBlockMap[name.data()] = new_stat_block;
            return new_stat_block;
        }
    }
    else
        return iter->second.lock();
}

StatBlock::~StatBlock()
{
    // Rider effects are generated per stat block and need to be cleaned up.
    for (auto rider_effect : HitRiders)
        delete rider_effect.Effect;
}
