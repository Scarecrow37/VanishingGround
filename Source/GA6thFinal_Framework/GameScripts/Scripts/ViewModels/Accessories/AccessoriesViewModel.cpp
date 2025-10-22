#include "pchScripts.h"
#include "AccessoriesViewModel.h"
#include "ExcelDataSystem/ExcelDataSystem.h"
#include "Utility/SingletonHelper.h"

namespace
{
    File::Guid FindIconGuid(ExcelDataBase& dataBase, const AccessoryElement& element)
    {
        File::Guid guid;
        const std::string& name = element.AccessoryName;
        size_t index =  dataBase.FindRowIndex((const char8_t*)name.c_str(), u8"Name");
        if (index != dataBase.FIND_INDEX_FAIL)
        {
            std::string_view data = dataBase.FindData(index, u8"Big Icon ID");
            if (data != dataBase.FIND_STR_FAIL)
            {
                guid = UmFileSystem.GetGuidFromAssetID(std::stoi(data.data()));
            }
        }
        if (guid.IsNull())
        {
            std::string message = name;
            message += (const char*)u8" 의 Icon 에셋이 존재하지 않습니다. ";
            message += " ID : ";
            UmLogger.Log(LogLevel::LEVEL_WARNING, message);
        }
        return guid;
    }
}

AccessoriesViewModel::AccessoriesViewModel(MVVM::Model<std::vector<AccessoryElement>>& model) 
    : 
    ViewModel(model) 
{

}

std::vector<AccessoriesUIData> AccessoriesViewModel::Convert(const std::vector<AccessoryElement>& value)
{
    _uiData.clear();
    for (auto& element : value)
    {
        AccessoriesUIData data;
        if (ExcelDataSystem* system = SingletonComponent<ExcelDataSystem>::GetInstance())
        {
            if (std::unique_ptr<ExcelDataBase> db = system->FindExcelDataBase(u8"장신구"))
            {
                data.Icon = FindIconGuid(*db, element);
            }
        }
       _uiData.push_back(data);
    }
    return _uiData;
}
