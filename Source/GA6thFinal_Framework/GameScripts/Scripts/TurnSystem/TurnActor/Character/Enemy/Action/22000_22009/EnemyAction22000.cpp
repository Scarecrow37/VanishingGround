#include "pchScripts.h"
#include "EnemyAction22000.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
namespace EnemyAction
{
    void Action22000::OnActionEnter() {}

    bool Action22000::OnActionUpdate()
    {
        auto player = Player::GetInstance();
        if (player)
        {
            // 플레이어에게 출혈 토큰을 추가합니다.
            auto& system = player->GetTokenInventory();
            system.AddTokenStackFromID(TokenObject::Bleed1::ID, 2);
            system.AddTokenStackFromID(TokenObject::Bleed2::ID, 2);
            system.AddTokenStackFromID(TokenObject::Poison2::ID, 2);
        }
        return true;
    }

    void Action22000::OnActionExit() {}
} // namespace EnemyAction