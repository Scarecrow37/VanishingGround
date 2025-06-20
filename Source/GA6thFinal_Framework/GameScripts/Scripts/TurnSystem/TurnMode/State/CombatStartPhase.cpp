#include "CombatStartPhase.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"

REGISTER_CLASS(FSMStateFactory, CombatStartPhase)

CombatStartPhase::CombatStartPhase()
    : 
    _phaseEnd(false)
{

}

CombatStartPhase::~CombatStartPhase() {}

void CombatStartPhase::ResetCharacterStats() 
{
    for (auto& weak : GameObject::FindGameObjectsWithTag(CharacterBase::TAG))
    {
        if (false == weak.expired())
        {
            auto           object    = weak.lock();
            CharacterBase* character = nullptr;
            for (int i = 0; i < object->GetComponentCount(); ++i)
            {
                character = object->GetComponentAtIndex<CharacterBase>(i);
                if (nullptr != character)
                {
                    break;
                }
            }
            if (nullptr != character)
            {
                character->SetHP(character->MaxHP);
                character->SetMP(character->MaxMP);
                character->SetState(TurnActor::STATE::Wait);
            }
        }
    }
}

void CombatStartPhase::OnAwake() {}

void CombatStartPhase::OnStart() {}

void CombatStartPhase::OnEnter() 
{
    ResetCharacterStats();
    UmLogger.Message(LogLevel::LEVEL_TRACE, (const char*)u8"배틀 시작...3");
    UmTime.Invoke(&GetFSM(), 1.f, [this]() { UmLogger.Message(LogLevel::LEVEL_TRACE, (const char*)u8"배틀 시작...2"); });
    UmTime.Invoke(&GetFSM(), 2.f, [this]() { UmLogger.Message(LogLevel::LEVEL_TRACE, (const char*)u8"배틀 시작...1"); });
    UmTime.Invoke(&GetFSM(), 3.f, [this]() { this->_phaseEnd = true; });
}

void CombatStartPhase::OnExit() {}

void CombatStartPhase::OnUpdate() {}
