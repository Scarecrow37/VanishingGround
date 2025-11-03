#pragma once
#include "Condition/TurnActionCondition.h"
#include <BattleSystem/Battle.h>

// Condition 클래스 등록을 위한 레지스터
#define REGISTER_TURN_ACTION_CONDITION(CLASS) REGISTER_CLASS(TurnAction, CLASS)

class CharacterBase;
class Player;
struct PlayerStats;
class Enemy;
struct EnemyStats;
class WeaponElement;
struct WeaponStats;
class TokenInventory;

//턴 라이프 사이클 사용을 위한 Base 클래스입니다.
class TurnAction abstract : public ReflectSerializer, public FactoryConstructor<TurnActionCondition>
{
    USING_PROPERTY(TurnAction)
    friend class TurnMode;
public:
    static void ImGuiDrawActionMaker(std::string_view windowID, std::unique_ptr<TurnAction>& action, bool& showActionEditor);
    static void ImGuiDrawActionMaker(std::string_view windowID, std::shared_ptr<TurnAction>& action, bool& showActionEditor);
    static void ImGuiDrawActionMaker(std::string_view windowID, TurnAction& action, bool& showActionEditor);

    // 2개 이상의 조건의 연산을 정의합니다.
    enum class ConditionOperator
    {
        AND,
        OR
    };

    TurnAction() = default;
    virtual ~TurnAction()
    { 
        SetDestroy();
    }

    /// <summary>
    /// 이 액션을 라이프 사이클에서 제외합니다.
    /// </summary>
    void SetDestroy()
    {
        if (_isDestroy)
        {
            OnDestroy();
            *_isDestroy = true;
            _isDestroy  = nullptr;
        }
    }

    /// <summary>
    /// 이 액션의 life cycle이 활성화 되어있는지 확인합니다.
    /// </summary>
    bool IsValidAction() const { return _isDestroy != nullptr; }

    /// <summary>
    /// 등록된 Condition 객체들의 조건을 평가합니다.
    /// </summary>
    /// <returns></returns>
    virtual bool EvaluateConditions();

    /// <summary>
    /// Condition 객체를 등록합니다.
    /// </summary>
    /// <typeparam name="T"></typeparam>
    template<typename T>
    void AddCondition()
    {
        static_assert(std::is_base_of_v<TurnActionCondition, T>, "This type does not derive from TurnActionCondition.");
        auto& map = GetInstanceConstructors();
        std::string key = typeid(T).name();
        auto findIter = map.find(key);
        if (findIter != map.end())
        {   
            T* condition = findIter->second();
            _conditions.emplace_back(condition);
        }
        else
        {
            std::string msg = std::format("{}{}", key, (const char*)u8"는 등록되지 않은 Condition 클래스 입니다.");
            UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
        }
    }

    /// <summary>
    /// 현재 추가된 Condition들을 반환합니다.
    /// </summary>
    /// <returns></returns>
    const std::vector<std::unique_ptr<TurnActionCondition>>& GetConditions() const { return _conditions; }

    /// <summary>
    /// Condition들의 정보를 설명하는 문자열을 반환합니다.
    /// </summary>
    /// <returns></returns>
    const std::string& GetConditionsInfo() const;

    /*액션 조건이 true를 평가했을때 호출되는 함수 객체입니다.*/
    std::function<void()> OnActionActive;

public:
    /*Action의 이름을 반환해야합니다.*/
    virtual const std::string& GetActionName() = 0;

    /*Action의 효과를 정의하는 내용을 반환해야합니다.*/
    virtual const std::string& GetActionInfo() = 0;

    /*편집을 위한 ImGui 함수를 구현해야합니다.*/
    virtual void ImGuiDrawActionEditor() = 0;

    /// <summary>
    /// 턴 모드에 이 액션이 추가될때 호출됩니다.
    /// </summary>
    virtual void OnAddedAction() {};

    /// <summary>
    /// SetDestory가 호출될때 호출됩니다.
    /// </summary>
    virtual void OnDestroy() {}

    /// <summary>전투가 시작될 때 호출됩니다.</summary>
    virtual void OnCombatStart() {}

    /// <summary>라운드가 시작될 때 호출됩니다.</summary>
    virtual void OnRoundStart() {}

    /// <summary>라운드가 끝날 때 호출됩니다.</summary>
    virtual void OnRoundEnd() {}

    /// <summary>인자로 넘어온 캐릭터의 턴이 시작되면 호출됩니다.</summary>
    /// <param name="destination">턴이 시작된 대상</param>
    virtual void OnTurnStart(CharacterBase& destination) {}

    /// <summary>인자로 넘어온 캐릭터의 턴이 종료되면 호출됩니다.</summary>
    virtual void OnTurnEnd(CharacterBase& destination) {}

    /// <summary>
    /// 플레이어가 사망하면 호출됩니다.
    /// </summary>
    /// <param name="player"></param>
    virtual void OnPlayerDead(Player& player) {}

    /// <summary>
    /// 적이 사망하면 호출됩니다.
    /// </summary>
    /// <param name="enemy"></param>
    virtual void OnEnemyDead(Enemy& enemy) {}

    /// <summary>
    /// 플레이어가 무기 공격을 통해 적을 죽이면 호출합니다.
    /// </summary>
    /// <param name="enemy"></param>
    /// <param name="weapon"></param>
    virtual void OnEnemyDeadByWeapon(Enemy& enemy, WeaponElement& weapon) {}

    /// <summary>
    /// QTE가 시작할 때 호출됩니다. 
    /// </summary>
    /// <param name="player"></param>
    virtual void OnPlayerQTEStart(Player& player) {}

    /// <summary>
    /// 플레이어 QTE 판정 종료후 호출됩니다.
    /// </summary>
    /// <param name="player"></param>
    virtual void OnPlayerQTEResult(Player& player, const QTE::OverallResult& result) {}

    /// <summary>
    /// 플레이어가 공격할 적을 선택한 뒤 호출됩니다.
    /// </summary>
    /// <param name="targetFlag"></param>
    virtual void OnPlayerBattleTargetSelected(Battle::EnemyTargetFlag& targetFlag) {}

    /// <summary>
    /// 플레이어의 연격 데미지 계산 전에 호출됩니다.
    /// </summary>
    /// <param name="attacker"></param>
    /// <param name="attackerStats"></param>
    /// <param name="weaponStats"></param>
    /// <param name="target"></param>
    /// <param name="targetStats"></param>
    virtual void OnPlayerBattleCalculateChainModifier(Player& attacker, PlayerStats& attackerStats,
                                                     WeaponStats& weaponStats, Enemy& target, EnemyStats& targetStats)
    {
    }

    /// <summary>
    /// 플레이어의 배틀 데미지 계산 전에 호출됩니다.
    /// </summary>
    /// <param name="attacker :">플레이어 컴포넌트</param>
    /// <param name="attackerStats :">실제 데미지 계산에 사용될 스텟</param>
    /// <param name="weaponStats :">실제 데미지 계산에 사용될 스텟</param>
    /// <param name="target :">공격 당하는 적 컴포넌트</param>
    /// <param name="targetStats :">실제 데미지 계산에 사용될 스텟</param>
    virtual void OnPlayerBattleCalculateDamageModifier(Player& attacker, PlayerStats& attackerStats,
                                                       WeaponStats& weaponStats, Enemy& target, EnemyStats& targetStats)
    {
    }

    /// <summary>
    /// 적의 배틀 데미지 계산 직전에 호출됩니다.
    /// </summary>
    /// <param name="attacker :">적 컴포넌트</param>
    /// <param name="attackerStats :">실제 계산에 사용되는 적 스텟</param>
    /// <param name="target :">플레이어 컴포넌트</param>
    /// <param name="targetStats :">실제 계산에 사용되는 플레이어 스텟</param>
    virtual void OnEnemyBattleCalculateDamageModifier(Enemy& attacker, EnemyStats& attackerStats, Player& target,
                                                      PlayerStats& targetStats)
    {
    }

    /// <summary>
    /// 적의 연격 데미지 계산 전에 호출됩니다.
    /// </summary>
    /// <param name="attacker"></param>
    /// <param name="attackerStats"></param>
    /// <param name="weaponStats"></param>
    /// <param name="target"></param>
    /// <param name="targetStats"></param>
    virtual void OnEnemyBattleCalculateChainModifier(Enemy& attacker, EnemyStats& attackerStats, Player& target,
                                                     PlayerStats& targetStats)
    {
    }

    /// <summary>Player가 공격할 때 MISS등 관계 여부 상관 없이 데미지 계산 전에 호출됩니다.</summary>
    virtual void OnPlayerBattlePreCalculate(Player& attacker, PlayerStats& attackerStats, WeaponStats& weaponStats,
                                     Enemy& target, EnemyStats& targetStats, const QTE::NoteResult& result)
    {
    }

    /// <summary>
    /// 토큰을 부여하기 직전에 호출합니다.
    /// </summary>
    /// <param name="target :">부여 대상</param>
    /// <param name="tokenID :">부여하는 토큰 아이디</param>
    /// <param name="tokenCount :">부여하는 갯수</param>
    virtual void OnTokenAddedStart(CharacterBase& target, int& tokenID, int& tokenCount) {};

    /// <summary>
    /// 토큰을 부여한뒤 호출합니다.
    /// </summary>
    /// <param name="target :">부여 대상</param>
    /// <param name="tokenID :">부여한 토큰 아이디</param>
    /// <param name="tokenCount :">부여한 갯수</param>
    virtual void OnTokenAddedEnd(CharacterBase& target, int tokenID, int tokenCount) {};

    /// <summary>
    /// 토큰 스택을 제거시키기 직전에 호출합니다.
    /// </summary>
    /// <param name="target :">부여 대상</param>
    /// <param name="tokenID :">제거하는 토큰 아이디</param>
    /// <param name="tokenCount :">제거하는 갯수</param>
    virtual void OnTokenRemovedStart(CharacterBase& target, int& tokenID, int& tokenCount) {};

    /// <summary>
    /// 토큰 스택을 제거시키기 직전에 호출합니다.
    /// </summary>
    /// <param name="target :">부여 대상</param>
    /// <param name="tokenID :">제거된 토큰 아이디</param>
    /// <param name="tokenCount :">제거된 갯수</param>
    virtual void OnTokenRemovedEnd(CharacterBase& target, int tokenID, int tokenCount) {};

    /// <summary>
    /// 플레이어에 TakeDamage가 들어가기 직전에 호출됩니다.
    /// </summary>
    /// <param name="player :">대상</param>
    /// <param name="damage :">들어갈 데미지</param>
    virtual void OnPlayerTakeDamageStart(Player& target, int& damage) {}

    /// <summary>
    /// 플레이어에 TakeDamage가 들어간 후 호출됩니다.
    /// </summary>
    /// <param name="target :">대상</param>
    /// <param name="damage :">들어간 데미지</param>
    virtual void OnPlayerTakeDamageEnd(Player& target, int damage) {}
    
    /// <summary>
    /// Enemy에 TakeDamage가 들어가기 직전에 호출됩니다.
    /// </summary>
    /// <param name="target :">대상</param>
    /// <param name="damage :">들어갈 데미지</param>
    virtual void OnEnemyTakeDamageStart(Enemy& target, int& damage) {}

    /// <summary>
    /// Enemy에 TakeDamage가 들어가기 직전에 호출됩니다.
    /// </summary>
    /// <param name="target :">대상</param>
    /// <param name="damage :">들어갈 데미지</param>
    virtual void OnEnemyTakeDamageEnd(Enemy& target, int damage) {}

    /// <summary>
    /// 플레이어 무기의 Speed 사용할때 호출됩니다.
    /// </summary>
    /// <param name="weapon :">해당 무기</param>
    /// <param name="plusSpeed :">부여할 추가 속도</param>
    virtual void OnWeaponRoundSpeedApply(WeaponElement& weapon, int& plusSpeed) {}

    /// <summary>
    /// 소멸 계시 뽑기를 실행할때 호출됩니다.
    /// </summary>
    /// <param name="count"></param>
    virtual void OnRandomExtinctionPushPlayer(size_t& count) {}

    /// <summary>
    /// 이 액션이 악세서리로 장착되면 호출됩니다.
    /// </summary>
    virtual void OnEquipAccessory() {}

    /// <summary>
    /// 플레이어가 스턴에 걸리면 호출됩니다.
    /// </summary>
    virtual void OnPlayerStun(Player& player) {}

    /// <summary>
    /// 적이 스턴에 걸리면 호출됩니다.
    /// </summary>
    /// <param name="enemy"></param>
    virtual void OnEnemyStun(Enemy& enemy){}

    /// <summary>
    /// WeaponViewModel의 Convert가 요청될때마다 호출됩니다.
    /// </summary>
    virtual void OnConvertWeaponViewModel(WeaponStats& stats) {}

    /// <summary>
    /// 플레이어가 토큰 데미지를 입기 전에 호출됩니다.
    /// </summary>
    virtual void OnPlayerTokenTakeDamage(int tokenID, int& damage) {}

    /// <summary>
    /// 적이 토큰 데미지를 입기 전에 호출됩니다.
    /// </summary>
    virtual void OnEnemyTokenTakeDamage(int tokenID, int& damage) {}
    
    /// <summary>
    /// 캐릭터의 연격 유지 라운드 수를 사용할때 호출됩니다.
    /// </summary>
    /// <param name="character"></param>
    /// <param name="maxChainRoundCount"></param>
    virtual void OnCharacterMaxChainRoundCountUse(CharacterBase& character, int& maxChainRoundCount) {}

    /// <summary>
    /// (몬스터 전용) 이 액션이 몬스터의 커스텀 액션에 있으면 호출됩니다.
    /// </summary>
    virtual void OnEnemyCombatStartPhase(Enemy& enemy) {}

    /// <summary>
    /// (몬스터 전용) 이 액션이 몬스터의 커스텀 액션에 있으면 호출됩니다.
    /// </summary>
    virtual void OnEnemyTurnEnd(Enemy& enemy) {}

public:
    REFLECT_PROPERTY(ActionName, ActionInfo, LogicOperator)

    GETTER_ONLY(const std::string&, ActionName) { return GetActionName(); }
    // 계시 이름
    // type : const std::string&
    PROPERTY(ActionName)

    GETTER_ONLY(const std::string&, ActionInfo) { return GetActionInfo(); }
    // 액션 정보
    // type : const std::string&
    PROPERTY(ActionInfo)

    GETTER(ConditionOperator, LogicOperator) { return ReflectFields->LogicOperator; }
    SETTER(ConditionOperator, LogicOperator) { ReflectFields->LogicOperator = value; }
    // 연산자
    // type : ConditionOperator
    PROPERTY(LogicOperator)

    GETTER_ONLY(size_t, ConditionCount) { return _conditions.size(); }
    // 현재 이 액션의 조건 객체의 개수
    // type : size_t
    PROPERTY(ConditionCount)

protected:
    /// <summary>
    /// 조건 직렬화 데이터 타입 <typeid().name(), JSON>
    /// </summary>
    using ConditionDataType = std::pair<std::string, std::string>;
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    std::vector<ConditionDataType> ConditionDatas;
    ConditionOperator LogicOperator = ConditionOperator::AND;
    REFLECT_FIELDS_END(TurnAction)

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;

    /// <summary>
    /// 조건 수정용 Imgui 에디터를 드로우 합니다.
    /// </summary>
    void ImguiDrawConditionEditor();

private:
    bool* _isDestroy = nullptr;
    std::vector<std::unique_ptr<TurnActionCondition>> _conditions;

private:
    void ConditionsToReflectDatas();
    void ReflectDatasToConditions();

    /// <summary>
    /// 등록된 Condition 객체들의 조건을 실제로 평가하는 함수입니다.
    /// </summary>
    /// <returns></returns>
    bool EvaluateConditionsEx();
    
public:
    TurnAction& CopyAction(const TurnAction& rhs)
    {
        if (this == &rhs) // Self-assignment check
            return *this;

        TurnAction& rhsAction  = const_cast<TurnAction&>(rhs);
        std::string data = rhsAction.SerializedReflectFields();
        DeserializedReflectFields(data);
        return *this;
    }
    explicit TurnAction(const TurnAction& rhs) { CopyAction(rhs); }
    TurnAction& operator=(const TurnAction& rhs) { return CopyAction(rhs); }

};