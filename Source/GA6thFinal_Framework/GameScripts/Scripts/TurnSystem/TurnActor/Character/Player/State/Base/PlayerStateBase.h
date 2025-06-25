#pragma once
#include <GameCore/FSM/FSMState.h>

class Player;
class PlayerStateBase : public FSMState
{
public:
    PlayerStateBase();
    virtual ~PlayerStateBase() override;

protected:
    Player& GetPlayer(); 

private:
    Player* _player;
};