#pragma once
#include "GameCore/FSM/FiniteStateMachine.h"

class CharacterBase;
/// <summary>
/// TurnMode를 Start에서 가져와주는 베이스 클래스입니다.
/// </summary>
class TurnModeStateBase : public FSMState
{
public:
    TurnModeStateBase();
    virtual ~TurnModeStateBase() override;

protected:
    void OnStart() override;

    void UpdateCharacterDead(const std::function<void(CharacterBase&)>& deadCallback = std::function<void(CharacterBase&)>());

protected:
    class TurnMode* _turnMode = nullptr;
    class WeaponSystem* _weaponSystem = nullptr;
    class RevelationSystem* _revelationSystem = nullptr;
    class AccessorySystem*  _accessorySystem  = nullptr;
};