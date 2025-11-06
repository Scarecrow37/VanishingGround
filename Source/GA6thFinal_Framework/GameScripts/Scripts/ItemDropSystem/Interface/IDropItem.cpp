#include "pchScripts.h"
#include "IDropItem.h"
#include "ItemDropSystem/UI/ItemDropUIRootManager.h"
#include "Utility/SingletonHelper.h"
#include "ExcelDataSystem/ExcelDataSystem.h"
#include "RevelationSystem/RevelationSystem.h"

std::u8string_view DropItemInfo::GetDataBaseName(ArtifactDropType type)
{
    switch (type)
    {
    case ArtifactDropType::SWORD:
    case ArtifactDropType::DAGGER:
    case ArtifactDropType::WARHAMMER:
        return u8"무기";
    case ArtifactDropType::ACCESSORY:
        return u8"장신구";
    case ArtifactDropType::REVELATION:
        return u8"계시";
    case ArtifactDropType::Consumable:
        return u8"소모품";
    case ArtifactDropType::ERASE_REVELATION:
    default:
        return u8"";
    }
}

int DropItemInfo::GetArtifactCategoryAssetID(ArtifactDropType type, bool isMapScene)
{
    int id = 0;
    if (isMapScene)
    {
        switch (type)
        {
        case ArtifactDropType::SWORD:
            id = 440017;
            break;
        case ArtifactDropType::DAGGER:
            id = 440018;
            break;
        case ArtifactDropType::WARHAMMER:
            id = 440019;
            break;
        case ArtifactDropType::ACCESSORY:
            id = 440020;
            break;
        case ArtifactDropType::REVELATION:
            id = 440021;
            break;
        case ArtifactDropType::ERASE_REVELATION:
            id = 440022;
            break;
        default:
            id = 0;
            break;
        }
    }
    else
    {
        switch (type)
        {
        case ArtifactDropType::SWORD:
            id = 460012;
            break;
        case ArtifactDropType::DAGGER:
            id = 460013;
            break;
        case ArtifactDropType::WARHAMMER:
            id = 460014;
            break;
        case ArtifactDropType::ACCESSORY:
            id = 460016;
            break;
        case ArtifactDropType::REVELATION:
            id = 460015;
            break;
        case ArtifactDropType::ERASE_REVELATION:
            id = 460017;
            break;
        default:
            id = 0;
            break;
        }
    }
    return id;
}

int DropItemInfo::GetArtifactIconID(DropItemInfo itemInfo)
{
    if (ExcelDataSystem* excelDataSystem = SingletonComponent<ExcelDataSystem>::GetInstance())
    {
        std::u8string_view dbName = GetDataBaseName(itemInfo.Category);
        if (false == dbName.empty())
        {
            std::unique_ptr<ExcelDataBase> dataBase = excelDataSystem->FindExcelDataBase(dbName);
            if (dataBase)
            {
                const std::string& name     = itemInfo.Name;
                std::u8string_view u8Name   = (const char8_t*)name.data();
                size_t             rowIndex = dataBase->FindRowIndex(u8Name, u8"Name");
                if (rowIndex != ExcelDataBase::FIND_INDEX_FAIL)
                {
                    std::string_view id = dataBase->FindData(rowIndex, u8"Big Icon ID");
                    if (id != ExcelDataBase::FIND_STR_FAIL)
                    {
                        return std::stoi(id.data());
                    }
                }
            }
            else
            {
                std::u8string message = u8"엑셀 DB에서 ";
                message += dbName;
                message += u8"를 찾을 수 없습니다.";
                UmLogger.Log(LogLevel::LEVEL_WARNING, message);
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
        return DropItemInfo::GetArtifactCategoryAssetID(itemInfo.Category, true);
    case ArtifactDropType::REVELATION:
        return GetRevelationDefaultIcon(itemInfo);
    case ArtifactDropType::ERASE_REVELATION:
        return DropItemInfo::GetArtifactCategoryAssetID(itemInfo.Category, true);
    default:
        return 0;
    }
}

std::string DropItemInfo::GetArtifactDescription(DropItemInfo itemInfo)
{
    if (ExcelDataSystem* excelDataSystem = SingletonComponent<ExcelDataSystem>::GetInstance())
    {
        std::u8string_view dbName = GetDataBaseName(itemInfo.Category);
        if (false == dbName.empty())
        {
            std::unique_ptr<ExcelDataBase> dataBase = excelDataSystem->FindExcelDataBase(dbName);
            if (dataBase)
            {
                const std::string& name     = itemInfo.Name;
                std::u8string_view u8Name   = (const char8_t*)name.data();
                size_t             rowIndex = dataBase->FindRowIndex(u8Name, u8"Name");
                if (rowIndex != ExcelDataBase::FIND_INDEX_FAIL)
                {
                    std::string_view description = dataBase->FindData(rowIndex, u8"Description");
                    if (description != ExcelDataBase::FIND_STR_FAIL)
                    {
                        return description.data();
                    }
                }
            }
            else
            {
                std::u8string message = u8"엑셀 DB에서 ";
                message += dbName;
                message += u8"를 찾을 수 없습니다.";
                UmLogger.Log(LogLevel::LEVEL_WARNING, message);
            }
        }
    }
    return "";
}
