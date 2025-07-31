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
        return File::GuidRef("a4e15352-fb7b-4b98-a816-8ce992a5bae6");
    }
};

struct GetPlayerFrameGuid
{
    File::GuidRef operator()() const
    {
        return File::GuidRef("34a8ea24-4930-40b7-9804-44913d370b8d");
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
            portraitGuid = File::GuidRef("dfe97341-a308-4c16-a772-bf27a8dc8692");
        }
        return portraitGuid;
    }

    File::GuidRef operator()(const int weaponId) const
    {
        File::GuidRef portraitGuid;
        switch (weaponId)
        {
        case 1: // 녹슨자의 검
            portraitGuid = File::GuidRef("d8c48715-62dc-4e6f-b72b-d0454417a30f");
            break;
        case 2: // 돌격 대장의 망치
            portraitGuid = File::GuidRef("1400cdb7-d00b-42cd-84bb-c3a23eb1f6a8");
            break;
        case 3: // 돌파자의 장검
            portraitGuid = File::GuidRef("cb4204c9-583c-4e59-b310-fe649ba6c6f6");
            break;
        case 4: // 제물의 단검
            portraitGuid = File::GuidRef("e051b13f-3bbe-4bfb-adc2-338f88e8a8fe");
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