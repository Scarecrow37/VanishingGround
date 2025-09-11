#include "pchScripts.h"
#include "ItemDropUIRootManager.h"
#include "ItemDropSystem/UI/ArtifactUIManager.h"
#include "ItemDropSystem/ItemDropSystem.h"

ItemDropUIRootManager::ItemDropUIRootManager()
{
    static_instance = this;
}

ItemDropUIRootManager::~ItemDropUIRootManager()
{
    if (this == static_instance)
    {
        static_instance = nullptr;
    }
}

int ItemDropUIRootManager::GetArtifactCategoryAssetID(ArtifactDropType artifactDropType)
{
    int id = 0;
    size_t categoryIndex = static_cast<size_t>(artifactDropType);
    if (categoryIndex < ReflectFields->ArtifactsCategoryAssetID.size())
    {
        id = ReflectFields->ArtifactsCategoryAssetID[categoryIndex];
    }
    return id;
}

void ItemDropUIRootManager::DeserializedReflectEvent()
{
    ReflectFields->ArtifactsCategoryAssetID.resize(rfl::get_enumerator_array<ArtifactDropType>().size());
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

        if (ImGui::TreeNodeEx("Artifact Category Asset Setting", ImGuiTreeNodeFlags_DefaultOpen))   
        {
            ImGuiHelper::HoveredToolTip(u8"유물 카테고리 UI 에셋 ID 입니다.");
            int i = 0;
            for (auto& id : ReflectFields->ArtifactsCategoryAssetID)
            {
                constexpr auto     category  = rfl::get_enumerator_array<ArtifactDropType>();
                static std::string inputBuff = STR_NULL;
                inputBuff = UmFileSystem.GetPathFromAssetID(id).string();
                if (inputBuff.empty())
                {
                    inputBuff = STR_NULL;
                }              
                auto& [str, value] = category[i];
                ImGui::DragInt(str.data(), &id);
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
                    {
                        DragDropAsset::Data* data      = static_cast<DragDropAsset::Data*>(payLoad->Data);
                        File::Path           path      = data->GetPath();
                        const auto           extension = path.extension();
                        if (extension == L".png" || extension == L".dds")
                        {
                            if (int assetID = UmFileSystem.GetAssetIDFromPath(path); 0 != assetID)
                            {
                                id = assetID;
                            }
                        }
                    }
                    ImGui::EndDragDropTarget();
                }
                ImGuiHelper::HoveredToolTip(inputBuff);
                ++i;
            }
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
}

void ItemDropUIRootManager::Reset() 
{
    
}

void ItemDropUIRootManager::Awake()
{
    if (this != static_instance)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"ItemDropUIRootManager는 하나만 존재해야 합니다.");
        GameObject::Destroy(this);
    }
    else
    {
        gameObject->AddTag(ItemDropUIRootManager::TAG);
        gameObject->ActiveSelf = false;
        Base::Awake();
    }
}

