#include "pchScripts.h"
#include "RoundStartPhase.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include <WeaponSystem/WeaponSystem.h>
#include <TurnSystem/TurnMode/State/CombatStartPhase.h>
#include <TurnSystem/TurnActor/Character/CharacterBase.h>
#include <RevelationSystem/RevelationSystem.h>
#include "TutorialSystem/TutorialSystem.h"
#include "RoundInfoUI/RoundInfoUIManager.h"
#include "Token/TokenSystem.h"

REGISTER_CLASS(FSMStateFactory, RoundStartPhase)

RoundStartPhase::RoundStartPhase() 
    : 
    _isPhaseEnd(false) 
{

}

void RoundStartPhase::OnStart() 
{
    TurnModeStateBase::OnStart();
    _roundInfoUIManager = GameObject::FindComponentWithTag<RoundInfoUIManager>("Round Info Panel");
}

void RoundStartPhase::OnEnter() 
{
    /// 사운드
    UmAudio.Play("-20100");

    _isPhaseEnd = false;

    if (_weaponSystem)
    {
        _weaponSystem->RollRandomSpeed();
    }
    int currRound = _turnMode->AddRoundCount();

    std::string message = std::format("{}{}", currRound, (const char*)u8"라운드 시작!!!!===========================================================");
    UmLogger.Message(LogLevel::LEVEL_DEBUG, message);

    _turnMode->MakeTurnList();
    _turnMode->SortTurnList();

    if (_revelationSystem)
    {
        _revelationSystem->RollRoundElement();
    }

    NotifyRoundStart();

    //캐릭터 사망 확인
    UpdateCharacterDead();   

    if (TutorialSystem* system = SingletonComponent<TutorialSystem>::GetInstance())
    {
        system->Show(805900);
    }  

    if (auto roundInfoUIManager = _roundInfoUIManager.lock())
    {
        if (_turnMode)
        {
            using namespace u8_literals;
            std::string msg = u8"라운드  "_c_str;
            msg += std::to_string(currRound);
            roundInfoUIManager->FadeInfoUI(msg);

            float delayTime = roundInfoUIManager->UIAnimationTime;
            if (TokenSystem* system = SingletonComponent<TokenSystem>::GetInstance())
            {
                float tokenDelayTime = system->TokenDamageDelayTime * 2; // 두개의 토큰 대미지를 기다려야함 (출혈, 중독)     
                if (delayTime < tokenDelayTime)
                {
                    delayTime = tokenDelayTime;
                }     
            }        
            UmTime.Invoke(roundInfoUIManager.get(), delayTime,
            [this, weakFSM = GetFSM().GetWeakPtrAs<FiniteStateMachine>()]() 
            {   
                if (auto fsm = weakFSM.lock())
                {
                    _isPhaseEnd = true;
                }
            });
        }     
    }
    else
    {
        if (TokenSystem* system = SingletonComponent<TokenSystem>::GetInstance())
        {
            float delayTime = system->TokenDamageDelayTime * 2; //두개의 토큰 대미지를 기다려야함 (출혈, 중독)     
            UmTime.Invoke(GetFSM(), delayTime, [this]() { _isPhaseEnd = true; });
        }
        else
        {
            _isPhaseEnd = true;
        }   
    }
}

void RoundStartPhase::OnExit() 
{
    
}

void RoundStartPhase::OnUpdate() 
{

}
void RoundStartPhase::NotifyRoundStart()
{
    if (_turnMode)
    {
        CombatStartPhase* combatStartPhase = _turnMode->States->CombatStartPhase;
        if (combatStartPhase)
        {
            for (auto& character : combatStartPhase->GetCharacters())
            {
                character->OnRoundStart();
            }
        }
        _turnMode->ApplyActions([](TurnAction& action) { action.OnRoundStart(); });
    }
}