#include "pchScripts.h"
#include "ExtinctionRevelationApply.h"
#include "TurnSystem/TurnAction/TurnActionFactory.h"

REGISTER_TURN_ACTION(ExtinctionRevelationApply)

REFLECT_FUNCTION(ExtinctionRevelationApply)

ExtinctionRevelationApply::ExtinctionRevelationApply()
{
    UpdateActionInfo();
}
ExtinctionRevelationApply::~ExtinctionRevelationApply() = default;

const std::string& ExtinctionRevelationApply::GetActionName()
{
    static const std::string name = (const char*)u8"반복 전투시 추가되는 소멸 계시 갯수를 조정합니다";
    return name;
}

const std::string& ExtinctionRevelationApply::GetActionInfo()
{
    return _actionInfo;
}

void ExtinctionRevelationApply::ImGuiDrawActionEditor() 
{
    ImGuiDrawPropertys();
    int plus = ReflectFields->PlusCount;
    if (ImGui::InputInt((const char*)u8"추가 소멸 갯수", &plus))
    {
        ReflectFields->PlusCount = plus;
        UpdateActionInfo();
    }
    ImguiDrawConditionEditor();
}

void ExtinctionRevelationApply::DeserializedReflectEvent() 
{
    UpdateActionInfo();
}

void ExtinctionRevelationApply::OnRandomExtinctionPushPlayer(size_t& count) 
{
    if (EvaluateConditions())
    {
        count += ReflectFields->PlusCount;
    }
}

void ExtinctionRevelationApply::UpdateActionInfo()
{
    using namespace u8_literals;
    _actionInfo.clear();
    _actionInfo = u8"플레이어에게 추가되는 소멸 계시의 갯수 "_c_str;
    if (0 < ReflectFields->PlusCount)
    {
        _actionInfo += "+ ";
    }
    _actionInfo += std::to_string(ReflectFields->PlusCount);
}