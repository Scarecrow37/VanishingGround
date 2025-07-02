#pragma once
#include "Base/EnemyConditionBase.h"

/*
* 자신의 턴 종료를 체크하는 조건입니다.
*/

class EnemyEndCondition : public EnemyConditionBase
{
public:
// EnemyConditionBase을(를) 통해 상속됨
    void OnAwake() override;
    void OnStart() override;
    bool Evaluate() override;

};
