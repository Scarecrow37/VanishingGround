#include "pchScripts.h"
#include "PoisonToken.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <Token/TokenInventory.h>
#include <Particle/ParticleComponent.h>

namespace TokenObject
{
    REGISTER_TOKEN(Poison)
    REGISTER_TOKEN(PoisonGrant)
    REGISTER_TOKEN(PoisonResistance)
    
    void Poison::OnPreTokenAdded(CharacterBase* owner, int tokenID, int& count)
    {
        if (owner && false == owner->IsDead() || count <= 0)
        {
            auto& tokenInventory = owner->GetTokenInventory();
            int   numResistance  = tokenInventory.GetTokenStackFromID(PoisonResistance::ID);
            if (numResistance > 0)
            {
                // 실제로 상쇄되는 개수
                int reduced = std::min(count, numResistance);
                // 토큰 감소
                count -= reduced;
                // 저항 소모
                tokenInventory.RemoveTokenStackFromID(PoisonResistance::ID, reduced);
            }
        }
    }
    void Poison::OnTurnStart(CharacterBase* owner) 
    {
        if (owner)
        {
            auto TakeDamageToken = [owner, this]()
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
                        particle->PlayEffect("poison");
                    }
                }
                tokenInventory.RemoveTokenStackFromID(ID);
            };
             
            if (TokenSystem* system = SingletonComponent<TokenSystem>::GetInstance())
            {
                float delayTime = system->TokenDamageDelayTime;
                UmTime.Invoke(owner, delayTime, TakeDamageToken);
            }
            else
            {
                TakeDamageToken();
            }
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