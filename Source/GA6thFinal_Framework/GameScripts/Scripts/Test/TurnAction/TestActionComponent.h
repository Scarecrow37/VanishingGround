#pragma once
#include "UmFramework.h"
#include <TurnSystem/TurnAction/TurnAction.h>

class TestAction : public TurnAction
{
public:
    TestAction()           = default;
    ~TestAction() override = default;

    // TurnAction을(를) 통해 상속됨
    void OnCombatStart() override;
    void OnRoundStart() override;
    void OnRoundEnd() override;
    void OnTurnStart(CharacterBase* destination) override;
    void OnTurnEnd(CharacterBase* destination) override;

    // TurnAction을(를) 통해 상속됨
    const std::string& GetActionName() override;
    const std::string& GetActionInfo() override;
    void             ImGuiDrawActionEditor() override;
};



class TestActionComponent : public Component
{
    USING_PROPERTY(TestActionComponent)

public:
    TestActionComponent();
    ~TestActionComponent() override;

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(TestActionComponent)

    void Start() override;

    std::unique_ptr<TestAction> _testAction;
};
