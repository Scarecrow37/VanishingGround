#include "pchScripts.h"
#include "WeaponViewModel.h"

#include "TurnSystem/TurnActor/Character/Player/Player.h"
#include "TurnSystem/TurnMode/TurnMode.h"
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
        data.Enable                  = true;
        WeaponSystem*  weaponSystem  = SingletonComponent<WeaponSystem>::GetInstance();
        WeaponElement& currentWeapon = weaponSystem->GetCurrentWeaponElement();
        WeaponStats    stats         = currentWeapon.Stats;

        //추가로 UI에 필요한 액션 스텟 부여
        if (TurnMode* mode = SingletonComponent<TurnMode>::GetInstance())
        {
            mode->ApplyActions([&stats](TurnAction& action) 
            { 
                action.OnConvertWeaponViewModel(stats); 
            });
        }

        // 추가 속도 액션 적용
        int speed = stats.Speed;
        if (TurnMode* mode = SingletonComponent<TurnMode>::GetInstance())
        {
            mode->ApplyActions([&speed, &currentWeapon](TurnAction& action) 
            { 
                action.OnWeaponRoundSpeedApply(currentWeapon, speed); 
            });
        }

        const int        weaponId = stats.WeaponID;
        const WeaponType type     = stats.Type;
        data.WeaponIcon           = GetWeaponIcon()(weaponId);
        data.WeaponName           = stats.WeaponName;
        data.HitDamage            = std::to_string(stats.HitDamage);
        data.CriticalDamage       = std::to_string(stats.CriticalDamage);
        data.AttackCount          = std::to_string(stats.AttackCount);
        data.Description          = GetWeaponDescription()(weaponId);
        data.Speed                = std::to_string(speed);
        ImVec4 imColor            = stats.GetGradeToColor(stats.Grade);
        data.GradeColor           = Color(imColor.x, imColor.y, imColor.z, imColor.w);
    }
    else
    {
        data.Enable = false;
    }

    return data;
}