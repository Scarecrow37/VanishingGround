#include "pchScripts.h"
#include "EnemyActionBase.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <Animation/AnimationComponent.h>
#include <TurnSystem/TurnMode/TurnMode.h>
#include <BattleSystem/Battle.h>
#include <Stats/Enemy/EnemyStatsComponent.h>
#include <Stats/Enemy/EnemyStats.h>
#include <Audio/Table/AudioTableComponent.h>

namespace EnemyAction
{
    ActionBase::ActionBase(Enemy* owner) 
        : _owner(owner)
    {
        if (_owner)
        {
            _animator = _owner->GetAnimationComponent();
            _audioTable = _owner->GetAudioTableComponent();
        }
    }

    void ActionBase::RequestActionEnter()
    {
        _isActionEnd = false;
        OnActionEnter();
    }

    void ActionBase::RequestActionUpdate()
    {
        OnActionUpdate();
    }

    void ActionBase::RequestActionExit()
    {
        OnActionExit();
    }


    void ActionBase::ProcessBattle(int damage, float damageScale)
    {
        TurnMode* turnMode = TurnMode::GetInstance();
        if (turnMode)
        {
            Enemy*  enemy  = _owner;
            Player* player = turnMode->GetPlayer();
            if (enemy && player)
            {
                auto* statComponent = enemy->GetEnemyStats();
                if (statComponent)
                {
                    auto& stat = statComponent->GetStats();
                    stat.Damage = damage;
                    stat.DamageMultiplier = damageScale;
                }
                Battle()(*enemy, *player);
            }
        }
    }
    void ActionBase::PlaySoundFromKey(std::string_view key) 
    {
        auto* audioTable = _owner->GetAudioTableComponent();
        if (audioTable)
        {
            audioTable->Play(key.data());
        }
    }
} // namespace EnemyAction