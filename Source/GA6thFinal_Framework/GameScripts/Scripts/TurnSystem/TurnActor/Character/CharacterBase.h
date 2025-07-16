#pragma once
#include "../TurnActor.h"
#include "Token/TokenInventory.h"

struct CharacterStats;
class SkeletalMeshRenderer;

class CharacterBase abstract : public TurnActor
{
    USING_PROPERTY(CharacterBase)
public:
    inline static constexpr const char* TAG = "Character";
    inline static constexpr const char* MODEL_NAME = "Model";

public:
    REFLECT_PROPERTY(
        HP,
        MaxHP, 
        MaxMP, 
        ChainCount, 
        ChainRoundCount,
        MaxChainRoundCount
        )

    GETTER_ONLY(int, MaxHP) { return GetMaxHP(); }
    PROPERTY(MaxHP)

    GETTER_ONLY(int, MaxMP) { return GetMaxMP(); }
    PROPERTY(MaxMP)
    
    int SetChainCount(int value) { return _chainCount = std::clamp(value, 0, 99); }
    GETTER_ONLY(int, ChainCount) { return _chainCount; }
    //현재 연격 수
    PROPERTY(ChainCount)

    GETTER_ONLY(int, HP) { return _hp; }
    PROPERTY(HP)

    GETTER_ONLY(int, MaxChainRoundCount) { return GetMaxChainRoundCount(); }
    PROPERTY(MaxChainRoundCount)

    //체인 라운드 카운트를 계산합니다.
    int DecrementChainRoundCount() 
    { 
        _chainRoundCount = std::clamp(_chainRoundCount - 1, 0, GetMaxChainRoundCount());
        if (_chainRoundCount == 0)
        {
            _chainCount = 0;
            _chainRoundCount = GetMaxChainRoundCount();
        }
        return _chainRoundCount;
    }
    GETTER_ONLY(int, ChainRoundCount) { return _chainRoundCount; }
    PROPERTY(ChainRoundCount)

private:
    int GetMaxHP();
    int GetMaxMP();
    int GetMaxChainRoundCount();

public:
    virtual void Revive() override;
    virtual void Dead() override;

    inline TokenInventory& GetTokenInventory() { return _tokenInventory; }

public:
    CharacterBase();
    virtual ~CharacterBase();

protected:
    virtual CharacterStats* GetCharacterStats() = 0;

protected:
    REFLECT_FIELDS_BEGIN(TurnActor)
    REFLECT_FIELDS_END(CharacterBase)

private:
    int _hp;
    int _chainCount;
    int _chainRoundCount;

    TokenInventory _tokenInventory;
    SkeletalMeshRenderer* _skeletalMeshRenderer = nullptr;

protected:
    /// <summary>
    /// <para> 이 함수는 항상 Start 함수 전에 호출되며 프리팹이 인스턴스화 된 직후에 호출됩니다.                </para>
    /// <para> 게임 오브젝트의 Active가 false 상태인 경우 Awake 함수는 true가 될때까지 호출되지 않습니다.      </para>
    /// </summary>
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
};
