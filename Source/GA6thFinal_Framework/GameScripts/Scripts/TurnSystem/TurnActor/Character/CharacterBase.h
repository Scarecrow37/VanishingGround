#pragma once
#include <QTE/Result/QTEResult.h>
#include "TurnSystem/TurnActor/TurnActor.h"
#include "Token/TokenInventory.h"
#include "Animation/Structs/AnimationData.h"

struct CharacterStats;
class SkeletalMeshRenderer;
class AnimationComponent;
class ParticleComponent;

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
        MaxChainRoundCount,
        StunResistance
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

    GETTER_ONLY(int, StunResistance) { return GetStunResistance(); }
    PROPERTY(StunResistance)

private:
    int GetMaxHP();
    int GetHP();
    int GetChainCount();
    int GetChainRoundCount();
    int GetMaxChainRoundCount();
    int GetStunResistance();

public:
    virtual CharacterStats* GetCharacterStats() = 0;

    virtual void ClearState() override;
    virtual void Revive() override;
    virtual void Dead() override;

    virtual void Heal(int amount);
    virtual void HealByPercentage(int percentage);
    virtual void TakeDamage(int damage, bool playAnim = true);
    virtual void TakeChain(int chainDamage);

    virtual void ShowDamage(int damage, std::span<std::string> sources) = 0;

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
    // 파티클 컴포넌트를 반환합니다.
    ParticleComponent* GetParticleComponent() const { return _particleComponent; }

    bool FindComponent();

protected:
    REFLECT_FIELDS_BEGIN(TurnActor)
    REFLECT_FIELDS_END(CharacterBase)

private:
    TokenInventory          _tokenInventory;
    SkeletalMeshRenderer*   _skeletalMeshRenderer = nullptr;
    AnimationComponent*     _animationComponent   = nullptr;
    ParticleComponent*      _particleComponent    = nullptr;

protected:
    virtual void Awake() override;
    virtual void Start() override;

    void InitAnimationCallback();

public:
    virtual void OnCombatStart() override;
    virtual void OnRoundStart() override;
    virtual void OnRoundEnd() override;
    virtual void OnEachTurnStart(CharacterBase* destination) override;
    virtual void OnTurnStart() override;
    virtual void OnTurnEnd() override;
    virtual void OnHit() override;
    virtual void OnKill(CharacterBase* destination) override;
    virtual void OnTokenAdded(int tokenID) override;
    virtual void OnTokenRemoved(int tokenID) override;
    virtual void OnQTEStart() override;
    virtual void OnQTEEnd() override;
    virtual void OnNotifiedAnimationEvent(const Timeline::EventContext* context);

    virtual void ImGuiDrawPropertysEvent() override;
};
