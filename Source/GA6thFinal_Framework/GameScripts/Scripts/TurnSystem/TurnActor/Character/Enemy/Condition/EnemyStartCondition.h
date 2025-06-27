#pragma once
#include "Base/EnemyConditionBase.h"

/*
* 자신의 턴 시작을 체크하는 조건입니다.
*/
class EnemyStartCondition : public EnemyConditionBase
{
public:
// EnemyConditionBase을(를) 통해 상속됨
    void OnAwake() override;
    void OnStart() override;
    bool Evaluate() override;


};