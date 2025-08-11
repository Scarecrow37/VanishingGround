#include "pchScripts.h"
#include "TurnSystemHelper.h"
#include <UmFramework.h>
#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <BattleSystem/Battle.h>

using namespace u8_literals;

std::vector<CharacterBase*> TurnSystemHelper::GetTargetCharacters(TurnTarget target)
{
    std::vector<CharacterBase*> targetCharacters;
    if (TurnMode* turnMode = TurnMode::GetInstance())
    {
        auto lastAttacker    = Battle::GetLastAttacker().lock();
        auto lastTarget      = Battle::GetLastTarget().lock();
        auto lastTargetEnemy = Battle::GetLastTargetEnemy().lock();
        switch (target)
        {
        default:
            return targetCharacters;
        case TurnTarget::SELF: {
            const auto& self = lastAttacker;
            if (self)
            {
                targetCharacters.push_back(self.get());
            }
            break;
        }
        case TurnTarget::PLAYER: {
            Player* player = turnMode->GetPlayer();
            if (player)
            {
                targetCharacters.push_back(player);
            }
            break;
        }
        case TurnTarget::ENEMY: {
            if (lastTarget.get() == lastTargetEnemy.get())
            {
                if (nullptr != lastTargetEnemy)
                {
                    targetCharacters.push_back(lastTargetEnemy.get());
                }
            }
            break;
        }
        case TurnTarget::ALL_ENEMIES: {
            auto& enemys = turnMode->GetEnemies();
            for (auto& enemy : enemys)
            {
                targetCharacters.push_back(enemy);
            }
            break;
        }
        case TurnTarget::ALL: {
            auto& characters = turnMode->GetCharacters();
            for (auto& character : characters)
            {
                targetCharacters.push_back(character);
            }
            break;
        }
        }      
    }
    return targetCharacters;
}

bool TurnSystemHelper::DrawTargetComboboxWithToolTip(TurnTarget& target, bool showComboTitle)
{
    bool isEdit = false;
    const std::string prevValue = rfl::enum_to_string(target);

    bool beginCombo = showComboTitle ? ImGui::BeginCombo(u8"타겟 설정"_c_str, prevValue.c_str())
                                     : ImGui::BeginCombo(u8"##타겟 설정"_c_str, prevValue.c_str());
    if (beginCombo)
    {
        constexpr auto enumerator = rfl::get_enumerator_array<TurnTarget>();
        for (auto& [name, value] : enumerator)
        {
            if (ImGui::Selectable(name.data(), prevValue == name))
            {
                target = value;
                isEdit = true;
            }
            ImGuiHelper::HoveredToolTip(GetTurnTargetToolTip(value));
        }
        ImGui::EndCombo();
    }    
    return isEdit;
}
