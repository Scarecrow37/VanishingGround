#pragma once
#include "Factory/FSMStateFactory.h"
#include "Factory/FSMConditionFactory.h"

class FiniteStateMachine : public Component
{
    USING_PROPERTY(FiniteStateMachine)
private:
    static const char* AddStateImguiPopUp();
    void ImguiDrawStates();

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
        return GetStateWithKey(key); 
    }

    template<typename T>
    bool RemoveState()
    {
        static_assert(std::is_base_of_v<FSMState, T>, "T is not derived from State.");
        const char* key = typeid(T).name();
        RemoveStateWithKey(key);
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
            FSMState* instance = FSMStateFactory::NewInstanceWithKey(key);
            if (instance)
            {
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

private:
    std::map<std::string, std::unique_ptr<FSMState>> _stateMap;

public:
    REFLECT_PROPERTY()

    FiniteStateMachine();
    ~FiniteStateMachine();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::unordered_map<std::string, std::string>StateReflectDatas;
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
    virtual void Awake() {};

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
    virtual void Start() {};

    /// <summary>
    /// <para> FixedUpdate 는 종종 Update 보다 더 자주 호출됩니다. </para> <para> 프레임 속도가 낮은 경우 프레임당 여러
    /// 번 호출될 수 있으며 프레임 속도가 높은 경우 프레임 사이에 호출되지 않을 수 있습니다.  </para>
    /// </summary>
    virtual void FixedUpdate() {};

    /// <summary>
    /// Update 는 프레임당 한 번 호출됩니다.
    /// </summary>
    virtual void Update() {};

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
