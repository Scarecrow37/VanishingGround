#include "pchScripts.h"
#include "MonsterActionBase.h"
#include "Utility/SingletonHelper.h"
#include "Monster/System/MonsterSystem.h"

#include "Animation/AnimationComponent.h"
#include "Particle/ParticleComponent.h"

#include "TurnSystem/TurnMode/TurnMode.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"
#include "TurnSystem/TurnAction/Token/TokenApplyAction.h"

#include "BattleSystem/Battle.h"
#include "Stats/Enemy/EnemyStatsComponent.h"
#include "Stats/Enemy/EnemyStats.h"


namespace Monster
{
    namespace Action
    {
        Base::Base(std::string_view animationKey) : _animationKey(animationKey) {}
        Base::Base()  = default;
        Base::~Base() = default;

        void Base::ProcessActionEnter()
        {
            _isActionEnd = false;
            Refresh();
            if (false == _animationKey.empty())
            {
                if (false == ProcessAnimation(_animationKey))
                {
                    SetActionEnd();
                }
            }
            OnActionEnter();
        }
        void Base::ProcessActionUpdate()
        {
            OnActionUpdate();
        }
        void Base::ProcessActionExit()
        {
            OnActionExit();
        }
        void Base::ProcessAnimationEvent(const Timeline::EventContext* context) 
        {
            OnNotifiedAnimationEvent(context);
        }
        TokenApplyAction* Base::GetTokenAction(size_t index)
        {
            assert(index >= 1); // [assert] 토큰 액션 인덱스는 1부터 시작합니다. (엑셀 데이터에서 1부터 시작하기 때문)
            size_t subOne = index - 1;
            if (subOne < _tokenActions.size())
            {
                return _tokenActions[subOne].get();
            }
            return nullptr;
        }
        ActionParam Base::GetActionParam(size_t index)
        {
            assert(index >= 1); // [assert] 액션 파라미터 인덱스는 1부터 시작합니다. (엑셀 데이터에서 1부터 시작하기 때문)
            size_t subOne = index - 1;
            if (subOne < _actionParams.size())
            {
                return _actionParams[subOne];
            }
            return ActionParam();
        }
        TokenParam Base::GetTokenParam(size_t index)
        {
            assert(index >=  1); // [assert] 토큰 파라미터 인덱스는 1부터 시작합니다. (엑셀 데이터에서 1부터 시작하기 때문)
            size_t subOne = index - 1;
            if (subOne < _tokenParams.size())
            {
                return _tokenParams[subOne];
            }
            return TokenParam();
        }
        bool Base::BeginTokenActions() 
        {
            // 토큰 액션 적용
            if (TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance())
            {
                for (auto& tokenAction : _tokenActions)
                {
                    turnMode->AddTurnAction(tokenAction.get());
                }
                return true;
            }
            return false;
        }
        bool Base::EndTokenActions() 
        {
            // 토큰 액션 적용
            if (TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance())
            {
                for (auto& tokenAction : _tokenActions)
                {
                    tokenAction->SetDestroy();
                }
                return true;
            }
            return false;
        }
        void Base::ProcessBattle(int damage, float damageScale) 
        {
            TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance();
            if (turnMode)
            {
                Enemy*  enemy  = GetOwnerEnemy();
                Player* player = turnMode->GetPlayer();
                if (enemy && player)
                {
                    // 데미지 및 데미지 배율 설정
                    auto* statComponent = enemy->GetEnemyStats();
                    if (statComponent)
                    {
                        auto& stat            = statComponent->GetStats();
                        stat.Damage           = damage;
                        stat.DamageMultiplier = damageScale;
                    }
                    // 배틀 처리
                    Battle()(*enemy, *player);
                }
            }
        }
        bool Base::ProcessAnimation(std::string_view animKey) 
        {
            bool result = false;
            if (AnimationComponent* animator = GetAnimationComponent())
            {
                if (animator->HasAnimationMappingKey(animKey))
                {
                    animator->BeginBuildOverrideAnimation();
                    {
                        animator->ClearOverrideAnimations();
                        animator->SetNextAnimationFlags(ANIMATION_FLAG_ALWAYS_UPDATE | ANIMATION_FLAG_USE_BLEND);
                        result = animator->PushBackOverrideAnimation(animKey);
                        if (result)
                        {
                            animator->SetCurrentAnimationPopCondition(
                                [](const AnimationData& data) { return data.IsEnd(); }); // 애니메이션이 끝날 경우 Pop

                            auto weakOwner = GetWeakOwner();
                            animator->SetCurrentAnimationPopCallback([weakOwner, this]() {
                                if (false == weakOwner.expired())
                                {
                                    this->SetActionEnd();
                                }
                            });
                        }
                    }
                    animator->EndBuildOverrideAnimation();
                }
            }
            return result;
        }
        bool Base::Initialize(std::weak_ptr<Enemy> owner, const ActionContext* pActionContext,
                              const std::vector<ActionParam>* pActionParams,
                              const std::vector<TokenParam>*  pTokenParams)
        {
            if (owner.expired() == true ||
                pActionContext == nullptr ||
                pActionParams == nullptr ||
                pTokenParams == nullptr)
            {
                assert(false); // [assert] Invalid Initialize parameters.
                return false;
            }
            _weakOwner     = owner;
            _actionContext = *pActionContext;
            _actionParams  = *pActionParams;
            _tokenParams   = *pTokenParams;

            if (auto sharedOwner = _weakOwner.lock())
            {
                AnimationComponent* animationComp = sharedOwner->GetAnimationComponent();
                ParticleComponent*  particleComp  = sharedOwner->GetParticleComponent();
                if (animationComp)
                {
                    _weakAnimation = static_pointer_cast<AnimationComponent>(animationComp->GetWeakPtr().lock());
                }
                if (particleComp)
                {
                    _weakParticle  = static_pointer_cast<ParticleComponent>(particleComp->GetWeakPtr().lock());
                }
                for (size_t i = 0; i < _tokenParams.size(); ++i)
                {
                    _tokenActions.emplace_back(std::make_unique<TokenApplyAction>());
                    TokenApplyAction* tokenAction = _tokenActions.back().get();
                    tokenAction->TokenID          = _tokenParams[i].TokenID;
                    tokenAction->TokenCount       = _tokenParams[i].Count;
                }
                return true;
            }
            return false;
        }
        void Base::Reset()
        {
            _isActionEnd = false;
            OnActionReset();
        }
        void Base::Refresh()
        {
            RefreshTarget();
        }
        void Base::RefreshTarget()
        {
            _weakTarget = GetTargetFromActionContext(_actionContext);
        }
        std::weak_ptr<CharacterBase> Base::GetTargetFromActionContext(const ActionContext& actionContext)
        {
            const std::string& targetStr = actionContext.Target;
            return GetTargetFromString(targetStr);
        }
        std::weak_ptr<CharacterBase> Base::GetTargetFromString(std::string_view targetStr)
        {
            std::weak_ptr<CharacterBase> weakTarget;
            if (targetStr == "Self")
            {
                weakTarget = std::static_pointer_cast<CharacterBase>(_weakOwner.lock());
            }
            else if (targetStr == "Player")
            {
                if (TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance())
                {
                    if (Player* player = turnMode->GetPlayer())
                    {
                        weakTarget = std::static_pointer_cast<CharacterBase>(player->GetWeakPtr().lock());
                    }
                }
            }
            else
            {
                try // ID로 변환 시도
                {
                    DataID targetID = std::stoi(targetStr.data());
                    weakTarget = GetTargetFromID(targetID);
                }
                catch (...)
                {
                }
            }
            return weakTarget;
        }
        std::weak_ptr<CharacterBase> Base::GetTargetFromID(DataID targetID)
        {
            std::weak_ptr<CharacterBase> weakTarget;
            if (MonsterSystem* system = SingletonComponent<MonsterSystem>::GetInstance())
            {
                auto weakEnemies = system->GetSpawnedEnemiesFromID(targetID);
                if (weakEnemies)
                {
                    for (const auto& weakEnemy : *weakEnemies)
                    {
                        if (false == weakEnemy.expired())
                        {
                            auto sharedEnemy = weakEnemy.lock();
                            if (false == sharedEnemy->IsDead())
                            {
                                weakTarget = std::static_pointer_cast<CharacterBase>(sharedEnemy);
                                break; // 첫번째 살아있는 몬스터만 타겟으로 설정
                            }
                        }
                    }
                }
            }
            return weakTarget;
        }
    } // namespace Action
} // namespace Monster