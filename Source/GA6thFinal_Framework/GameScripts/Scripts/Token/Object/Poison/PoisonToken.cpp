#include "pchScripts.h"
#include "PoisonToken.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <Token/TokenInventory.h>

namespace TokenObject
{
    REGISTER_TOKEN(Poison)
    REGISTER_TOKEN(PoisonGrant)
    REGISTER_TOKEN(PoisonResistance)

    bool Poison::CanAdd(CharacterBase* owner) const
    {
        if (owner && false == owner->IsDead())
        {
            auto& tokenInventory = owner->GetTokenInventory();
            if (tokenInventory.HasTokenFromID(PoisonResistance::ID))
            {
                tokenInventory.RemoveTokenStackFromID(PoisonResistance::ID);
                return false;
            }
            else
            {
                return true;
            }
        }
        return false;
    }
    void Poison::OnTurnStart(CharacterBase* owner) 
    {
        if (owner)
        {
            auto& tokenInventory = owner->GetTokenInventory();
            if (false == owner->IsDead())
            {
                int stackCount = tokenInventory.GetTokenStackFromID(ID);
                int param  = GetTokenParam(0);
                int damage = param * stackCount;
                UmLogger.Log(LogLevel::LEVEL_TRACE, TokenLog(*owner));
                owner->TakeDamage(damage);
            }
            tokenInventory.RemoveTokenStackFromID(ID);
        }
    }
    void PoisonGrant::OnTurnEnd(CharacterBase* owner) 
    {
        if (owner)
        {
            auto& tokenInventory = owner->GetTokenInventory();
            tokenInventory.RemoveTokenStackFromID(ID);
        }
    }
    void PoisonGrant::OnPostPlayerAttackCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData,
                                                       int& damage)
    {
        if (damage > 0)
        {
            auto&     tokenInventory = targetData.Source.GetTokenInventory();
            const int param          = GetTokenParam(0);
            tokenInventory.AddTokenStackFromID(Poison::ID, param);
        }
    }
    void PoisonGrant::OnPostEnemyAttackCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData,
                                                      int& damage)
    {
        if (damage > 0)
        {
            auto&     tokenInventory = targetData.Source.GetTokenInventory();
            const int param          = GetTokenParam(0);
            tokenInventory.AddTokenStackFromID(Poison::ID, param);
        }
    }
} // namespace TokenObject