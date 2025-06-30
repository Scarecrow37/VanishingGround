#pragma once
#include "Base/EnemyConditionBase.h"

/*
* 사망 상태를 체크하는 조건입니다.
*/
class EnemyDeadCondition : public EnemyConditionBase
{
public:



// EnemyConditionBase을(를) 통해 상속됨
    void OnAwake() override;
    void OnStart() override;
    bool Evaluate() override;
};