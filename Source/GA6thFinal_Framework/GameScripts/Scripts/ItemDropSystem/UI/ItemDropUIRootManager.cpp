#include "pchScripts.h"
#include "ItemDropUIRootManager.h"
#include "ItemDropSystem/UI/ArtifactUIManager.h"
#include "ItemDropSystem/ItemDropSystem.h"
#include "RevelationSystem/RevelationSystem.h"
#include "ExcelDataSystem/ExcelDataSystem.h"
#include "ItemDropSystem/UI/ItemInfoUIManager.h"
#include "ItemDropSystem/UINavi/RestartStageNavi.h"

UMREAL_COMPONENT(ItemDropUIRootManager)

ItemDropUIRootManager::ItemDropUIRootManager()
{
    
}

ItemDropUIRootManager::~ItemDropUIRootManager()
{
  
}

void ItemDropUIRootManager::AutoFocus() const 
{
    if (ArtifactUIManager* artifactUI = ArtifactUI)
    {
        //포커스 가능한 UI로 설정
        for (size_t i = 0; i < ARTIFACT_DROP_COUNT; ++i)
        {
            if (artifactUI->FocusNavi(i))
            {
                return;
            }
        }
    }

    //없으면 RestartButton으로 포커스 설정
    if (auto restartButton = GameObject::FindWithTag(RestartStageNavi::TAG).lock())
    {
        if (auto navi = restartButton->GetComponent<RestartStageNavi>())
        {
            navi->Focus();
        }
    }
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

