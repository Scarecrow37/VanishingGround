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
        switch (enemyType)
        {
        case EnemyType::MONSTER_A:
            portraitGuid = UmFileSystem.GetGuidFromAssetID(231000);
            break;
        case EnemyType::MONSTER_B:
            portraitGuid = UmFileSystem.GetGuidFromAssetID(231001);
            break;
        case EnemyType::MONSTER_C:
            portraitGuid = UmFileSystem.GetGuidFromAssetID(231002);
            break;
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

struct GetWeaponFrameGuid
{
    File::Guid operator()(bool isFocus) const
    {
        File::Guid frameGuid;
        if (isFocus)
        {
            frameGuid = UmFileSystem.GetGuidFromAssetID(450013);
        }
        else
        {
            frameGuid = UmFileSystem.GetGuidFromAssetID(450011);
        }
        return frameGuid;
    }
};

struct GetEnemyFrameGuid
{
    File::Guid operator()(bool isFocus) const 
    {
        File::Guid frameGuid;
        if (isFocus)
        {
            frameGuid = UmFileSystem.GetGuidFromAssetID(450014);
        }
        else
        {
            frameGuid = UmFileSystem.GetGuidFromAssetID(450012);
        }
        return frameGuid;
    }
};

TurnQueueViewModel::TurnQueueViewModel(MVVM::Model<std::deque<std::pair<int, TurnActor*>>>& model)
    : ViewModel(model)
{
    
}

std::vector<TurnUIData> TurnQueueViewModel::Convert(const std::deque<std::pair<int, TurnActor*>>& value)
{
    _turnQueueData.clear();

    std::vector<Enemy*> enemys = Battle::GetTargetsFromFlags(Battle::ENEMY_TARGET_FLAG_ALL);
    bool isFocus = true;
    for (const auto & slotAndActor : value)
    {
        if (TurnMode::IsPlayerActorSlot(slotAndActor))
        {
            WeaponSystem* weaponSystem = SingletonComponent<WeaponSystem>::GetInstance();
            const int     slotIndex    = slotAndActor.first;
            WeaponStats&  stats        = weaponSystem->GetWeaponStatsAtIndex(slotIndex);
            int           weaponId     = stats.WeaponID;
            File::Guid    portraitGuid = GetPortraitGuid()(weaponId);
            File::Guid    frameGuid    = GetWeaponFrameGuid()(isFocus);
            TurnUIData    data{.ActorPortrait = portraitGuid, .Frame = frameGuid, .Type = TurnUIData::ActorType::PLAYER };
            _turnQueueData.push_back(data);
        }
        else
        {
            TurnActor* actor = slotAndActor.second;
            if (const Enemy* enemy = dynamic_cast<Enemy*>(actor); nullptr != enemy)
            {
                const EnemyType enemyType     = enemy->Type;
                const File::Guid portraitGuid = GetPortraitGuid()(enemyType);
                const File::Guid frameGuid    = GetEnemyFrameGuid()(isFocus);
                TurnUIData data;
                data.ActorPortrait = portraitGuid;
                data.Frame         = frameGuid;

                Monster::SpawnPoint point = enemy->SpawnPoint;
                switch (point)
                {
                case Monster::SpawnPoint::Left:
                    data.Type = TurnUIData::ActorType::ENEMY_LEFT;
                    break;          
                case Monster::SpawnPoint::Middle:
                    data.Type = TurnUIData::ActorType::ENEMY_MIDDLE;
                    break;
                case Monster::SpawnPoint::Right:
                    data.Type = TurnUIData::ActorType::ENEMY_RIGHT;
                    break;
                case Monster::SpawnPoint::Invalid:
                default:
                    break;
                }
                     
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