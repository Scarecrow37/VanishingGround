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
            portraitGuid = File::GuidRef("c5af3e41-5d64-4baf-beda-348cbc262b56");
        }
        return portraitGuid;
    }

    File::GuidRef operator()(const WeaponStats::WeaponType weaponType) const
    {
        File::GuidRef portraitGuid;
        switch (weaponType)
        {
        case WeaponStats::WeaponType::SWORD:
            portraitGuid = File::GuidRef("561e71d2-52cb-40be-8d83-6eded3ab68a6");
            break;
        case WeaponStats::WeaponType::DAGGER:
            portraitGuid = File::GuidRef("9589c123-388a-4112-9fc3-4299510c8416");
            break;
        case WeaponStats::WeaponType::WARHAMMER:
            portraitGuid = File::GuidRef("9d24f28c-c523-4606-a8a4-aaaabd41bbdc");
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
            WeaponStats::WeaponType weaponType   = stats.Type;
            File::GuidRef           frameGuid    = GetPlayerFrameGuid()();
            File::GuidRef           portraitGuid = GetPortraitGuid()(weaponType);
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