#include "pchScripts.h"
#include "WeaponViewModel.h"

#include "TurnSystem/TurnActor/Character/Player/Player.h"
#include "WeaponSystem/WeaponSystem.h"
#include "ExcelDataSystem/ExcelDataSystem.h"
#include "Utility/SingletonHelper.h"

struct GetWeaponIcon
{
    File::Guid operator()(const int weaponId) const
    {
        File::Guid portraitGuid;
        if (ExcelDataSystem* system = SingletonComponent<ExcelDataSystem>::GetInstance())
        {
            if (std::unique_ptr<ExcelDataBase> db = system->FindExcelDataBase(u8"무기"))
            {
                std::string idStr = std::to_string(weaponId);
                size_t      index = db->FindRowIndex((const char8_t*)idStr.c_str(), u8"ID");
                if (index != db->FIND_INDEX_FAIL)
                {
                    std::string_view data = db->FindData(index, u8"Big Icon ID");
                    if (data != db->FIND_STR_FAIL)
                    {
                        portraitGuid = UmFileSystem.GetGuidFromAssetID(std::stoi(data.data()));
                        if (portraitGuid.IsNull())
                        {
                            std::string message = "Asset ID ";
                            message += data;
                            message += " is not import";
                            UmLogger.Log(LogLevel::LEVEL_WARNING, message);
                        }
                    }
                }
            }
        }
        return portraitGuid;
    }
};

namespace
{
    struct GetWeaponDescription
    {
        std::string operator()(const int weaponId) const
        {
            std::string description;
            if (ExcelDataSystem* system = SingletonComponent<ExcelDataSystem>::GetInstance())
            {
                if (std::unique_ptr<ExcelDataBase> db = system->FindExcelDataBase(u8"무기"))
                {
                    std::string idStr = std::to_string(weaponId);
                    size_t      index = db->FindRowIndex((const char8_t*)idStr.c_str(), u8"ID");
                    if (index != db->FIND_INDEX_FAIL)
                    {
                        std::string_view data = db->FindData(index, u8"Description");
                        if (data != db->FIND_STR_FAIL)
                        {
                            description = data;
                        }
                    }
                }
            }
            return description;
        }
    };
} // namespace


WeaponViewModel::WeaponViewModel(MVVM::Model<TurnActor*>& model): MVVM::ViewModel<TurnActor*, WeaponUIData>(model) {}

WeaponUIData WeaponViewModel::Convert(TurnActor* const& value)
{
    WeaponUIData data{};
    if (nullptr != value && typeid(*value) == typeid(Player))
    {
        data.Enable                     = true;
        WeaponSystem*      weaponSystem = SingletonComponent<WeaponSystem>::GetInstance();
        const WeaponStats& state        = weaponSystem->GetCurrentWeaponElement().Stats;
        const int          weaponId     = state.WeaponID;
        const WeaponType   type         = state.Type;
        data.WeaponIcon                 = GetWeaponIcon()(weaponId);
        data.WeaponName                 = state.WeaponName;
        data.HitDamage                  = std::to_string(state.HitDamage);
        data.CriticalDamage             = std::to_string(state.CriticalDamage);
        data.Speed                      = std::to_string(state.Speed);
        data.AttackCount                = std::to_string(state.AttackCount);
        data.Description                = GetWeaponDescription()(weaponId);

        ImVec4 imColor  = state.GetGradeToColor(state.Grade);
        data.GradeColor = Color(imColor.x, imColor.y, imColor.z, imColor.w);
    }
    else
    {
        data.Enable = false;
    }

    return data;
}