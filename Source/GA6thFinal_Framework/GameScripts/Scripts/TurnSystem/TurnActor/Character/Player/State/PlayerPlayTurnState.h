#pragma once
#include "Base/PlayerStateBase.h"
#include <BattleSystem/Battle.h>
#include <QTE/Result/QTEResult.h>
class Enemy;

/*
 * 플레이어의 턴이 시작된 상태입니다.
 */
class PlayerPlayTurnState : public PlayerStateBase, public InputReceiver
{
public:
    //플레이어의 입력 상태
    enum class InputState
    {
        //자신의 턴이 아닌상태
        NONE,
        //행동 선택 상태
        ACTION_SELECTION,
        //QTE 선택 상태
        QUICK_TIME_EVENT,
        //QTE 연출 상태
        ATTACK_EVENT
    };

    PlayerPlayTurnState();
    virtual ~PlayerPlayTurnState() override;
protected:
    // PlayerStateBase을(를) 통해 상속됨
    void OnAwake() override;
    void OnStart() override;
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate() override;

    void OnQTEFinish(const std::vector<QTE::Result>& results);
    
    void PressedButtonA(const Input::Controller& controller);
    void ReleasedButtonA(const Input::Controller& controller);

private:
    void UpdateAttackButtonHeld(float dt);
    
    void UpdateActionSelectionUI(float dt);
    void UpdateQuickTimeEventUI(float dt);
    void UpdateAttackEventUI(float dt);

    bool IsAttackable() const;
    void PushAttackTarget(Battle::EnemyTargetFlag_ target);

    // Animation
    void SetAttackReady();
    void SetAttack();
    void SetAttackEnd();

    // Callback //
    void BattleOnAttackEvent();
    void BattleOnHitEvent(const QTE::Result& result);

    Battle::EnemyTargetFlag_ GetAttackTargetFromButton(unsigned int button) const;

private:
    bool       _setImguiPosCenter;
    InputState _inputState;
    bool       _isDownAButton;
    float      _attackButtonHeldTime;
    float      _attackButtonHeldWaitTime;
    int        _attackRemaining; // 공격 남은 횟수
    std::deque<Battle::EnemyTargetFlag_> _attackTargets;

    std::map<int, class AnimationComponent*> weaponAnims;
    std::map<int, class ParticleComponent*> weaponEffects;
    
};


#include <Stats/Weapon/WeaponStats.h>
#include <Utility/SingletonHelper.h>

class AnimationComponent;
class ParticleComponent;
class WeaponModelManager;

class WeaponModelData
{
    friend class WeaponModelManager;

public:
    WeaponModelData() : _index(SIZE_MAX) {}
    WeaponModelData(size_t index, WeaponType type, GameObject* gameObject, AnimationComponent* animation,
                    ParticleComponent* particle)
        : _index(index)
        , Type(type)
        , GameObject(gameObject)
        , Animation(animation)
        , Particle(particle)
    {
    }

    const WeaponType    Type        = WeaponType::SWORD;
    GameObject*         GameObject  = nullptr;
    AnimationComponent* Animation   = nullptr;
    ParticleComponent*  Particle    = nullptr;

    inline bool IsValid() const { return GameObject != nullptr && Animation != nullptr && Particle != nullptr; }

private:
    size_t _index = SIZE_MAX;
};

class WeaponModelManager : public Component
{
    USING_PROPERTY(WeaponModelManager)

    using AnimationPool = std::vector<class AnimationComponent*>;
    using ParticlePool  = std::vector<class ParticleComponent*>;
public:
    WeaponModelManager();
    ~WeaponModelManager();

    const File::Guid& GetWeaponPrefabGuid(WeaponType type) const;

    WeaponModelData RequestAvailableWeapon(WeaponType type);
    bool            ReturnWeaponModel(WeaponModelData data);

private:
    void Awake() override;
    void Start() override;
    void Update() override;
    void OnDestroy() override;

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

private:
    SingletonComponent<WeaponModelManager> _singletonComponent{this};

    std::unordered_map<WeaponType, File::Guid>          _weaponPrefabGuidTable;
    std::unordered_map<WeaponType, AnimationPool>       _weaponAnimationTable;
    std::unordered_map<WeaponType, ParticlePool>        _weaponParticleTable;
    std::unordered_map<WeaponType, std::stack<size_t>>  _availableWeaponIndicesTable;


    REFLECT_FIELDS_BEGIN(Component)
    std::unordered_map<std::string, std::string> WeaponPrefabGuidTable;
    REFLECT_FIELDS_END(WeaponModelManager)

    constexpr static int WEAPON_POOLING_SIZE = 10;
};