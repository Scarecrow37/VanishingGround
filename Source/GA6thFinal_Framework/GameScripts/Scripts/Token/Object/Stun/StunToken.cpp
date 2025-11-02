#include "pchScripts.h"
#include "StunToken.h"

#include <TurnSystem/TurnActor/Character/CharacterBase.h>
#include <Token/TokenInventory.h>
#include <Stats/CharacterStats.h>
#include "ContentMath/ContentMath.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"

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
                // 스턴 액션 호출
                if (TurnMode* mode = SingletonComponent<TurnMode>::GetInstance())
                {
                    if (typeid(Player) == typeid(*owner))
                    {
                        Player* player = static_cast<Player*>(owner);
                        mode->ApplyActions([player](TurnAction& action) { action.OnPlayerStun(*player); });
                    }
                    else if (typeid(Enemy) == typeid(*owner))
                    {
                        Enemy* enemy = static_cast<Enemy*>(owner);
                        mode->ApplyActions([enemy](TurnAction& action) { action.OnEnemyStun(*enemy); });
                    }
                }
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
                stats->StunResistance = ContentMath::CeilPercentage(
                    stats->StunResistance, static_cast<int>(stats->StunResistanceMultiplier * 100.0f));
                tokenInventory.AddTokenStackFromID(TokenObject::StunResistance::ID, stats->StunResistance);
            }
            UmLogger.Log(LogLevel::LEVEL_TRACE, TokenLog(*owner));
        }
    }
} // namespace TokenObject