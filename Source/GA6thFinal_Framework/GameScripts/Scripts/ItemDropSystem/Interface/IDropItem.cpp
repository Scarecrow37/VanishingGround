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

int DropItemInfo::GetArtifactCategoryAssetID(ArtifactDropType type)
{
    int id = 0;
    if (ExcelDataSystem* excelDataSystem = SingletonComponent<ExcelDataSystem>::GetInstance())
    {
        std::unique_ptr<ExcelDataBase> dataBase;
        dataBase = excelDataSystem->FindExcelDataBase(u8"전투");
        if (dataBase)
        {
            std::string_view             data;
            constexpr std::u8string_view columnKey = u8"Description";
            size_t                       rowIndex  = ExcelDataBase::FIND_INDEX_FAIL;
            switch (type)
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
        return DropItemInfo::GetArtifactCategoryAssetID(itemInfo.Category);
    case ArtifactDropType::REVELATION:
        return GetRevelationDefaultIcon(itemInfo);
    case ArtifactDropType::ERASE_REVELATION:
        return DropItemInfo::GetArtifactCategoryAssetID(itemInfo.Category);
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
