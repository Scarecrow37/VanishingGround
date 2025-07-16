#include "pchScripts.h"
#include "EnemyAction22001.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
namespace EnemyAction
{
    void Action22001::OnActionEnter() 
    {
    }
    bool Action22001::OnActionUpdate() 
    {
        auto player = Player::GetInstance();
        if (player)
        {
            // 플레이어에게 출혈 토큰을 추가합니다.
            auto& system = player->GetTokenInventory();
            system.AddTokenStackFromID(TokenObject::Bleed::ID, 2);
            system.AddTokenStackFromID(TokenObject::Poison3::ID, 3);
        }
        return true;
    }
    void Action22001::OnActionExit() 
    {
    }
} // namespace EnemyAction