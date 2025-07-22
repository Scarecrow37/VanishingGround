#pragma once
#include "Condition/TurnActionCondition.h"

// Condition 클래스 등록을 위한 레지스터
#define REGISTER_TURN_ACTION_CONDITION(CLASS) REGISTER_CLASS(TurnAction, CLASS)

class CharacterBase;
class Player;
struct PlayerStats;
class Enemy;
struct EnemyStats;
struct WeaponStats;

//턴 라이프 사이클 사용을 위한 Base 클래스입니다.
class TurnAction abstract : public ReflectSerializer, public FactoryConstructor<TurnActionCondition>
{
    USING_PROPERTY(TurnAction)
    friend class TurnMode;
public:
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
            *_isDestroy = true;
        }
    }

    /// <summary>
    /// 이 액션의 life cycle이 활성화 되어있는지 확인합니다.
    /// </summary>
    bool IsValidAction() { return _isDestroy != nullptr; }

    /// <summary>
    /// 등록된 Condition 객체들의 조건을 평가합니다.
    /// </summary>
    /// <returns></returns>
    bool EvaluateConditions();

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

public:
    /*Action의 이름을 반환해야합니다.*/
    virtual const std::string& GetActionName() = 0;

    /*Action의 효과를 정의하는 내용을 반환해야합니다.*/
    virtual const std::string& GetActionInfo() = 0;

    /*편집을 위한 ImGui 함수를 구현해야합니다.*/
    virtual void ImGuiDrawActionEditor() = 0;

    /// <summary>전투가 시작될 때 호출됩니다.</summary>
    virtual void OnCombatStart() {}

    /// <summary>라운드가 시작될 때 호출됩니다.</summary>
    virtual void OnRoundStart() {}

    /// <summary>라운드가 끝날 때 호출됩니다.</summary>
    virtual void OnRoundEnd() {}

    /// <summary>인자로 넘어온 캐릭터의 턴이 시작되면 호출됩니다..</summary>
    /// <param name="destination">턴이 시작된 대상</param>
    virtual void OnTurnStart(CharacterBase* destination) {}

    /// <summary>인자로 넘어온 캐릭터의 턴이 종료되면 호출됩니다.</summary>
    virtual void OnTurnEnd(CharacterBase* destination) {}

    /// <summary>
    /// 플레이어가 배틀 데미지 계산 전에 호출됩니다.
    /// </summary>
    /// <param name="attacker :">플레이어 컴포넌트</param>
    /// <param name="attackerStats :">실제 데미지 계산에 사용될 스텟</param>
    /// <param name="weaponStats :">실제 데미지 계산에 사용될 스텟</param>
    /// <param name="target :">공격 당하는 적 컴포넌트</param>
    /// <param name="targetStats :">실제 데미지 계산에 사용될 스텟</param>
    virtual void OnPlayerBattleStart(Player& attacker, PlayerStats& attackerStats, WeaponStats& weaponStats, Enemy& target, EnemyStats& targetStats) {}

    /// <summary>
    /// 적이 배틀데미지 계산 직전에 호출됩니다.
    /// </summary>
    /// <param name="attacker :">적 컴포넌트</param>
    /// <param name="attackerStats :">실제 계산에 사용되는 적 스텟</param>
    /// <param name="target :">플레이어 컴포넌트</param>
    /// <param name="targetStats :">실제 계산에 사용되는 플레이어 스텟</param>
    virtual void OnEnemyBattleStart(Enemy& attacker, EnemyStats& attackerStats, Player& target, PlayerStats& targetStats) {}

public:
    REFLECT_PROPERTY(Name, LogicOperator)

    GETTER_ONLY(const std::string&, Name) { return GetActionName(); }
    // 계시 이름
    PROPERTY(Name)

    GETTER(ConditionOperator, LogicOperator) { return ReflectFields->LogicOperator; }
    SETTER(ConditionOperator, LogicOperator) { ReflectFields->LogicOperator = value; }
    PROPERTY(LogicOperator)

protected:
    /// <summary>
    /// 조건 직렬화 데이터 타입 <typeid().name(), JSON>
    /// </summary>
    using ConditionDataType = std::pair<std::string, std::string>;
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    std::vector<ConditionDataType> _conditionDatas;
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
    
};