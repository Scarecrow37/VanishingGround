#include "pchScripts.h"
#include "BleedToken.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <Token/TokenInventory.h>
#include <Particle/ParticleComponent.h>

namespace TokenObject
{
    REGISTER_TOKEN(Bleed)
    REGISTER_TOKEN(BleedGrant)
    REGISTER_TOKEN(BleedResistance)

    void Bleed::OnPreTokenAdded(CharacterBase* owner, int tokenID, int& count)
    {
        if (owner && false == owner->IsDead() && count > 0)
        {
            auto& tokenInventory = owner->GetTokenInventory();
            int   numResistance  = tokenInventory.GetTokenStackFromID(BleedResistance::ID);
            if (numResistance > 0)
            {
                // 실제로 상쇄되는 개수
                int reduced = std::min(count, numResistance);
                // 토큰 감소
                count -= reduced;
                // 저항 소모
                tokenInventory.RemoveTokenStackFromID(BleedResistance::ID, reduced);
            }
        }
    }
    void Bleed::OnTurnStart(CharacterBase* owner)
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
                TakeDamage(owner, damage);

                // 이펙트 출력
                if (ParticleComponent* particle = owner->GetParticleComponent())
                {
                    particle->PlayEffect("bleed");
                }
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