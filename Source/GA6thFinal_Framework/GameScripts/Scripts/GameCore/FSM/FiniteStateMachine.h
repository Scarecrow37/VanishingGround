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
    /// <param name="state :">대상 상태</param>
    /// <param name="condition :">전이 조건</param>
    /// <param name="nextState :">변경될 상태</param>
    void AddTransition(std::string_view state, std::string_view condition, std::string_view nextState);

    /// <summary>
    /// 모든 상황에서 전이가 가능한 전이 객체를 추가합니다.
    /// </summary>
    /// <param name="condition :">전이 조건</param>
    /// <param name="nextState :">변경될 상태</param>
    void AddAnyTransition(std::string_view condition, std::string_view nextState);

    /// <summary>
    /// 전이 객체를 제거합니다.
    /// </summary>
    /// <param name="index :">제거할 전이객체의 인덱스</param>
    void EraseTransition(int index);

    /// <summary>
    /// FSM의 시작 상태를 설정합니다.
    /// </summary>
    /// <param name="index :">사용할 시작점의 인덱스</param>
    void SetEntryTransition(int index);

public:
    /// <summary>
    /// FSM에서 사용할 State를 추가합니다.
    /// </summary>
    /// <typeparam name="T"></typeparam>
    template<typename T>
    void AddState()
    {
        static_assert(std::is_base_of_v<FSMState, T>, "T is not derived from State.");
        const char* key = typeid(T).name();
        AddStateWithKey(key);
    }

    /// <summary>
    /// FSM에 등록된 State를 가져옵니다.
    /// </summary>
    /// <typeparam name="T"></typeparam>
    template<typename T>
    T* GetState()
    {
        static_assert(std::is_base_of_v<FSMState, T>, "T is not derived from State.");
        const char* key = typeid(T).name();
        return static_cast<T*>(GetStateWithKey(key)); 
    }

    template<typename T>
    bool RemoveState()
    {
        static_assert(std::is_base_of_v<FSMState, T>, "T is not derived from State.");
        const char* key = typeid(T).name();
        return RemoveStateWithKey(key);
    }

private:
    /// <summary>
    /// State를 등록합니다.
    /// </summary>
    /// <param name="stateTypeIdName"></param>
    void AddStateWithKey(std::string_view stateTypeIdName)
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

    FSMState* GetStateWithKey(std::string_view key)
    {
        auto stateFind = _stateMap.find(key.data());
        if (stateFind != _stateMap.end())
        {
            return stateFind->second.get();
        }
    }

    bool RemoveStateWithKey(std::string_view key) 
    { 
        size_t count = _stateMap.erase(key.data());
        return 0 > count;
    }

public:
    template <typename T>
    void AddCondition()
    {
        static_assert(std::is_base_of_v<FSMCondition, T>, "T is not derived from Condition.");
        const char* key = typeid(T).name();
        AddConditionWithKey(key);
    }

    template <typename T>
    T* GetCondition()
    {
        static_assert(std::is_base_of_v<FSMCondition, T>, "T is not derived from Condition.");
        const char* key = typeid(T).name();
        return static_cast<T*>(GetConditionWithKey(key));
    }

    template <typename T>
    bool RemoveCondition()
    {
        static_assert(std::is_base_of_v<FSMCondition, T>, "T is not derived from Condition.");
        const char* key = typeid(T).name();
        return RemoveConditionWithKey(key);
    }

private:
    /// <summary>
    /// Condition을 등록합니다.
    /// </summary>
    /// <param name="conditionTypeIdName"></param>
    void AddConditionWithKey(std::string_view conditionTypeIdName)
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

    FSMCondition* GetConditionWithKey(std::string_view key)
    {
        auto conditionFind = _conditionMap.find(key.data());
        if (conditionFind != _conditionMap.end())
        {
            return conditionFind->second.get();
        }
    }

    bool RemoveConditionWithKey(std::string_view key)
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
    int EntryTransitionID = -1;
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
