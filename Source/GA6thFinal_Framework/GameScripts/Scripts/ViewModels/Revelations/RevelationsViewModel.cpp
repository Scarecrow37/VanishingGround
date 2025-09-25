#include "pchScripts.h"
#include "RevelationsViewModel.h"

#include "RevelationSystem/RevelationElement/RevelationElement.h"

#include "Utility/SingletonHelper.h"
#include "ExcelDataSystem/ExcelDataSystem.h"

struct GetRevelationIcon
{
    File::Guid operator()(const int revelationID) const
    {
        File::Guid iconGuid;
        if (ExcelDataSystem* system = SingletonComponent<ExcelDataSystem>::GetInstance())
        {
            if (std::unique_ptr<ExcelDataBase> db = system->FindExcelDataBase(u8"계시"))
            {
                std::string idStr = std::to_string(revelationID);
                size_t index = db->FindRowIndex((const char8_t*)idStr.c_str(), u8"ID");
                if (index != ExcelDataBase::FIND_INDEX_FAIL)
                {
                    std::string_view iconID = db->FindData(index, u8"Big Icon ID");
                    if (iconID != ExcelDataBase::FIND_STR_FAIL)
                    {
                        iconGuid = UmFileSystem.GetGuidFromAssetID(std::stoi(iconID.data()));
                        if (iconGuid.IsNull())
                        {
                            std::string message = "Revelation asset ID ";
                            message += iconID;
                            message += " is not import";
                            UmLogger.Log(LogLevel::LEVEL_WARNING, message);
                        }
                    }
                }
            }
        }
        return iconGuid;
    }
};

struct GetRevelationDescription
{
    std::string operator()(const int revelationID) const
    {
        std::string description = WStringToU8(L"<Description>"
                                              L"<Text color=\"#ffffff\">Unkown Description</Text>"
                                              L"</Description>");

        if (ExcelDataSystem* system = SingletonComponent<ExcelDataSystem>::GetInstance())
        {
            if (std::unique_ptr<ExcelDataBase> db = system->FindExcelDataBase(u8"계시"))
            {
                std::string idStr = std::to_string(revelationID);
                size_t      index = db->FindRowIndex((const char8_t*)idStr.c_str(), u8"ID");
                if (index != ExcelDataBase::FIND_INDEX_FAIL)
                {
                    std::string_view data = db->FindData(index, u8"Description");
                    if (ExcelDataBase::FIND_STR_FAIL != data)
                    {
                        description = data;
                    }
                }
            }
        }
        return description;
    }
};

RevelationsViewModel::RevelationsViewModel(MVVM::Model<std::vector<std::shared_ptr<RevelationElement>>>& model)
    : ViewModel(model)
{

}

std::vector<RevelationUIData> RevelationsViewModel::Convert(
    const std::vector<std::shared_ptr<RevelationElement>>& value)
{
    _uiData.clear();

    for (const auto& revelationElement : value)
    {
        const int revelationID = revelationElement->RevelationID;

        RevelationUIData uiData;

        uiData.Name        = revelationElement->ElementName;
        uiData.Icon        = GetRevelationIcon()(revelationID);
        uiData.Grade       = revelationElement->Grade;
        uiData.Description = GetRevelationDescription()(revelationID);

        _uiData.push_back(uiData);
    }

    return _uiData;
}