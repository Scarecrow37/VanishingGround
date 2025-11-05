#pragma once
#include "Base/TurnModeStateBase.h"

class GameOverState : public TurnModeStateBase
{
public:
    GameOverState();
    virtual ~GameOverState() override;

protected:
    void OnAwake() override;
    void OnStart() override;
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate() override;

private:
    bool _waitPlayerDeactive = true;
};