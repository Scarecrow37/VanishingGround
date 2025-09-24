#include "pchScripts.h"
#include "TurnQueueViewModel.h"

#include "TurnSystem/TurnActor/TurnActor.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enum/EnemyEnum.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
#include "WeaponSystem/WeaponSystem.h"
#include "ExcelDataSystem/ExcelDataSystem.h"
#include "Utility/SingletonHelper.h"

struct GetPortraitGuid
{
    File::Guid operator()(const EnemyType enemyType) const
    {
        File::Guid portraitGuid;
        if (ExcelDataSystem* dataSystem = SingletonComponent<ExcelDataSystem>::GetInstance())
        {
            if (std::unique_ptr<ExcelDataBase> dataBase = dataSystem->FindExcelDataBase(u8"에셋 테이블"))
            {
                int assetID = 0;
                switch (enemyType)
                {
                    case EnemyType::MONSTER_A: 
                    {
                        size_t rowIndex = dataBase->FindRowIndex(u8"몬스터A_턴", u8"Note");
                        if (rowIndex != ExcelDataBase::FIND_INDEX_FAIL)
                        {
                            std::string_view data = dataBase->FindData(rowIndex, u8"ID");
                            assetID               = std::stoi(data.data());
                        }
                        break;
                    }
                    case EnemyType::MONSTER_B: 
                    {
                        size_t rowIndex = dataBase->FindRowIndex(u8"몬스터B_턴", u8"Note");
                        if (rowIndex != ExcelDataBase::FIND_INDEX_FAIL)
                        {
                            std::string_view data = dataBase->FindData(rowIndex, u8"ID");
                            assetID               = std::stoi(data.data());
                        }
                        break;
                    }
                    case EnemyType::MONSTER_C: 
                    {
                        size_t rowIndex = dataBase->FindRowIndex(u8"몬스터C_턴", u8"Note");
                        if (rowIndex != ExcelDataBase::FIND_INDEX_FAIL)
                        {
                            std::string_view data = dataBase->FindData(rowIndex, u8"ID");
                            assetID               = std::stoi(data.data());
                        }
                        break;
                    }            
                }
                portraitGuid = UmFileSystem.GetGuidFromAssetID(assetID);
            }
        }

        return portraitGuid;
    }

    File::Guid operator()(const int weaponId) const
    {
        File::Guid portraitGuid;
        if (ExcelDataSystem* dataSystem = SingletonComponent<ExcelDataSystem>::GetInstance())
        {
            if (std::unique_ptr<ExcelDataBase> dataBase = dataSystem->FindExcelDataBase(u8"무기"))
            {
                std::string idStr = std::to_string(weaponId);
                size_t rowIndex = dataBase->FindRowIndex((const char8_t*)idStr.c_str(), u8"ID");
                if (rowIndex != dataBase->FIND_INDEX_FAIL)
                {
                    std::string_view data    = dataBase->FindData(rowIndex, u8"Turn Icon ID");
                    int              assetID = std::stoi(data.data());
                    portraitGuid             = UmFileSystem.GetGuidFromAssetID(assetID);
                    if (portraitGuid.IsNull())
                    {
                        UmLogger.Log(LogLevel::LEVEL_WARNING, "Unknown weapon ID: " + std::to_string(assetID));
                    }
                }
            }
        }
       return portraitGuid;
    }
};

TurnQueueViewModel::TurnQueueViewModel(MVVM::Model<std::deque<std::pair<int, TurnActor*>>>& model)
    : ViewModel(model)
{
    
}

std::vector<TurnUIData> TurnQueueViewModel::Convert(const std::deque<std::pair<int, TurnActor*>>& value)
{
    _turnQueueData.clear();

    bool isFocus = true;
    for (const auto & slotAndActor : value)
    {
        if (TurnMode::IsPlayerActorSlot(slotAndActor))
        {
            WeaponSystem* weaponSystem = SingletonComponent<WeaponSystem>::GetInstance();
            const int     slotIndex    = slotAndActor.first;
            WeaponStats&  stats        = weaponSystem->GetWeaponStatsAtIndex(slotIndex);
            int           weaponId     = stats.WeaponID;
            File::Guid    frameGuid    = isFocus ? UmFileSystem.GetGuidFromAssetID(110054) : UmFileSystem.GetGuidFromAssetID(110052);
            File::Guid portraitGuid    = GetPortraitGuid()(weaponId);
            TurnUIData    data{.ActorPortrait = portraitGuid, .Frame = frameGuid};
            _turnQueueData.push_back(data);
        }
        else
        {
            TurnActor* actor = slotAndActor.second;
            if (const Enemy*     enemy = dynamic_cast<Enemy*>(actor); nullptr != enemy)
            {

                const EnemyType enemyType = enemy->Type;
                const File::Guid portraitGuid = GetPortraitGuid()(enemyType);
                const File::Guid frameGuid    = isFocus ? UmFileSystem.GetGuidFromAssetID(110055) : UmFileSystem.GetGuidFromAssetID(110053);
                TurnUIData    data{.ActorPortrait = portraitGuid, .Frame = frameGuid};
                _turnQueueData.push_back(data);
            }
            else
            {
                UmLogger.Log(LogLevel::LEVEL_WARNING, "Actor가 Player도 Enemy도 아닙니다.");
            }

        }

        if (isFocus)
        {
            isFocus = false;
        }
    }

    return _turnQueueData;
}