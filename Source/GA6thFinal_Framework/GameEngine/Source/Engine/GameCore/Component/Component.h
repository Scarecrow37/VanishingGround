#pragma once

//참고 Unity Game Loop https://docs.unity3d.com/kr/2022.3/Manual/ExecutionOrder.html
class Component abstract :
    public ReflectSerializer,
    public ITimeInvoker
{
    inline static GameObject staticDummyObject;
    friend class GameObject;
    friend class EComponentFactory;
    friend class ESceneManager;
    USING_PROPERTY(Component)

public:
    enum class TYPE
    {
        // 일반
        GENERIC,    
        // 렌더러
        MESH,
        // 조명
        LIGHT,
        // 카메라
        CAMERA,
        // UI
        UI,
    };

    /// <summary>
    /// 생성시 타입 플래그를 지정해줘야합니다.
    /// </summary>
    /// <param name="isMeshComponent"></param>
    Component(TYPE type = TYPE::GENERIC);
    virtual ~Component();

    /// <summary>
    /// 이 컴포넌트의 weak_ptr을 반환합니다.
    /// </summary>
    /// <returns>weak_ptr this</returns>
    std::weak_ptr<Component> GetWeakPtr() const
    {
        return _weakPtr;
    }

    // ITimeInvoker을(를) 통해 상속됨
    virtual std::weak_ptr<ITimeInvoker> GetWeakInvoker() override;

protected:
    /// <summary>
    /// <para> 이 함수는 ComponentFactory.AddComponentToObject() 직후 호출됩니다.              </para>
    /// <para> * 엔진 사용을 위한 초기화 이후 바로 호출됩니다.                                  </para>
    /// <para> 에디터 모드, 런타임 모드 상관 없이 게임 오브젝트에 추가하는 즉시 1회 호출됩니다.    </para>
    /// </summary>
    virtual void Reset() {};

    /// <summary>
    /// 라이프 사이클에 포함될때 호출되는 함수입니다.
    /// Enable 여부와 상관없이 1회 호출됩니다. 에디터 모드, 런타임 모드 상관 없이 호출됩니다.
    /// </summary>
    virtual void Added() {};

    /// <summary>
    /// <para> 이 함수는 항상 Start 함수 전에 호출되며 프리팹이 인스턴스화 된 직후에 호출됩니다.                </para>
    /// <para> 게임 오브젝트의 Active가 false 상태인 경우 Awake 함수는 true가 될때까지 호출되지 않습니다.      </para>
    /// </summary>
    virtual void Awake() {};

    /// <summary>
    /// <para>  오브젝트가 활성화된 경우에만 호출됩니다.                                                              </para>    
    /// <para>  컴포넌트의 Enable 활성화 직후 이 함수를 호출합니다 false 상태인 경우 true가 될때까지 호출되지 않습니다.  </para>
    /// </summary>
    virtual void OnEnable() {};

    /// <summary>
    /// <para>  오브젝트가 활성화된 경우에만 호출됩니다.                                                              </para>    
    /// <para>  컴포넌트의 Disable 활성화 이후 이 함수를 호출합니다 true 상태인 경우 false가 될때까지 호출되지 않습니다.  </para>
    /// </summary>
    virtual void OnDisable() {};

    /// <summary>
    /// <para>  컴포넌트의 첫번째 Update 전에 한번 호출됩니다.   </para>
    /// </summary>
    virtual void Start() {};

    /// <summary>
    /// <para> FixedUpdate 는 종종 Update 보다 더 자주 호출됩니다.                                                                  </para>
    /// <para> 프레임 속도가 낮은 경우 프레임당 여러 번 호출될 수 있으며 프레임 속도가 높은 경우 프레임 사이에 호출되지 않을 수 있습니다.  </para> 
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

    /// <summary>
    /// Scene Manager의 Load Scene이 요청되면 호출됩니다. (Active 여부와 무관합니다)
    /// </summary>
    virtual void OnLoadScene(Scene& loadScene, LoadSceneMode mode) {};

    /// <summary>
    /// <para> 직렬화 직전 자동으로 호출되는 이벤트 함수입니다.  </para>
    /// <para> 직접 override 해서 사용합니다.                  </para>
    /// <para> 부모의 가상함수를 직접 호출하지 않아도 호출됩니다.</para>
    /// </summary>
    virtual void SerializedReflectEvent() {};

    /// <summary>
    /// <para> 역직렬화 이후 자동으로 호출되는 이벤트 함수 입니다.  </para>
    /// <para> 직접 override 해서 사용합니다.                     </para>
    /// /// <para> 부모의 가상함수를 직접 호출하지 않아도 호출됩니다.</para>
    /// </summary>
    virtual void DeserializedReflectEvent() {};

public:
    /// <summary>
    /// <para> 에디터 Scene View에 DrawDebug를 그리기 위한 함수입니다. </para>
    /// <para> 에디터 에서만 호출됩니다.                               </para>
    /// </summary>
    virtual void OnDrawDebug() {};

    /// <summary>
    /// <para> 에디터 Scene View에 DrawDebug를 그리기 위한 함수입니다. </para>
    /// <para> 컴포넌트가 Inspector에 선택되었을때만 호출됩니다. </para>
    /// <para> 에디터 에서만 호출됩니다. </para>
    /// </summary>
    virtual void OnDrawDebugSelected() {};

public:
    GETTER_ONLY(GameObject&, gameObject)
    {
        return *_gameObject;
    }
    
    // type : GameObject&
    // get : 이 컴포넌트가 부착된 게임 오브젝트입니다. 컴포넌트는 항상 게임 오브젝트에 부착됩니다.     
    PROPERTY(gameObject);

    GETTER_ONLY(Transform&, transform)
    { 
        return _gameObject->transform_property_getter();
    }
    // type : Transform&
    // get : 게임 오브젝트의 transform입니다. 모든 오브젝트는 Transform을 가지고 있습니다.
    PROPERTY(transform)

    GETTER(bool, Enable)
    {
        return ReflectFields->_enable;
    }
    SETTER(bool, Enable)
    {
        ESceneManager::Engine::SetComponentEnable(this, value);
    }
    // type : bool
    // get, set : 컴포넌트의 활성화 여부입니다.
    PROPERTY(Enable);

    GETTER_ONLY(const bool&, EnableInHierarchy)
    {
        return _enableInHierarchy;
    }
    // type : const bool&
    // get : 컴포넌트의 하이러키 기준 활성화 여부입니다.
    PROPERTY(EnableInHierarchy);

    REFLECT_PROPERTY(
        Enable
    )

protected:
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    bool _enable = true;
    REFLECT_FIELDS_END(Component)

public:
    /// <summary>
    /// 이 컴포넌트의 실제 클래스 이름입니다.
    /// </summary>
    /// <returns>컴포넌트 클래스 실제 이름</returns>
    const char* ClassName() const
    {
        return _className.c_str();
    }

    /// <summary>
    /// 이 컴포넌트의 타입입니다.
    /// </summary>
    /// <returns>컴포넌트의 타입</returns>
    Component::TYPE GetType() const
    {
        return _type;
    }

    /// <summary>
    /// 이 컴포넌트가 추가된 오브젝트에서의 인덱스를 반환합니다.
    /// </summary>
    /// <returns>int 인덱스</returns>
    int GetIndex() const;

    /// <summary>
    /// 컴포넌트를 추가합니다.
    /// </summary>
    /// <typeparam name="TComponent :">추가할 컴포넌트 타입</typeparam>
    template <IS_BASE_COMPONENT_C TComponent>
    inline TComponent& AddComponent();

    /// <summary>
    /// <para> TComponent 타입의 컴포넌트를 찾아서 반환합니다. </para>
    /// <para> 실패시 nullptr를 반환합니다.                     </para>
    /// </summary>
    /// <typeparam name="TComponent :">검색할 컴포넌트 타입</typeparam>
    /// <returns>해당 타입 컴포넌트의 ptr</returns>
    template <IS_BASE_COMPONENT_C TComponent>
    inline TComponent* GetComponent() const;

    /// <summary>
    /// 전달받은 인덱스의 컴포넌트를 TComponent 타입으로 dynamic_cast를 시도해
    /// 반환합니다.
    /// </summary>
    /// <typeparam name="TComponent :">캐스팅할 컴포넌트 타입</typeparam>
    /// <param name="index :">컴포넌트 인덱스</param>
    /// <returns>해당 타입 컴포넌트의 ptr</returns>
    template <IS_BASE_COMPONENT_C TComponent>
    inline TComponent* GetComponentAtIndex(size_t index) const;

    /// <summary>
    /// <para> TComponent 타입의 컴포넌트를 전부 찾아서 반환합니다. </para>
    /// <para> 실패시 empty를 반환합니다.                         </para>
    /// </summary>
    /// <typeparam name="TComponent"></typeparam>
    /// <returns>찾은 모든 컴포넌트에 대한 배열</returns>
    template <IS_BASE_COMPONENT_C TComponent>
    inline std::vector<TComponent*> GetComponents() const;

    /// <summary>
    /// 이 오브젝트에 부착된 컴포넌트 개수를 반환합니다.
    /// </summary>
    /// <returns>이 오브젝트에 부착된 컴포넌트 개수.</returns>
    inline size_t GetComponentCount() const;

private:
    /*초기화 함수 호출 여부를 관리하기 위한 내부 구조체입니다.*/
    struct InitFlags
    {
        InitFlags();
        ~InitFlags();
        inline bool IsAwake() const
        {
            return _isAwake;
        }
        inline void SetAwake()
        {
            _isAwake = true;
        }

        inline bool IsStart() const
        {
            return _isStart;
        }
        inline void SetStart()
        {
            _isStart = true;
        }
    private:
        bool _isAwake;
        bool _isStart;
    };
    InitFlags _initFlags;

    const TYPE _type;
    std::string _className;
    GameObject* _gameObject;
    std::weak_ptr<Component> _weakPtr;
    bool _enableInHierarchy;
    bool _prevFrameEnableInHierarchy;

private:
    /// <summary>
    /// 프리팹용 OverrideFlag들을 해제합니다. 에디터 모드에서만 동작합니다.
    /// </summary>
    inline void UnsetOverrideFlags();

    /// <summary>
    /// _enableInHierarchy을 갱신합니다.
    /// </summary>
    void UpdateEnableInHierarchy();

};

template <IS_BASE_COMPONENT_C TComponent>
inline TComponent& Component::AddComponent()
{
    return gameObject->AddComponent<TComponent>();
}

template <IS_BASE_COMPONENT_C TComponent>
inline TComponent* Component::GetComponent() const
{
    return gameObject->GetComponent<TComponent>();
}

template <IS_BASE_COMPONENT_C TComponent>
inline TComponent* Component::GetComponentAtIndex(size_t index) const
{
    return gameObject->GetComponentAtIndex<TComponent>(index);
}

template <IS_BASE_COMPONENT_C TComponent>
inline std::vector<TComponent*> Component::GetComponents() const
{
    return gameObject->GetComponents<TComponent>();
}

inline size_t Component::GetComponentCount() const
{
    GameObject& object = gameObject;
    return object.GetComponentCount();
}

inline void Component::UnsetOverrideFlags() 
{
    if constexpr (Application::IsEditor())
    {
        applyReflectFields([&](std::string_view name, void* pData) 
        {
            UmGameObjectFactory.UnsetOverrideFlag(pData);
        });
    }
}



