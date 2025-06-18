#pragma once
#include "Factory/FSMStateFactory.h"
#include "Factory/FSMConditionFactory.h"

class FiniteStateMachine : public Component
{
    USING_PROPERTY(FiniteStateMachine)
public:
    struct Transition
    {
        FSMState* CurrState;
        FSMCondition* Condition;
        FSMState* NextState;

        inline bool operator()(const Transition& a, const Transition& b) const
        { 
            void* pointersA[3] = {a.CurrState, a.Condition, a.NextState};
            void* pointersB[3] = {b.CurrState, b.Condition, b.NextState};
            for (int i = 0; i < 3; ++i)
            {
                if (pointersA[i] < pointersB[i])
                    return true;
                if (pointersA[i] > pointersB[i])
                    return false;
            }
            return false;
        }   

        inline bool operator==(const Transition& b) const
        {
            void* pointersA[3] = {this->CurrState, this->Condition, this->NextState};
            void* pointersB[3] = {b.CurrState, b.Condition, b.NextState};
            for (int i = 0; i < 3; ++i)
            {
                if (pointersA[i] != pointersB[i])
                    return false;
            }
            return true;
        }
    };

private: 
    void ImGuiDrawDebug();
    void ImguiDrawTransition();

    static const char* SelectStateImguiChild();
    const char* SelectMyStateImguiChild(bool enableAnyState = false);
    void ImguiDrawStates();

    static const char* SlectConditionImguiChild();
    const char* SelectMyConditionImguiChild();
    void ImguiDrawCondiitons();

public:
    /// <summary>
    /// 전이 객체를 추가합니다.
    /// </summary>
    template <FSM_STATE_BASE StateType, FSM_CONDITION_BASE ConditionType, FSM_STATE_BASE NextStateType>
    void AddTransition()
    {
        const char* stateKey     = typeid(StateType).name();
        const char* conditionKey = typeid(ConditionType).name();
        const char* nextStateKey = typeid(NextStateType).name();
        AddTransitionToKey(stateKey, conditionKey, nextStateKey);
    }

    /// <summary>
    /// 모든 상황에서 전이가 가능한 전이 객체를 추가합니다.
    /// </summary>
    template <FSM_CONDITION_BASE ConditionType, FSM_STATE_BASE NextStateType>
    void AddTransition()
    {
        const char* conditionKey = typeid(ConditionType).name();
        const char* nextStateKey = typeid(NextStateType).name();
        AddAnyTransitionToKey(conditionKey, nextStateKey);
    }

    /// <summary>
    /// 전이 객체를 제거합니다.
    /// </summary>
    template <FSM_STATE_BASE StateType, FSM_CONDITION_BASE ConditionType, FSM_STATE_BASE NextStateType>
    void EraseTransition()
    {
        Transition eraseTransition{};
        eraseTransition.CurrState = GetState<StateType>();
        eraseTransition.Condition = GetCondition<ConditionType>();
        eraseTransition.NextState = GetState<NextStateType>();
        EraseTransition(eraseTransition);
    }

    /// <summary>
    /// 전이 객체를 제거합니다.
    /// </summary>
    template <FSM_CONDITION_BASE ConditionType, FSM_STATE_BASE NextStateType>
    void EraseTransition()
    {
        Transition eraseTransition{};
        eraseTransition.CurrState = nullptr;
        eraseTransition.Condition = GetCondition<ConditionType>();
        eraseTransition.NextState = GetState<NextStateType>();
        EraseTransition(eraseTransition);
    }

    /// <summary>
    /// FSM의 시작 상태를 설정합니다.
    /// </summary>
    template <FSM_STATE_BASE StateType>
    void SetEntryState()
    {
        const char* key = typeid(StateType).name();
        SetEntryStateToKey(key);
    }

private:
    /// <summary>
    /// 전이 객체를 추가합니다.
    /// </summary>
    /// <param name="state :">대상 상태</param>
    /// <param name="condition :">전이 조건</param>
    /// <param name="nextState :">변경될 상태</param>
    void AddTransitionToKey(std::string_view state, std::string_view condition, std::string_view nextState);

    /// <summary>
    /// 모든 상황에서 전이가 가능한 전이 객체를 추가합니다.
    /// </summary>
    /// <param name="condition :">전이 조건</param>
    /// <param name="nextState :">변경될 상태</param>
    void AddAnyTransitionToKey(std::string_view condition, std::string_view nextState);

    /// <summary>
    /// 전이 객체를 제거합니다.
    /// </summary>
    void EraseTransition(const Transition& eraseTransition);

    /// <summary>
    /// 전이 객체를 제거합니다.
    /// </summary>
    /// <param name="index :">제거할 전이객체의 인덱스</param>
    void EraseTransitionToIndex(int index);

    /// <summary>
    /// FSM의 시작 상태를 설정합니다.
    /// </summary>
    void SetEntryStateToKey(std::string_view key);

public:
    /// <summary>
    /// FSM에서 사용할 State를 추가합니다.
    /// </summary>
    /// <typeparam name="T"></typeparam>
    template <FSM_STATE_BASE T>
    void AddState()
    {
        static_assert(std::is_base_of_v<FSMState, T>, "T is not derived from State.");
        const char* key = typeid(T).name();
        AddStateToKey(key);
    }

    /// <summary>
    /// FSM에 등록된 State를 가져옵니다.
    /// </summary>
    /// <typeparam name="T"></typeparam>
    template <FSM_STATE_BASE T>
    T* GetState()
    {
        static_assert(std::is_base_of_v<FSMState, T>, "T is not derived from State.");
        const char* key = typeid(T).name();
        return static_cast<T*>(GetStateToKey(key)); 
    }

    template <FSM_STATE_BASE T>
    bool RemoveState()
    {
        static_assert(std::is_base_of_v<FSMState, T>, "T is not derived from State.");
        const char* key = typeid(T).name();
        return RemoveStateToKey(key);
    }

private:
    /// <summary>
    /// State를 등록합니다.
    /// </summary>
    /// <param name="stateTypeIdName"></param>
    void AddStateToKey(std::string_view stateTypeIdName)
    {
        const char* key = stateTypeIdName.data();
        auto stateFind = _stateMap.find(key);
        if (stateFind == _stateMap.end())
        {
            FSMState* instance = MakeState(key);
            if (instance)
            {
                instance->_owner = this;
                _stateMap[key].reset(instance);
            }
            else
            {
                UmLogger.Log(LogLevel::LEVEL_WARNING, (const char*)u8"존재하지 않는 State 입니다..");
            }
        }
        else
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, (const char*)u8"이미 등록된 State 타입입니다.");
        }
    }

    FSMState* GetStateToKey(std::string_view key)
    {
        FSMState* result = nullptr;
        auto stateFind = _stateMap.find(key.data());
        if (stateFind != _stateMap.end())
        {
            result = stateFind->second.get();
        }
        return result;
    }

    bool RemoveStateToKey(std::string_view key) 
    { 
        size_t count = _stateMap.erase(key.data());
        return 0 > count;
    }

public:
    template <FSM_CONDITION_BASE T>
    void AddCondition()
    {
        static_assert(std::is_base_of_v<FSMCondition, T>, "T is not derived from Condition.");
        const char* key = typeid(T).name();
        AddConditionToKey(key);
    }

    template <FSM_CONDITION_BASE T>
    T* GetCondition()
    {
        static_assert(std::is_base_of_v<FSMCondition, T>, "T is not derived from Condition.");
        const char* key = typeid(T).name();
        return static_cast<T*>(GetConditionToKey(key));
    }

    template <FSM_CONDITION_BASE T>
    bool RemoveCondition()
    {
        static_assert(std::is_base_of_v<FSMCondition, T>, "T is not derived from Condition.");
        const char* key = typeid(T).name();
        return RemoveConditionToKey(key);
    }

private:
    /// <summary>
    /// Condition을 등록합니다.
    /// </summary>
    /// <param name="conditionTypeIdName"></param>
    void AddConditionToKey(std::string_view conditionTypeIdName)
    {
        const char* key = conditionTypeIdName.data();
        auto conditionFind = _conditionMap.find(key);
        if (conditionFind == _conditionMap.end())
        {
            FSMCondition* instance = MakeCondition(key);
            if (instance)
            {
                instance->_owner = this;
                _conditionMap[key].reset(instance);
            }
            else
            {
                UmLogger.Log(LogLevel::LEVEL_WARNING, (const char*)u8"존재하지 않는 Condidtion 입니다.");
            }
        }
        else
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, (const char*)u8"이미 등록된 Condition 타입입니다.");
        }
    }

    FSMCondition* GetConditionToKey(std::string_view key)
    {
        FSMCondition* result = nullptr;
        auto conditionFind = _conditionMap.find(key.data());
        if (conditionFind != _conditionMap.end())
        {
            result = conditionFind->second.get();
        }
        return result;
    }

    bool RemoveConditionToKey(std::string_view key)
    {
        size_t count = _conditionMap.erase(key.data());
        return 0 > count;
    }

public:
    bool IsValidEntryPoint();

private:
    std::map<std::string, std::unique_ptr<FSMState>> _stateMap;
    std::map<std::string, std::unique_ptr<FSMCondition>> _conditionMap;
    std::vector<Transition> _transitions;
    std::set<Transition, Transition> _transitionSet;

    FSMState* _currState; 
    FSMState* _nextState; 
    int       _nextOrder;

private:
    FSMState*     MakeState(std::string_view key);
    FSMCondition* MakeCondition(std::string_view key);

    void CheckTransitionCodition(Transition& transition);

private:
    void OnAwakeFSMEntities();
    void OnStartFSMEntities();

    void ChangeTransition();
    void UpdateTransition();

public:
    REFLECT_PROPERTY()

    FiniteStateMachine();
    ~FiniteStateMachine();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::unordered_map<std::string, std::string> StateReflectDatas;
    std::unordered_map<std::string, std::string> ConditionReflectDatas;
    std::vector<std::array<std::string, 3>>      TransitionReflectDatas;
    std::string EntryState = STR_NULL;
    REFLECT_FIELDS_END(FiniteStateMachine)

    /*
    직렬화 직전 자동으로 호출되는 이벤트 함수입니다.
    직접 override 해서 사용합니다.
    */
    virtual void SerializedReflectEvent();
    /*
    역직렬화 이후 자동으로 호출되는 이벤트 함수 입니다.
    직접 override 해서 사용합니다.
    */
    virtual void DeserializedReflectEvent();

    /*
    ImGuiDrawPropertys() 호출 이후 콜되는 이벤트 함수입니다.
    */
    virtual void ImGuiDrawPropertysEvent();

    /// <summary>
    /// <para> 이 함수는 ComponentFactory.AddComponentToObject() 직후 호출됩니다.              </para>
    /// <para> * 엔진 사용을 위한 초기화 이후 바로 호출됩니다.                                  </para>
    /// <para> 에디터 모드, 런타임 모드 상관 없이 게임 오브젝트에 추가하는 즉시 1회 호출됩니다.    </para>
    /// </summary>
    virtual void Reset() {};

    /// <summary>
    /// <para> 이 함수는 항상 Start 함수 전에 호출되며 프리팹이 인스턴스화 된 직후에 호출됩니다.                </para>
    /// <para> 게임 오브젝트의 Active가 false 상태인 경우 Awake 함수는 true가 될때까지 호출되지 않습니다.      </para>
    /// </summary>
    virtual void Awake();

    /// <summary>
    /// <para>  오브젝트가 활성화된 경우에만 호출됩니다. </para> <para>  컴포넌트의 Enable 활성화 직후 이 함수를
    /// 호출합니다 false 상태인 경우 true가 될때까지 호출되지 않습니다.  </para>
    /// </summary>
    virtual void OnEnable() {};

    /// <summary>
    /// <para>  오브젝트가 활성화된 경우에만 호출됩니다. </para> <para>  컴포넌트의 Disable 활성화 이후 이 함수를
    /// 호출합니다 true 상태인 경우 false가 될때까지 호출되지 않습니다.  </para>
    /// </summary>
    virtual void OnDisable() {};

    /// <summary>
    /// <para>  컴포넌트의 첫번째 Update 전에 한번 호출됩니다.   </para>
    /// </summary>
    virtual void Start();

    /// <summary>
    /// <para> FixedUpdate 는 종종 Update 보다 더 자주 호출됩니다. </para> <para> 프레임 속도가 낮은 경우 프레임당 여러
    /// 번 호출될 수 있으며 프레임 속도가 높은 경우 프레임 사이에 호출되지 않을 수 있습니다.  </para>
    /// </summary>
    virtual void FixedUpdate() {};

    /// <summary>
    /// Update 는 프레임당 한 번 호출됩니다.
    /// </summary>
    virtual void Update();

    /// <summary>
    /// LateUpdate 는 Update가 모두 끝난 후 호출됩니다.
    /// </summary>
    virtual void LateUpdate() {};

    /// <summary>
    /// <para> OnDestroy 는 모든 라이프 사이클 순회 이후 파괴 예정된 컴포넌트들이 파괴되기 직전에 호출됩니다. </para>
    /// </summary>
    virtual void OnDestroy() {};

    /// <summary>
    /// <para> OnApplicationQuit 은 Application::Quit이 호출되면 호출됩니다.
    /// </summary>
    virtual void OnApplicationQuit() {};
};
