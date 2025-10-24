#include "pchScripts.h"
#include "StunToken.h"

#include <TurnSystem/TurnActor/Character/CharacterBase.h>
#include <Token/TokenInventory.h>
#include <Stats/CharacterStats.h>
namespace TokenObject
{
    REGISTER_TOKEN(Stun)
    REGISTER_TOKEN(StunResistance)

    bool Stun::CanAdd(CharacterBase* owner) const 
    {
        if (owner && false == owner->IsDead())
        {
            auto& tokenInventory = owner->GetTokenInventory();
            if (tokenInventory.HasTokenFromID(StunResistance::ID))
            {
                // 기절 저항을 1 깎는다.
                tokenInventory.RemoveTokenStackFromID(StunResistance::ID);
                // 기절 저항이 있다면 기절 토큰을 추가하지 않는다.
                return false;
            }
            else
            {
                // 기절 저항이 없다면 기절 토큰을 추가한다.
                return true;
            }
        }
        return false;
    }

    void Stun::OnTurnStart(CharacterBase* owner)
    {
        if (owner && false == owner->IsDead())
        {
            // 기절 상태인 경우, 턴을 넘김
            auto& tokenInventory = owner->GetTokenInventory();
            tokenInventory.RemoveTokenFromID(ID); // 기절 토큰 제거
            owner->SetTurnActorFlags(TurnActor::FLAGS_TURN_SKIP); // 턴 스킵 플래그 설정
            auto* stats = owner->GetCharacterStats();
            if (stats)
            {
                // 스턴 저항 수치 갱신은 올림 계산
                const float stunResistance = std::ceilf((float)stats->StunResistance * stats->StunResistanceMultiplier);
                stats->StunResistance = static_cast<int>(stunResistance);
                tokenInventory.AddTokenStackFromID(TokenObject::StunResistance::ID, stats->StunResistance);
            }
        }
    }
} // namespace TokenObject