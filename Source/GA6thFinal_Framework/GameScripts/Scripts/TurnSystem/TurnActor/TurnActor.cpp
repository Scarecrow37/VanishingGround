#include "pchScripts.h"
#include "TurnActor.h"

REFLECT_FUNCTION(TurnActor)

TurnActor::TurnActor() 
    : 
    _currState(STATE::Dead)
{

}
TurnActor::~TurnActor() = default;

void TurnActor::ClearState() 
{
}

void TurnActor::UpdatePostTurnState()
{
    if (_currState != STATE::Dead)
    {
        if (HasTurnActorFlags(TurnActorFlags::FLAGS_TURN_SKIP))
        {
            RemoveTurnActorFlags(TurnActorFlags::FLAGS_TURN_SKIP); // 턴 스킵 플래그 제거

            std::string_view name    = gameObject->ToString();
            std::string      message = std::format("{}{}{}", name.data(), (const char*)u8"가 기절로 인하여",
                                                   (const char*)u8" 턴을 스킵합니다.");
            UmLogger.Message(LogLevel::LEVEL_DEBUG, message);

            _currState = STATE::Wait; // 턴 스킵 플래그가 있으면 턴을 스킵합니다.
        }
    }
}

void TurnActor::PlayTurn()
{
    // Wait 상태일 때만
    if (_currState == STATE::Wait)
    {
        _currState = STATE::Play;
    }
}

void TurnActor::Revive() 
{
    if (_currState == STATE::Dead)
    {
        _currState = STATE::Wait;
    }
}

void TurnActor::Dead() 
{
    if (_currState != STATE::Dead)
    {
        _currState = STATE::Dead;
    }
}

void TurnActor::EndTurn() 
{
    if (_currState == STATE::Play)
    {
        _currState = STATE::Wait;
    }
}

void TurnActor::Awake() 
{
    gameObject->AddTag(TAG);
}

void TurnActor::OnCombatStart() {}

void TurnActor::OnRoundStart() {}

void TurnActor::OnRoundEnd() {}

void TurnActor::OnEachTurnStart(CharacterBase* destination) {}

void TurnActor::OnTurnStart() {}

void TurnActor::OnTurnEnd() {}

void TurnActor::OnHit() {}

void TurnActor::OnKill(CharacterBase* destination) {}

void TurnActor::OnTokenAdded(int tokenID) {}

void TurnActor::OnTokenRemoved(int tokenID) {}

void TurnActor::OnQTEStart() {}

void TurnActor::OnQTEEnd() {}
