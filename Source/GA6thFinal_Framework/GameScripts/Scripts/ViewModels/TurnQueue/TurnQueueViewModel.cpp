#include "pchScripts.h"
#include "TurnQueueViewModel.h"

#include "TurnSystem/TurnActor/TurnActor.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enum/EnemyEnum.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
#include "WeaponSystem/WeaponSystem.h"

struct GetEnemyFrameGuid
{
    File::GuidRef operator()() const
    {
        const File::Path& path = UmFileSystem.GetPathFromAssetID(110053);
        return path.ToGuid();
    }
};

struct GetPlayerFrameGuid
{
    File::GuidRef operator()() const
    {
        const File::Path& path = UmFileSystem.GetPathFromAssetID(110052);
        return path.ToGuid();
    }
};

struct GetPortraitGuid
{
    File::GuidRef operator()(const EnemyType enemyType) const
    {
        File::GuidRef portraitGuid;
        switch (enemyType)
        {
        case EnemyType::MONSTER_A:
            [[fallthrough]];
        case EnemyType::MONSTER_B:
            [[fallthrough]];
        case EnemyType::MONSTER_C:
            const File::Path& path = UmFileSystem.GetPathFromAssetID(113301);
            portraitGuid = path.ToGuid();
        }
        return portraitGuid;
    }

    File::GuidRef operator()(const int weaponId) const
    {
        File::GuidRef portraitGuid;
        switch (weaponId)
        {
        case 11004: // 녹슨자의 검
        {
            const File::Path& path = UmFileSystem.GetPathFromAssetID(113000);
            portraitGuid           = path.ToGuid();
        }
        break;
        case 11200: // 돌격 대장의 망치
        {
            const File::Path& path = UmFileSystem.GetPathFromAssetID(113200);
            portraitGuid           = path.ToGuid();
        }
        break;
        case 11000: // 돌파자의 장검
        {
            const File::Path& path = UmFileSystem.GetPathFromAssetID(113001);
            portraitGuid           = path.ToGuid();
        }
        break;
        case 11101: // 제물의 단검
        {
            const File::Path& path = UmFileSystem.GetPathFromAssetID(113100);
            portraitGuid           = path.ToGuid();
        }
        break;
        default:
            UmLogger.Log(LogLevel::LEVEL_WARNING, "Unknown weapon ID: " + std::to_string(weaponId));
            portraitGuid = File::NULL_GUID; // Default to SWORD
            break;
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

    for (const auto & slotAndActor : value)
    {
        if (TurnMode::IsPlayerActorSlot(slotAndActor))
        {
            WeaponSystem* weaponSystem = WeaponSystem::GetInstance();
            const int           slotIndex    = slotAndActor.first;
            WeaponStats& stats = weaponSystem->GetWeaponStatsAtIndex(slotIndex);
            int weaponId   = stats.WeaponID;
            File::GuidRef           frameGuid    = GetPlayerFrameGuid()();
            File::GuidRef       portraitGuid = GetPortraitGuid()(weaponId);
            TurnUIData data{.ActorPortrait = portraitGuid, .Frame = frameGuid};
            _turnQueueData.push_back(data);
        }
        else
        {
            TurnActor* actor = slotAndActor.second;
            if (const Enemy*     enemy = dynamic_cast<Enemy*>(actor); nullptr != enemy)
            {

                const EnemyType enemyType = enemy->Type;
                const File::GuidRef portraitGuid = GetPortraitGuid()(enemyType);
                const File::GuidRef frameGuid    = GetEnemyFrameGuid()();
                TurnUIData    data{.ActorPortrait = portraitGuid, .Frame = frameGuid};
                _turnQueueData.push_back(data);
            }
            else
            {
                UmLogger.Log(LogLevel::LEVEL_WARNING, "Actor가 Player도 Enemy도 아닙니다.");
            }

        }
    }

    return _turnQueueData;
}