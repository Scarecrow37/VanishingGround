#include "pchScripts.h"
#include "TokenTypeCountSpeedPlus.h"

#include "Stats/Weapon/WeaponStats.h"
#include "Token/Object/Armor/ArmorToken.h"
#include "Token/TokenSystem.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include "TurnSystem/TurnSystemHelper.h"

REGISTER_TURN_ACTION(TokenTypeCountSpeedPlus)

REFLECT_FUNCTION(TokenTypeCountSpeedPlus)

TokenTypeCountSpeedPlus::TokenTypeCountSpeedPlus()
{
    UpdateActionInfo();
}

TokenTypeCountSpeedPlus::~TokenTypeCountSpeedPlus() = default;

const std::string& TokenTypeCountSpeedPlus::GetActionName()
{
    const static std::string name = (const char*)u8"특정 토큰 상태인 캐릭터 하나당 추가 스피드 증가";
    return name;
}

const std::string& TokenTypeCountSpeedPlus::GetActionInfo()
{
    TryTokenSystemInfoUpdate();
    return _actionInfo;
}

void TokenTypeCountSpeedPlus::ImGuiDrawActionEditor()
{
    ImGuiDrawPropertys();
    ImGui::Text("Action");
    if (TurnSystemHelper::DrawTargetComboboxWithToolTip(ReflectFields->TokenTarget, true))
    {
        UpdateActionInfo();
    }
    ReflectHelper::ImGuiDraw::Private::InputAuto(TokenTag, UmCore->ImGuiDrawPropertysSetting);
    ReflectHelper::ImGuiDraw::Private::InputAuto(PlusSpeed, UmCore->ImGuiDrawPropertysSetting);
    ImGui::Separator();
    ImGui::Text("Conditions");
    ImguiDrawConditionEditor();
}

void TokenTypeCountSpeedPlus::OnWeaponRoundSpeedApply(WeaponElement& weapon, int& plusSpeed) 
{
    if (EvaluateConditions())
    {
        int tokenCount = 0;
        std::vector<CharacterBase*> targets = TurnSystemHelper::GetTargetCharacters(ReflectFields->TokenTarget);
        if (false == targets.empty())
        {
            for (auto& target : targets)
            {
                if (target)
                {
                    TokenInventory& inventory = target->GetTokenInventory();
                    if (inventory.HasTokenFromTag(ReflectFields->TokenTag))
                        ++tokenCount;
                }
            }
        }

        if (0 < tokenCount)
        {
            plusSpeed += tokenCount * PlusSpeed;
        }
    }
}

void TokenTypeCountSpeedPlus::TryTokenSystemInfoUpdate()
{
    if (false == _validTokenSystem)
    {
        if (TokenSystem* system = SingletonComponent<TokenSystem>::GetInstance())
        {
            _validTokenSystem = true;
            UpdateActionInfo();
        }
    }
}

void TokenTypeCountSpeedPlus::UpdateActionInfo()
{
    using namespace u8_literals;
    _actionInfo.clear();
    _actionInfo = (const char*)TurnSystemHelper::GetTurnTargetToolTip(ReflectFields->TokenTarget).data();
    _actionInfo += u8"에게 존재하는 "_c_str;
    _actionInfo += TokenTag;
    _actionInfo += u8" 토큰 종류 x "_c_str;
    _actionInfo += std::to_string(PlusSpeed);
    _actionInfo += u8" 만큼 속도 증가"_c_str;
}