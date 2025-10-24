#pragma once
#include <TurnSystem/TurnAction/Actions/TokenApplyAction/Base/TokenApplyAction.h>

//체력 잃을때 토큰 부여
class TakeDamageEndTokenApplyAction : public TokenApplyAction
{
    USING_PROPERTY(TakeDamageEndTokenApplyAction)
public:
    TakeDamageEndTokenApplyAction() = default;
    ~TakeDamageEndTokenApplyAction() override = default;

public:
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;
    const std::string& GetActionName() override;

private:
    void UpdateActionInfo() override;
    std::string _actionInfo;

protected:
    /// <summary>
    /// 플레이어에 TakeDamage가 들어간 후 호출됩니다.
    /// </summary>
    /// <param name="target :">>대상</param>
    /// <param name="damage :">들어간 데미지</param>
    virtual void OnPlayerTakeDamageEnd(Player& target, int damage) override;

    /// <summary>
    /// Enemy에 TakeDamage가 들어가기 직전에 호출됩니다.
    /// </summary>
    /// <param name="target :">대상</param>
    /// <param name="damage :">들어갈 데미지</param>
    virtual void OnEnemyTakeDamageEnd(Enemy& target, int damage) override;

};