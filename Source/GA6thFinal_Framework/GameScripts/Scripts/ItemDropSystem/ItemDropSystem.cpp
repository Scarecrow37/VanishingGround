#include "pchScripts.h"
#include "ItemDropSystem.h"
#include "WeaponSystem/WeaponSystem.h"
#include "WeaponSystem/WeaponTable/WeaponTableComponent.h"
#include "RevelationSystem/RevelationSystem.h"
#include "ItemDropSystem/UI/ArtifactUIManager.h"

ItemDropSystem::ItemDropSystem() = default;
ItemDropSystem::~ItemDropSystem() = default;

void ItemDropSystem::RollArtifacts() 
{  
    //일단 임시로 그냥 단순 6개 랜덤
    RevelationSystem* revelationSystem = RevelationSystem::GetInstance();
    if (revelationSystem)
    {
        const auto& table = revelationSystem->GetRevelationTableElements();
        for (auto& revelation : table)
        {
            _dropItems.push_back(revelation);
        }      
    }
    WeaponTableComponent* weaponTableComponent = WeaponTableComponent::GetInstance();
    if (weaponTableComponent)
    {
        const auto& table = weaponTableComponent->GetWeaponTableElements();
        for (auto& weapon : table)
        {
            _dropItems.push_back(weapon);
        }
    }

    std::shuffle(_dropItems.begin(), _dropItems.end(), Random::GetEngine());
    _dropItems.resize(6);
    _dropItemsModel = _dropItems; //UI 갱신
    _dropItems.clear(); //정리
}

void ItemDropSystem::ImGuiDrawPropertysEvent() 
{
    if (ImGui::TreeNode("Artifacts"))
    {
        if (ImGui::Button("Roll Artifacts"))
        {
            RollArtifacts();
        }
        const auto& model = _dropItemsModel;
        for (auto& item : model)
        {
            DropItemInfo info = item->GetItemInfo();
            ImGui::PushID(item);
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

 