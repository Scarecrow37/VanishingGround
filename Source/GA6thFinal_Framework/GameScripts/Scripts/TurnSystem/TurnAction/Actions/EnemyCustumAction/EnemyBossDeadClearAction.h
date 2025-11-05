#pragma once
#include <TurnSystem/TurnAction/TurnActionFactory.h>

class EnemyBossDeadClearAction : public TurnAction
{
    USING_PROPERTY(EnemyBossDeadClearAction)
public:
    EnemyBossDeadClearAction()           = default;
    ~EnemyBossDeadClearAction() override = default;

    REFLECT_PROPERTY()

private:
    void OnEnemyDead(Enemy& enemy) override;

protected:
    REFLECT_FIELDS_BEGIN(TurnAction)
    REFLECT_FIELDS_END(EnemyBossDeadClearAction)

private:
    const std::string& GetActionName() override;
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;
};
