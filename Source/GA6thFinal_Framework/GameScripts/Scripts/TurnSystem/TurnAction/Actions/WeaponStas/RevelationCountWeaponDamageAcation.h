#pragma once
#include "TurnSystem/TurnAction/TurnAction.h"

// 보유 계시만큼 일격 및 치명타 데미지 증가
class RevelationCountWeaponDamageAcation : public TurnAction
{
public:
    inline static const std::string NAME = (const char*)u8"항상 보유 계시 만큼 일격 및 치명타 데미지 증가";

    RevelationCountWeaponDamageAcation() = default;
    ~RevelationCountWeaponDamageAcation() override = default;

protected:
    /// <summary>
    /// 턴 모드에 이 액션이 추가될때 호출됩니다.
    /// </summary>
    void OnAddedAction() override;

    /// <summary>
    /// SetDestory가 호출될때 호출됩니다.
    /// </summary>
    void OnDestroy() override;

private:
    int _damage = 0;


// TurnAction을(를) 통해 상속됨
    const std::string& GetActionName() override;
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;
};