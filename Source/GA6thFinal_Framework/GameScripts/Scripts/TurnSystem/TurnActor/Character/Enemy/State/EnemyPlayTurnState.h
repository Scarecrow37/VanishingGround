#pragma once
#include "Base/EnemyStateBase.h"
#include "../Enum/EnemyEnum.h"

class EnemyAI;

/*
자신의 턴이 온 상태입니다.
*/
class EnemyPlayTurnState : public EnemyStateBase
{
private:
    // EnemyStateBase을(를) 통해 상속됨
    void OnAwake() override;
    void OnStart() override;
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate() override;

private:
    void SetAIModel(EnemyType type);
    bool ProcessAI(int aiModelID);

    ///////////////////////////////////
    // MonsterA Action
    ///////////////////////////////////
    bool Action22000();
    bool Action22001();
    bool Action22002();
    bool Action22003();
    bool Action22004();

    ///////////////////////////////////
    // MonsterB Action
    ///////////////////////////////////

private:
    EnemyAI* _aiModel;
};