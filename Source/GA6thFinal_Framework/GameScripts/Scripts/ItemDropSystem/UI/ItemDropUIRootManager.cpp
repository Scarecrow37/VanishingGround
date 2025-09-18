#include "pchScripts.h"
#include "ItemDropUIRootManager.h"
#include "ItemDropSystem/UI/ArtifactUIManager.h"
#include "ItemDropSystem/ItemDropSystem.h"
#include "RevelationSystem/RevelationSystem.h"
#include "DLLExportDefine.h"

UMREAL_COMPONENT(ItemDropUIRootManager)

ItemDropUIRootManager::ItemDropUIRootManager()
{
    
}

ItemDropUIRootManager::~ItemDropUIRootManager()
{
  
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

int ItemDropUIRootManager::GetArtifactIconID(DropItemInfo itemInfo)
{
    auto find = ReflectFields->ArtifactsIconIDMap.find(itemInfo.ID);
    if (find != ReflectFields->ArtifactsIconIDMap.end())
    {
        return find->second;
    }
    else
    {
        auto GetRevelationDefaultIcon = [](const DropItemInfo& info) -> int
        {
            if (RevelationSystem* system = SingletonComponent<RevelationSystem>::GetInstance())
            {
                RevelationElement* element = system->FindElement(info.Name);
                if (element)
                {
                    RevelationGrade grade = element->Grade;
                    switch (grade)
                    {
                    case RevelationGrade::COMMON:
                        return -202000;
                    case RevelationGrade::RARE:
                        return -202001;
                    case RevelationGrade::LEGENDARY:
                        return -202002;
                    case RevelationGrade::EXTINCTION:
                        return 0;
                    default:
                        break;
                    }
                }
            }
            return 0;
        };

        switch (itemInfo.Category)
        {
        case ArtifactDropType::DAGGER:
            return -201000;
        case ArtifactDropType::WARHAMMER:
            return -201001;
        case ArtifactDropType::SWORD:
            return -201002;
        case ArtifactDropType::ACCESSORY:
            return DropItemInfo::GetArtifactCategoryAssetID(itemInfo.Category);
        case ArtifactDropType::REVELATION:
            return GetRevelationDefaultIcon(itemInfo);
        case ArtifactDropType::ERASE_REVELATION:
            return DropItemInfo::GetArtifactCategoryAssetID(itemInfo.Category);
        default:
            return 0;
        }
    }
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
    _singletonComponent.SetSingleTon();
}

void ItemDropUIRootManager::Awake()
{
    if (_singletonComponent.TrySingleTon())
    {
        gameObject->AddTag(ItemDropUIRootManager::TAG);
        gameObject->ActiveSelf = false;
        Base::Awake();
    }
}

