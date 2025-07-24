#pragma once
#include "TurnSystem/TurnActor/TurnActor.h"
#include "Token/TokenInventory.h"
#include "Animation/Structs/AnimationData.h"

struct CharacterStats;
class SkeletalMeshRenderer;
class AnimationComponent;

class CharacterBase abstract : public TurnActor
{
    USING_PROPERTY(CharacterBase)
public:
    inline static constexpr const char* TAG = "Character";
    inline static constexpr const char* MODEL_NAME = "Model";
    CharacterBase();
    virtual ~CharacterBase();

public:
    REFLECT_PROPERTY(
        HP,
        MaxHP, 
        ChainCount, 
        ChainRoundCount,
        MaxChainRoundCount
        )

    GETTER_ONLY(int, MaxHP) { return GetMaxHP(); }
    PROPERTY(MaxHP)
  
    GETTER_ONLY(int, ChainCount) { return GetChainCount(); }
    //int 현재 연격 수
    PROPERTY(ChainCount)

    GETTER_ONLY(int, HP) { return GetHP(); }
    PROPERTY(HP)

    GETTER_ONLY(int, MaxChainRoundCount) { return GetMaxChainRoundCount(); }
    PROPERTY(MaxChainRoundCount)

    GETTER_ONLY(int, ChainRoundCount) { return GetChainRoundCount(); }
    //int 남은 연격 수 유지 시간
    PROPERTY(ChainRoundCount)

private:
    int GetMaxHP();
    int GetHP();
    int GetChainCount();
    int GetChainRoundCount();
    int GetMaxChainRoundCount();

public:
    virtual CharacterStats* GetCharacterStats() = 0;

    virtual void Revive() override;
    virtual void Dead() override;
    virtual void TakeDamage(int damage);
    virtual void TakeChain(int chainDamage);

    // 연격 수를 설정합니다.
    int SetChainCount(int value);

    // 체인 라운드 카운트를 계산합니다.
    int DecrementChainRoundCount();

    // 토큰 인벤토리를 반환합니다.
    TokenInventory&       GetTokenInventory() { return _tokenInventory; }
    // 스켈레탈 메쉬 렌더러를 반환합니다.
    SkeletalMeshRenderer* GetSkeletalMeshRenderer() const { return _skeletalMeshRenderer; }
    // 애니메이션 컴포넌트를 반환합니다.
    AnimationComponent*   GetAnimationComponent() const { return _animationComponent; }

protected:
    REFLECT_FIELDS_BEGIN(TurnActor)
    REFLECT_FIELDS_END(CharacterBase)

private:
    TokenInventory _tokenInventory;
    SkeletalMeshRenderer* _skeletalMeshRenderer = nullptr;
    AnimationComponent*   _animationComponent   = nullptr;

protected:
    virtual void Awake() override;

    void InitMeshModel();

public:
    virtual void OnCombatStart() override;
    virtual void OnRoundStart() override;
    virtual void OnRoundEnd() override;
    virtual void OnEachTurnStart(CharacterBase* destination) override;
    virtual void OnTurnStart() override;
    virtual void OnTurnEnd() override;
    virtual void OnHit() override;
    virtual void OnDead() override;
    virtual void OnKill(CharacterBase* destination) override;
    virtual void OnTokenAdded(int tokenID) override;
    virtual void OnTokenRemoved(int tokenID) override;

    virtual void ImGuiDrawPropertysEvent() override;

    
    // 애니메이션 리팩터링 전 임시 메서드
    enum AnimationType
    {
        IDLE,
        HIT,
        DEATH,
        ATTACK_1,
        ATTACK_2,
        ATTACK_3,
        ATTACK_4,
        ATTACK_READY,
        ATTACK_READY_LOOP,
        ATTACK,
        ATTACK_LOOP,
        ATTACK_END,
        SIZE,
    };
    virtual const char* GetAnimationName(AnimationType type) = 0;
    void SetMainAnimation(AnimationType type, int flags = 0, bool blend = true);
    void ClearOverrideAnimations();
    bool IsAnimationEnd();
};
