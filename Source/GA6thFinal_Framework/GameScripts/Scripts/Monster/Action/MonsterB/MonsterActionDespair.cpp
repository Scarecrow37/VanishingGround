#include "pchScripts.h"
#include "MonsterActionDespair.h"

REGISTER_MONSTER_ACTION(Monster::Action::Despair)
namespace Monster
{
    namespace Action
    {
        Despair::Despair() : Base("Attack1") {}
        Despair::~Despair() = default;
        void Despair::OnActionEnter() 
        {
            _attackCount = 0;
        }
        void Despair::OnActionUpdate() {}
        void Despair::OnActionExit() {}
        void Despair::OnActionReset() {}
        void Despair::OnNotifiedAnimationEvent(const Timeline::EventContext* context)
        {
            const std::string& label = context->GetLabel();
            if ("Attack" == label)
            {
                if (_attackCount < GetActionContext().AttackCount)
                {
                    Attack();
                    ++_attackCount;
                }
            }
        }

        /*
        플레이어게 S_Param(1)의 데미지로 피해를 준다.
        */
        void Despair::Attack() 
        {
            ActionParam damage = GetActionParam(1);
            ProcessBattle(damage.Param);
        }
    }
} // namespace Monster
