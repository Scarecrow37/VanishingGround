#pragma once
#include <GameCore/FSM/FSMCondition.h>

class Player;
class PlayerConditionBase : public FSMCondition
{
public:
    PlayerConditionBase();
    virtual ~PlayerConditionBase() override;

protected:
    Player& GetPlayer();

private:
    Player* _player;
};