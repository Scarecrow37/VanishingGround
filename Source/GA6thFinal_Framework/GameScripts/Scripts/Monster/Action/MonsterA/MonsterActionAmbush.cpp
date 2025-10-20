#include "pchScripts.h"
#include "MonsterActionAmbush.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"

REGISTER_MONSTER_ACTION(Monster::Action::Ambush)
namespace Monster
{
    namespace Action
    {
        Ambush::Ambush() : Base("Attack1") {}
        Ambush::~Ambush() = default;

        void Ambush::OnActionEnter() {}

        void Ambush::OnActionUpdate() {}

        void Ambush::OnActionExit() {}

        void Ambush::OnActionReset() {}

        void Ambush::OnNotifiedAnimationEvent(const Timeline::EventContext* context) 
        {
            const std::string& label = context->GetLabel();
            if ("Attack" == label)
            {
                Attack();
            }
        }

        /*
        플레이어게 S_Param(1)의 데미지로 피해를 준다.
        공격 시 플레이어에게 T_Param(1)을 부여한다.
        */
        void Ambush::Attack() 
        {
            ActionParam damage = GetActionParam(1);
            if (TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance())
            {
                if (Player* player = turnMode->GetPlayer())
                {
                    TokenParam      tokenParam     = GetTokenParam(1);
                    TokenInventory& tokenInventory = player->GetTokenInventory();
                    tokenInventory.AddTokenStackFromID(tokenParam.TokenID, tokenParam.Count);
                }
            }
            ProcessBattle(damage.Param);
        }
    }
} // namespace Monster