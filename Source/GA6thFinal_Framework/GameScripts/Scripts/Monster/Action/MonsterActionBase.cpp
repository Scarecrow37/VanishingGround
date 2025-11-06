#include "pchScripts.h"
#include "MonsterActionBase.h"
#include "Utility/SingletonHelper.h"
#include "Monster/System/MonsterSystem.h"

#include "Animation/AnimationComponent.h"
#include "Particle/ParticleComponent.h"

#include "TurnSystem/TurnMode/TurnMode.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"

#include "BattleSystem/Battle.h"
#include "Stats/Enemy/EnemyStatsComponent.h"
#include "Stats/Enemy/EnemyStats.h"


namespace Monster
{
    namespace Action
    {
        Base::Base(std::string_view animationKey, float delayTime) 
            : _animationKey(animationKey) 
            , _waitActionTime(delayTime) {}
        Base::Base()  = default;
        Base::~Base() = default;

        void Base::ProcessActionEnter()
        {
            _waitAnimationEnd   = false;
            _waitActionTimeEnd  = false;
            Refresh();

            ProcessAnimation(_animationKey);
            ProcessActionDelay();

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
        ActionParam Base::GetActionParam(size_t index) const
        {
            assert(index >= 1); // [assert] 액션 파라미터 인덱스는 1부터 시작합니다. (엑셀 데이터에서 1부터 시작하기 때문)
            size_t subOne = index - 1;
            if (subOne < _actionParams.size())
            {
                return _actionParams[subOne];
            }
            return ActionParam();
        }
        const std::vector<ActionParam>& Base::GetAllActionParams() const
        {
            return _actionParams;
        }
        TokenParam Base::GetTokenParam(size_t index) const
        {
            assert(index >=  1); // [assert] 토큰 파라미터 인덱스는 1부터 시작합니다. (엑셀 데이터에서 1부터 시작하기 때문)
            size_t subOne = index - 1;
            if (subOne < _tokenParams.size())
            {
                return _tokenParams[subOne];
            }
            return TokenParam();
        }
        const std::vector<TokenParam>& Base::GetAllTokenParams() const
        {
            return _tokenParams;
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
        void Base::ProcessAnimation(std::string_view animKey) 
        {
            bool result = false;
            if (false == _animationKey.empty())
            {
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
                                animator->SetCurrentAnimationPopCondition([](const AnimationData& data) {
                                    return data.IsEnd();
                                }); // 애니메이션이 끝날 경우 Pop

                                auto weakOwner = GetWeakOwner();
                                animator->SetCurrentAnimationPopCallback([weakOwner, this]() {
                                    if (false == weakOwner.expired())
                                    {
                                        _waitAnimationEnd = true;
                                    }
                                });
                            }
                        }
                        animator->EndBuildOverrideAnimation();
                    }
                }
            }
            if (false == result)
            {
                _waitAnimationEnd = true;
            }
        }
        void Base::ProcessActionDelay()
        {
            if (_waitActionTime > 0.0f)
            {
                std::weak_ptr<Enemy> weakOwner = _weakOwner;
                UmTime.Invoke(_waitActionTime, [this, weakOwner]() {
                    if (false == weakOwner.expired())
                    {
                        _waitActionTimeEnd = true;
                    }
                });
            }
            else
            {
                _waitActionTimeEnd = true;
            }
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
                return true;
            }
            return false;
        }
        void Base::Reset()
        {
            _waitAnimationEnd  = false;
            _waitActionTimeEnd = false;
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
        std::weak_ptr<CharacterBase> Base::GetTargetFromActionContext(const ActionContext& actionContext) const
        {
            const std::string& targetStr = actionContext.Target;
            return GetTargetFromString(targetStr);
        }
        std::weak_ptr<CharacterBase> Base::GetTargetFromString(std::string_view targetStr) const
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
        std::weak_ptr<CharacterBase> Base::GetTargetFromID(DataID targetID) const
        {
            std::weak_ptr<CharacterBase> weakTarget;
            if (MonsterSystem* system = SingletonComponent<MonsterSystem>::GetInstance())
            {
                auto weakEnemies = system->GetSpawnedEnemiesFromID(targetID);
                if (weakEnemies)
                {
                    for (const auto& weakEnemy : *weakEnemies)
                    {
                        if (auto sharedEnemy = weakEnemy.lock())
                        {
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