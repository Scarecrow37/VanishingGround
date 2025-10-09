#include "pchScripts.h"
#include "ItemDropUIRootManager.h"
#include "ItemDropSystem/UI/ArtifactUIManager.h"
#include "ItemDropSystem/ItemDropSystem.h"
#include "RevelationSystem/RevelationSystem.h"
#include "ExcelDataSystem/ExcelDataSystem.h"
#include "ItemDropSystem/UI/ItemInfoUIManager.h"

UMREAL_COMPONENT(ItemDropUIRootManager)

ItemDropUIRootManager::ItemDropUIRootManager()
{
    
}

ItemDropUIRootManager::~ItemDropUIRootManager()
{
  
}

void ItemDropUIRootManager::DeserializedReflectEvent()
{
    
}

void ItemDropUIRootManager::ImGuiDrawPropertysEvent()
{
    auto CheckWeakPtrText = [](const auto& weakPtr) 
    {
        if (auto ptr = weakPtr.lock())
        {
            ImGui::Text("ArtifactUIManager valid");
        }
        else
        {
            ImGui::Text("ArtifactUIManager nullptr");
        }
    };

    if (ImGui::TreeNode("Debug"))
    {
        CheckWeakPtrText(_artifactUIManager); 
        CheckWeakPtrText(_itemInfoUIManager);
        ImGui::TreePop();
    }
}

void ItemDropUIRootManager::Reset() 
{
    _singletonComponent.SetSingleTon();
    if (true == UmCore->IsPlay())
    {
        gameObject->ActiveSelf = true;
    }
}

void ItemDropUIRootManager::Awake()
{
    if (_singletonComponent.TrySingleTon())
    {
        gameObject->AddTag(ItemDropUIRootManager::TAG);
        Base::Awake();
    }
}

void ItemDropUIRootManager::Start() 
{
    if (_singletonComponent.IsSingleTon())
    {
        gameObject->ActiveSelf = false;
        if (auto artifactUI = GameObject::FindWithTag(ArtifactUIManager::TAG).lock())
        {
            if (ArtifactUIManager* component = artifactUI->GetComponent<ArtifactUIManager>())
            {
                auto weakComponent = component->GetWeakPtr();
                _artifactUIManager = std::static_pointer_cast<ArtifactUIManager>(weakComponent.lock());
            }
        }
        if (auto itemInfoUI = GameObject::FindWithTag(ItemInfoUIManager::TAG).lock())
        {
            if (ItemInfoUIManager* component = itemInfoUI->GetComponent<ItemInfoUIManager>())
            {
                auto weakComponent = component->GetWeakPtr();
                _itemInfoUIManager = std::static_pointer_cast<ItemInfoUIManager>(weakComponent.lock());
            }
        }
    }
}

