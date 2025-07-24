#include "pchScripts.h"
#include "EnemyAction22002.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
namespace EnemyAction
{
    void Action22002::OnActionEnter() 
    {
    }
    bool Action22002::OnActionUpdate() 
    {
        auto player = Player::GetInstance();
        if (player)
        {
            // 플레이어에게 출혈 토큰을 추가합니다.
            auto& system = player->GetTokenInventory();
            system.AddTokenStackFromID(TokenObject::Poison1::ID, 4);
        }
        return true;
    }
    void Action22002::OnActionExit() 
    {
    }
} // namespace EnemyAction