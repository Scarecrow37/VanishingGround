#include "pchScripts.h"
#include "ItemDropSystem.h"
#include "WeaponSystem/WeaponSystem.h"
#include "WeaponSystem/WeaponTable/WeaponTableComponent.h"
#include "RevelationSystem/RevelationSystem.h"
#include "ItemDropSystem/UI/ArtifactUIManager.h"

ItemDropSystem::ItemDropSystem() = default;
ItemDropSystem::~ItemDropSystem() = default;

std::vector<DropItemInfo> ItemDropSystem::RollArtifacts()
{  
    std::vector<DropItemInfo> artifacts;

    //일단 임시로 그냥 단순 6개 랜덤
    RevelationSystem* revelationSystem = RevelationSystem::GetInstance();
    if (revelationSystem)
    {
        const auto& table = revelationSystem->GetRevelationTableElements();
        for (auto& revelation : table)
        {
            artifacts.push_back(revelation->GetItemInfo());
        }      
    }
    WeaponTableComponent* weaponTableComponent = WeaponTableComponent::GetInstance();
    if (weaponTableComponent)
    {
        const auto& table = weaponTableComponent->GetWeaponTableElements();
        for (auto& weapon : table)
        {
            artifacts.push_back(weapon->GetItemInfo());
        }
    }

    if (false == artifacts.empty())
    {
        std::shuffle(artifacts.begin(), artifacts.end(), Random::GetEngine());
        artifacts.resize(6);
    }
    return artifacts;
}

void ItemDropSystem::SetDropItem(const std::vector<DropItemInfo>& itemInfos) 
{
    _dropItemsModel = itemInfos;
}

void ItemDropSystem::ImGuiDrawPropertysEvent() 
{
    if (ImGui::TreeNode("Artifacts"))
    {
        if (ImGui::Button("Roll Artifacts"))
        {
            std::vector<DropItemInfo> artifacts = RollArtifacts();
            SetDropItem(artifacts);
        }
        const auto& model = _dropItemsModel;
        for (auto& item : model)
        {
            const DropItemInfo& info = item;
            ImGui::PushID(&info);
            ImGui::Selectable(info.Name.data());
            ImGui::PopID();
        }
        ImGui::TreePop();
    }
}

void ItemDropSystem::Awake() 
{
    static_instance = this;
}

 