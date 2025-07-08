#pragma once
#include "../EnemyAI.h"

class EnemyAI23000 : public EnemyAI
{
public:
    EnemyAI23000();
    virtual ~EnemyAI23000();

private:
    ActionNode* _action21101    = nullptr; // AI 행동 1
    ActionNode* _action21102    = nullptr; // AI 행동 2
    ActionNode* _action21103    = nullptr; // AI 행동 3
    ConditionNode* _isBleeding  = nullptr; // AI 조건 검사
};
