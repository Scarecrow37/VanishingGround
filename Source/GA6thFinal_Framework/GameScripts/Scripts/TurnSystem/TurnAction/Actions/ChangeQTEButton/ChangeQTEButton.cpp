#include "pchScripts.h"
#include "ChangeQTEButton.h"
#include "TurnSystem/TurnAction/TurnActionFactory.h"
#include "QTE/System/QTESystem.h"

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
    if (QTESystem* system = SingletonComponent<QTESystem>::GetInstance())
    {
        // 키 바인딩 상태의 반대로 지정
        const QTE::KeyBinder& keyBinder = system->GetKeyBinder();
        QTE::KeyBindState newState;
        newState.ButtonX = keyBinder.GetKeyB();
        newState.ButtonB = keyBinder.GetKeyX();
        system->PushKeyBindState(newState);
    }
}

void ChangeQTEButton::OnDestroy() 
{
    if (QTESystem* system = SingletonComponent<QTESystem>::GetInstance())
    {
        system->PopKeyBindState();
    }
}

void ChangeQTEButton::UpdateActionInfo()
{
     using namespace u8_literals;  
    _actionInfo.clear();
    _actionInfo = u8"QTE시 X와 B키를 변경"_c_str;
}