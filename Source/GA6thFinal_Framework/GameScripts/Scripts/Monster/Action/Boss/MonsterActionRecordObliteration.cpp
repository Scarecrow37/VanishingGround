#include "pchScripts.h"
#include "MonsterActionRecordObliteration.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"

REGISTER_MONSTER_ACTION(Monster::Action::RecordObliteration)
namespace Monster
{
    namespace Action
    {
        RecordObliteration::RecordObliteration() : Base("Attack2") {}
        RecordObliteration::~RecordObliteration() = default;
        void RecordObliteration::OnActionEnter() {}
        void RecordObliteration::OnActionUpdate() {}
        void RecordObliteration::OnActionExit() {}
        void RecordObliteration::OnActionReset() {}
        void RecordObliteration::OnNotifiedAnimationEvent(const Timeline::EventContext* context) 
        {
            const std::string& label = context->GetLabel();
            if ("Attack" == label)
            {
                Attack();
            }
        }
        /*
        플레이어에게 S_Param(1)의 데미지로 피해를 준다.
        자신의 연격 1당 S_Param(2)의 추가 피해가 더해진다.
        */
        void RecordObliteration::Attack() 
        {
            int resultDamage = 0;

            ActionParam defaultDamage = GetActionParam(1);
            resultDamage = defaultDamage.Param;

            if (auto* owner = GetOwnerEnemy())
            {
                int chainCount = owner->ChainCount;
                ActionParam extraDamage = GetActionParam(2);
                resultDamage += extraDamage.Param * chainCount;
            }

            ProcessBattle(resultDamage);
        }
    } // namespace Action
} // namespace Monster