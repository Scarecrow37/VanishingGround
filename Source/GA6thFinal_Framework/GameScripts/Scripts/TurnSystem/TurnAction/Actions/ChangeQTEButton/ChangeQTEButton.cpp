#include "pchScripts.h"
#include "ChangeQTEButton.h"
#include "TurnSystem/TurnAction/TurnActionFactory.h"

REGISTER_TURN_ACTION(ChangeQTEButton)

ChangeQTEButton::ChangeQTEButton()
{
    UpdateActionInfo();
}

ChangeQTEButton::~ChangeQTEButton() = default;

const std::string& ChangeQTEButton::GetActionName()
{
   using namespace u8_literals;  
   const static std::string name = u8"QTE시 X와 B키를 변경"_c_str;
   return name;
}

const std::string& ChangeQTEButton::GetActionInfo()
{
    return _actionInfo;
}

void ChangeQTEButton::ImGuiDrawActionEditor() 
{
    //ImguiDrawConditionEditor();
}

void ChangeQTEButton::OnAddedAction() 
{
    //TODO : QTE 키 변경하는 코드 추가
}

void ChangeQTEButton::OnDestroy() 
{
    // TODO : QTE 키 변경 종료 코드 추가
}

void ChangeQTEButton::UpdateActionInfo()
{
     using namespace u8_literals;  
    _actionInfo.clear();
    _actionInfo = u8"QTE시 X와 B키를 변경"_c_str;
}