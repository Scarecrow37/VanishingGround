#pragma once
#include "Base/TurnModeStateBase.h"

class GameClearState : public TurnModeStateBase
{
public:
    GameClearState();
    virtual ~GameClearState() override;

    void OnAwake() override;
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate() override;
};