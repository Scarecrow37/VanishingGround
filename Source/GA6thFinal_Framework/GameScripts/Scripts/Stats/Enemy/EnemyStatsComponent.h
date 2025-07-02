#pragma once
#include "Stats/CharacterStatsComponent.h"

struct EnemyStats;
class EnemyStatsComponent : public CharacterStatsComponent
{
    USING_PROPERTY(EnemyStatsComponent)
public:
    REFLECT_PROPERTY()

public:
    EnemyStatsComponent();
    virtual ~EnemyStatsComponent();

    EnemyStats* GetStats() { return _stats; }

protected:
    REFLECT_FIELDS_BEGIN(CharacterStatsComponent)
    REFLECT_FIELDS_END(EnemyStatsComponent)

    // CharacterStatsComponent을(를) 통해 상속됨
    CharacterStats* NewCharacterStats() override;

private:
    EnemyStats* _stats = nullptr;
};
