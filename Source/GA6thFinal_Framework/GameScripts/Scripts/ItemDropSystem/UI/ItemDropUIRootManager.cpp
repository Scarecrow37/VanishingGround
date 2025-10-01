#include "pchScripts.h"
#include "ItemDropUIRootManager.h"
#include "ItemDropSystem/UI/ArtifactUIManager.h"
#include "ItemDropSystem/ItemDropSystem.h"
#include "RevelationSystem/RevelationSystem.h"
#include "ExcelDataSystem/ExcelDataSystem.h"

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
    ImGuiDrawArtifactUIAssetSetting();    
}

void ItemDropUIRootManager::ImGuiDrawArtifactUIAssetSetting() 
{
    if (ImGui::TreeNode("Artifact UI Setting"))
    {
        static std::string artifactsUIFrameAssetGuidBuff;
        artifactsUIFrameAssetGuidBuff = ArtifactsUIFrameAsset;
        artifactsUIFrameAssetGuidBuff = std::filesystem::path(artifactsUIFrameAssetGuidBuff).filename().string();
        ImGui::InputText("Artifacts UI Frame Asset", &artifactsUIFrameAssetGuidBuff, ImGuiInputTextFlags_ReadOnly);
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                DragDropAsset::Data* data      = static_cast<DragDropAsset::Data*>(payLoad->Data);
                File::Path           path      = data->GetPath();
                const auto           extension = path.extension();
                if (extension == L".png" || extension == L".dds")
                {
                    ReflectFields->ArtifactsUIFrameAssetGuid = data->GetGuid().string();
                }
            }
            ImGui::EndDragDropTarget();
        }
        ImGuiHelper::HoveredToolTip(u8"유물 드랍 프레임 UI 에셋 경로입니다.");

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
    }
}

