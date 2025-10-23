#include "pchScripts.h"
#include "BleedToken.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <Token/TokenInventory.h>

namespace TokenObject
{
    REGISTER_TOKEN(Bleed)
    REGISTER_TOKEN(BleedGrant)
    REGISTER_TOKEN(BleedResistance)

    bool Bleed::CanAdd(CharacterBase* owner) const
    {
        if (owner && false == owner->IsDead())
        {
            auto& tokenInventory = owner->GetTokenInventory();
            if (tokenInventory.HasTokenFromID(BleedResistance::ID))
            {
                tokenInventory.RemoveTokenStackFromID(BleedResistance::ID);
                return false;
            }
            else
            {
                return true;
            }
        }
        return false;
    }
    void Bleed::OnRoundStart(CharacterBase* owner)
    {
        if (owner)
        {
            auto& tokenInventory = owner->GetTokenInventory();
            if (false == owner->IsDead())
            {
                int stackCount = tokenInventory.GetTokenStackFromID(ID);
                int param  = GetTokenParam(0);
                int damage = param * stackCount;
                UmLogger.Log(LogLevel::LEVEL_DEBUG, TokenLog(*owner));
                owner->TakeDamage(damage);
            }
            tokenInventory.RemoveTokenStackFromID(ID);
        }
    }
    void BleedGrant::OnTurnEnd(CharacterBase* owner)
    {
        if (owner)
        {
            auto& tokenInventory = owner->GetTokenInventory();
            tokenInventory.RemoveTokenStackFromID(ID);
        }
    }
    void BleedGrant::OnPostPlayerAttackCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData,
                                                       int& damage)
    {
        if (damage > 0)
        {
            auto&     tokenInventory = targetData.Source.GetTokenInventory();
            const int param          = GetTokenParam(0);
            tokenInventory.AddTokenStackFromID(Bleed::ID, param);
        }
    }
    void BleedGrant::OnPostEnemyAttackCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData,
                                                      int& damage)
    {
        if (damage > 0)
        {
            auto&     tokenInventory = targetData.Source.GetTokenInventory();
            const int param          = GetTokenParam(0);
            tokenInventory.AddTokenStackFromID(Bleed::ID, param);
        }
    }
} // namespace TokenObject