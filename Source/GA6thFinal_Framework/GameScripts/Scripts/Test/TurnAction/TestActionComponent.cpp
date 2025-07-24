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
    UmLogger.Log(LogLevel::LEVEL_TRACE, u8"전투 시작");
}

void TestAction::OnRoundStart()
{
    UmLogger.Log(LogLevel::LEVEL_TRACE, u8"라운드 시작");
}

void TestAction::OnRoundEnd()
{
    UmLogger.Log(LogLevel::LEVEL_TRACE, u8"라운드 종료");
}

void TestAction::OnTurnStart(CharacterBase& destination)
{
    UmLogger.Log(LogLevel::LEVEL_TRACE, u8"턴 시작");
}

void TestAction::OnTurnEnd(CharacterBase& destination)
{
    UmLogger.Log(LogLevel::LEVEL_TRACE, u8"턴 종료");
}

const std::string& TestAction::GetActionName()
{
    static const std::string name = (const char*)u8"테스트 액션";
    return name;
}

const std::string& TestAction::GetActionInfo()
{
    static const std::string info = (const char*)u8"테스트 액션 정보";
    return info;
}

void TestAction::ImGuiDrawActionEditor() 
{
    ImGui::Text((const char*)u8"테스트 액션");
}
