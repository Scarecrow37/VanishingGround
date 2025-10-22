#include "pchScripts.h"
#include "MonsterActionExecutionShot.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"
#include "Token/Object/Bleed/BleedToken.h"

REGISTER_MONSTER_ACTION(Monster::Action::ExecutionShot)
namespace Monster
{
    namespace Action
    {
        ExecutionShot::ExecutionShot() : Base("Attack1") {}
        ExecutionShot::~ExecutionShot() = default;

        void ExecutionShot::OnActionEnter() {}

        void ExecutionShot::OnActionUpdate() {}

        void ExecutionShot::OnActionExit() {}

        void ExecutionShot::OnActionReset() {}

        void ExecutionShot::OnNotifiedAnimationEvent(const Timeline::EventContext* context)
        {
            const std::string& label = context->GetLabel();
            if ("Attack" == label)
            {
                Attack();
            }
        }

        /*
        플레이어가 출혈(205000) 토큰 소지 시 선언한다.
        플레이어가 출혈 상태가 아닐 시 
        플레이어에게 S_Param(1)의 데미지로 피해를 준다.
        플레이어가 출혈 상태일 시
        S_Param(2)의 데미지로 플레이어에게 피해를 준다.
        */
        void ExecutionShot::Attack() 
        {
            if (TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance())
            {
                if (Player* player = turnMode->GetPlayer())
                {
                    int damage = 0;
                    // 출혈 상태 이상 유무에 따라 다른 데미지로 공격
                    TokenInventory& tokenInventory = player->GetTokenInventory();
                    bool isBleed = tokenInventory.HasTokenFromID(TokenObject::Bleed::ID);
                    if (isBleed)
                    {
                        damage = GetActionParam(2).Param;
                    }
                    else
                    {
                        damage = GetActionParam(1).Param;
                    }
                    ProcessBattle(damage);
                }
            }
        }
    } // namespace Action
} // namespace Monster