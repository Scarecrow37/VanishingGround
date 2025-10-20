#pragma once
#include "Monster/Context/MonsterDataContext.h"
#include "Monster/Context/MonsterActionContext.h"
#include "Monster/Context/MonsterStatContext.h"
#include "Monster/Action/MonsterActionFactory.h"

class CharacterBase;
class Enemy;

class AnimationComponent;
class ParticleComponent;

class TokenApplyAction;

// @brief 몬스터 액션의 기본 멤버입니다. 이걸 추가하지 않으면 Factory에 추가가 불가능합니다
#define MONSTER_ACTION_DATA(id)             \
public:                                     \
    static constexpr int ID   = id;         \
    inline static int    GetActionID()      \
    {                                       \
        return ID;                          \
    }                                       \
    
namespace Monster
{
    namespace Action
    {
        class Base
        {
        public:
            Base();
            virtual ~Base();

        public:
            bool Initialize(std::weak_ptr<Enemy> owner, const ActionContext* pActionContext,
                            const std::vector<ActionParam>* pActionParams, const std::vector<TokenParam>* pTokenParams);
            void Reset();

            void ProcessActionEnter();
            void ProcessActionUpdate();
            void ProcessActionExit();
            void ProcessAnimationEvent(const Timeline::EventContext* context);

            inline int                          GetActionID() const { return _actionContext.ID; }
            inline std::weak_ptr<Enemy>         GetWeakOwner() const { return _weakOwner; }
            inline std::weak_ptr<CharacterBase> GetWeakTarget() const { return _weakTarget; }
            inline bool                         IsActionEnd() const { return _isActionEnd; }
            inline void                         SetActionEnd() { _isActionEnd = true; }

        protected: 
            // WeakPtr을 RowPtr로 변환하는 편의성 함수
            inline CharacterBase*      GetTarget() { return _weakTarget.lock().get(); }
            inline Enemy*              GetOwnerEnemy() { return _weakOwner.lock().get(); }
            inline AnimationComponent* GetAnimationComponent() { return _weakAnimation.lock().get(); }
            inline ParticleComponent*  GetParticleComponent() { return _weakParticle.lock().get(); }

            // 토큰 파라미터를 얻어옵니다. 인덱스는 0부터가 아닌 1부터 시작합니다.
            TokenApplyAction* GetTokenAction(size_t index);

            // 액션 파라미터를 얻어옵니다. 인덱스는 0부터가 아닌 1부터 시작합니다.
            ActionParam       GetActionParam(size_t index);

            // 토큰 파라미터를 얻어옵니다. 인덱스는 0부터가 아닌 1부터 시작합니다.
            TokenParam        GetTokenParam(size_t index);

            bool BeginTokenActions();
            bool EndTokenActions();

            // 배틀 처리를 수행합니다.
            void ProcessBattle(int damage, float damageScale = 1.0f);
            bool ProcessAnimation(std::string_view animKey);

            std::weak_ptr<CharacterBase> GetTargetFromActionContext(const ActionContext& actionContext);
            std::weak_ptr<CharacterBase> GetTargetFromString(std::string_view targetStr);
            std::weak_ptr<CharacterBase> GetTargetFromID(DataID targetID);

        private:
            virtual void OnActionEnter()    {};
            virtual void OnActionUpdate()   {};
            virtual void OnActionExit()     {};
            virtual void OnActionReset()    {};
            virtual void OnNotifiedAnimationEvent(const Timeline::EventContext* context) {}
            
            void Refresh();
            void RefreshTarget();

        private:
            ActionContext             _actionContext; // 액션 컨텍스트
            std::vector<ActionParam>  _actionParams;  // 액션 파라미터
            std::vector<TokenParam>   _tokenParams;   // 토큰 파라미터

            std::weak_ptr<CharacterBase>        _weakTarget;
            std::weak_ptr<Enemy>                _weakOwner;
            std::weak_ptr<AnimationComponent>   _weakAnimation;
            std::weak_ptr<ParticleComponent>    _weakParticle;

            std::vector<std::unique_ptr<TokenApplyAction>> _tokenActions;

            bool _isActionEnd = false;
        };
    }
}