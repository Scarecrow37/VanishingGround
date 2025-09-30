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

int ItemDropUIRootManager::GetArtifactCategoryAssetID(ArtifactDropType artifactDropType)
{
    int id = 0;
    if (ExcelDataSystem* excelDataSystem = SingletonComponent<ExcelDataSystem>::GetInstance())
    {
        std::unique_ptr<ExcelDataBase> dataBase;
        dataBase = excelDataSystem->FindExcelDataBase(u8"전투");
        if (dataBase)
        {
            std::string_view data;
            constexpr std::u8string_view columnKey = u8"Description";
            size_t rowIndex = ExcelDataBase::FIND_INDEX_FAIL;
            switch (artifactDropType)
            {
            case ArtifactDropType::SWORD:
                rowIndex = dataBase->FindRowIndex(u8"보상_검", columnKey);
                break;
            case ArtifactDropType::DAGGER:
                rowIndex = dataBase->FindRowIndex(u8"보상_단검", columnKey);
                break;
            case ArtifactDropType::WARHAMMER:
                rowIndex = dataBase->FindRowIndex(u8"보상_대형망치", columnKey);
                break;
            case ArtifactDropType::ACCESSORY:
                rowIndex = dataBase->FindRowIndex(u8"보상_장신구", columnKey);
                break;
            case ArtifactDropType::REVELATION:
                rowIndex = dataBase->FindRowIndex(u8"보상_계시", columnKey);
                break;
            case ArtifactDropType::ERASE_REVELATION:
                rowIndex = dataBase->FindRowIndex(u8"보상_계시 지우기", columnKey);
                break;
            default:
                break;
            }

            if (rowIndex != ExcelDataBase::FIND_INDEX_FAIL)
            {
                data = dataBase->FindData(rowIndex, u8"ID");
                if (data != ExcelDataBase::FIND_STR_FAIL)
                {
                    id = std::stoi(data.data());
                }
            }
        }
    }
    return id;
}

int ItemDropUIRootManager::GetArtifactIconID(DropItemInfo itemInfo)
{
    if (ExcelDataSystem* excelDataSystem = SingletonComponent<ExcelDataSystem>::GetInstance())
    {
        std::unique_ptr<ExcelDataBase> dataBase;
        switch (itemInfo.Category)
        {
        case ArtifactDropType::SWORD:
        case ArtifactDropType::DAGGER:
        case ArtifactDropType::WARHAMMER:
            dataBase = excelDataSystem->FindExcelDataBase(u8"무기");
            break;
        case ArtifactDropType::ACCESSORY:
            dataBase = excelDataSystem->FindExcelDataBase(u8"장신구");
            break;
        case ArtifactDropType::REVELATION:
            dataBase = excelDataSystem->FindExcelDataBase(u8"계시");
            break;
        case ArtifactDropType::ERASE_REVELATION:
            break;
        case ArtifactDropType::Consumable:
            dataBase = excelDataSystem->FindExcelDataBase(u8"소모품");
            break;
        default:
            return 0;
        }

        if (dataBase)
        {
            const std::string& name = itemInfo.Name;
            std::u8string_view u8Name = (const char8_t*)name.data();
            size_t rowIndex = dataBase->FindRowIndex(u8Name, u8"Name");
            if (rowIndex != ExcelDataBase::FIND_INDEX_FAIL)
            {
                std::string_view id = dataBase->FindData(rowIndex, u8"Big Icon ID");
                if (id != ExcelDataBase::FIND_STR_FAIL)
                {
                    return std::stoi(id.data());
                }
            }
        }
    }

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

