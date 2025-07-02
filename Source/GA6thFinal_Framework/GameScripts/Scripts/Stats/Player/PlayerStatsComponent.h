#pragma once
#include "Stats/CharacterStatsComponent.h"

struct PlayerStats;
class PlayerStatsComponent : public CharacterStatsComponent
{
    USING_PROPERTY(PlayerStatsComponent)
public:
    REFLECT_PROPERTY()

    PlayerStats* GetStats() { return _stats; }    
public:
    PlayerStatsComponent();
    virtual ~PlayerStatsComponent();

protected:
    REFLECT_FIELDS_BEGIN(CharacterStatsComponent)
    REFLECT_FIELDS_END(PlayerStatsComponent)

    // 이 함수를 override 해서 CharacterStats를 동적할당해 반환해야 합니다.
    CharacterStats* NewCharacterStats() override;
private:
    PlayerStats* _stats = nullptr;

};
