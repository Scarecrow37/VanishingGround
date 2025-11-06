#include "pchScripts.h"
#include "TokenTypeCountAttackDamagePlus.h"
#include "Token/TokenSystem.h"
#include "Token/Object/Armor/ArmorToken.h"
#include "TurnSystem/TurnSystemHelper.h"
#include "Stats/Weapon/WeaponStats.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"

REGISTER_TURN_ACTION(TokenTypeCountAttackDamagePlus)

REFLECT_FUNCTION(TokenTypeCountAttackDamagePlus)

TokenTypeCountAttackDamagePlus::TokenTypeCountAttackDamagePlus() 
{
    UpdateActionInfo();
}

TokenTypeCountAttackDamagePlus::~TokenTypeCountAttackDamagePlus() = default;

const std::string& TokenTypeCountAttackDamagePlus::GetActionName()
{
    const static std::string name = (const char*)u8"공격시 특정 타입 토큰 갯수만큼 데미지 증가";
    return name;
}

const std::string& TokenTypeCountAttackDamagePlus::GetActionInfo()
{
    TryTokenSystemInfoUpdate();
    return _actionInfo;
}

void TokenTypeCountAttackDamagePlus::ImGuiDrawActionEditor() 
{
    ImGuiDrawPropertys();
    ImGui::Text("Action");
    if (TurnSystemHelper::DrawTargetComboboxWithToolTip(ReflectFields->TokenTarget, true))
    {
        UpdateActionInfo();
    }
    ReflectHelper::ImGuiDraw::Private::InputAuto(TokenTag, UmCore->ImGuiDrawPropertysSetting);
    ReflectHelper::ImGuiDraw::Private::InputAuto(HitDamage, UmCore->ImGuiDrawPropertysSetting);
    ReflectHelper::ImGuiDraw::Private::InputAuto(CriticalDamage, UmCore->ImGuiDrawPropertysSetting);
    ReflectHelper::ImGuiDraw::Private::InputAuto(IsCountToken, UmCore->ImGuiDrawPropertysSetting);
    ImGui::Separator();
    ImGui::Text("Conditions");
    ImguiDrawConditionEditor();
}

void TokenTypeCountAttackDamagePlus::OnPlayerBattleCalculateDamageModifier(Player& attacker, PlayerStats& attackerStats,
                                                                           WeaponStats& weaponStats, Enemy& target,
                                                                           EnemyStats& targetStats)
{
    if (EvaluateConditions())
    {
        int tokenCount = 0;
        std::vector<CharacterBase*> targets    = TurnSystemHelper::GetTargetCharacters(ReflectFields->TokenTarget);
        if (false == targets.empty())
        {
            for (auto& target : targets)
            {
                if (target)
                {
                    if (target->IsDead())
                    {
                        continue;
                    }

                    TokenInventory& inventory = target->GetTokenInventory();

                    if (IsCountToken)
                    {
                        tokenCount += inventory.GetTokenStackFromTag(ReflectFields->TokenTag);
                    }
                    else
                    {
                        tokenCount += inventory.HasTokenFromTag(ReflectFields->TokenTag);
                    }
                }
            }
        }

        if (0 < tokenCount)
        {
            weaponStats.HitDamage += tokenCount * ReflectFields->HitDamage;
            weaponStats.CriticalDamage += tokenCount * ReflectFields->CriticalDamage;
        }   
    }
}


void TokenTypeCountAttackDamagePlus::TryTokenSystemInfoUpdate()
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

void TokenTypeCountAttackDamagePlus::UpdateActionInfo()
{
    using namespace u8_literals;
    _actionInfo.clear();
    _actionInfo = (const char*)TurnSystemHelper::GetTurnTargetToolTip(ReflectFields->TokenTarget).data();
    _actionInfo += u8"에게 존재하는 "_c_str;
    _actionInfo += TokenTag;

    if (IsCountToken)
        _actionInfo += u8" 토큰 갯수 만큼"_c_str;
    else
        _actionInfo += u8" 캐릭터 한명당"_c_str;

    _actionInfo += u8" 일격 + 갯수 x "_c_str;
    _actionInfo += std::to_string(ReflectFields->HitDamage);
    _actionInfo += u8" 치명타 + 갯수 x "_c_str;
    _actionInfo += std::to_string(ReflectFields->CriticalDamage);
}