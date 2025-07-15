#include "pchScripts.h"
#include "TestActionComponent.h"
#include <TurnSystem/TurnMode/TurnMode.h>

TestActionComponent::TestActionComponent() = default;
TestActionComponent::~TestActionComponent() = default;

void TestActionComponent::Start() 
{
    TurnMode* turnMode = TurnMode::GetInstance();
    if (turnMode)
    {
        _testAction.reset(new TestAction);
        turnMode->AddTurnAction(_testAction.get());
    }
}

void TestAction::OnCombatStart()
{
    __debugbreak();
}

void TestAction::OnRoundStart()
{
    __debugbreak();
}

void TestAction::OnRoundEnd()
{
    __debugbreak();
}

void TestAction::OnEachTurnStart(CharacterBase* destination)
{
    __debugbreak();
}

void TestAction::OnTurnStart()
{
    __debugbreak();
}

void TestAction::OnTurnEnd()
{
    __debugbreak();
}
