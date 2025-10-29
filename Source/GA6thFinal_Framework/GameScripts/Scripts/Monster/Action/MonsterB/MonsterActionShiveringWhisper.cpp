#include "pchScripts.h"
#include "MonsterActionShiveringWhisper.h"
#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>

REGISTER_MONSTER_ACTION(Monster::Action::ShiveringWhisper)
namespace Monster
{
    namespace Action
    {
        ShiveringWhisper::ShiveringWhisper() : Base("Attack1") {}
        ShiveringWhisper::~ShiveringWhisper() = default;
        void ShiveringWhisper::OnActionEnter() {}
        void ShiveringWhisper::OnActionUpdate() {}
        void ShiveringWhisper::OnActionExit() {}
        void ShiveringWhisper::OnActionReset() {}
        void ShiveringWhisper::OnNotifiedAnimationEvent(const Timeline::EventContext* context)
        {
            const std::string& label = context->GetLabel();
            if ("Attack" == label)
            {
                Attack();
            }
        }

        /*
        플레이어에게 S_Param(1)의 데미지로 피해를 준다.
        공격 시 플레이어에게 T_Param(1)을 부여한다.
        자신에게 T_Param(2)를 부여한다.
        */
        void ShiveringWhisper::Attack() 
        {
            if (auto player = GetTargetFromString("Player").lock())
            {
                TokenParam      tokenParam     = GetTokenParam(1);
                TokenInventory& tokenInventory = player->GetTokenInventory();
                tokenInventory.AddTokenStackFromID(tokenParam.TokenID, tokenParam.Count);
            }
            if (Enemy* owner = GetOwnerEnemy())
            {
                TokenParam      tokenParam     = GetTokenParam(2);
                TokenInventory& tokenInventory = owner->GetTokenInventory();
                tokenInventory.AddTokenStackFromID(tokenParam.TokenID, tokenParam.Count);
            }
            ActionParam damage = GetActionParam(1);
            ProcessBattle(damage.Param);
        }
    }
} // namespace Monster