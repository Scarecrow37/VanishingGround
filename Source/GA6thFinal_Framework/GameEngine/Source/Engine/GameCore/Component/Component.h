#pragma once

//참고 Unity Game Loop https://docs.unity3d.com/kr/2022.3/Manual/ExecutionOrder.html
class Component abstract :
    public ReflectSerializer
{
    friend class GameObject;
    friend class EComponentFactory;
    friend class ESceneManager;
    USING_PROPERTY(Component)
public:
    Component();
    virtual ~Component();

    /// <summary>
    /// 이 컴포넌트의 weak_ptr을 반환합니다.
    /// </summary>
    /// <returns>weak_ptr this</returns>
    std::weak_ptr<Component> GetWeakPtr() const
    {
        return _weakPtr;
    }

protected:
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

public:
    GETTER_ONLY(GameObject&, gameObject)
    {
        return *_gameObect;
    }
    //get : 이 컴포넌트가 부착된 게임 오브젝트입니다. 컴포넌트는 항상 게임 오브젝트에 부착됩니다.
    PROPERTY(gameObject);

    GETTER_ONLY(Transform&, transform)
    { 
        return _gameObect->transform_property_getter();
    }
    //get : 게임 오브젝트의 transform
    PROPERTY(transform)

    GETTER(bool, Enable)
    {
        return ReflectFields->_enable;
    }
    SETTER(bool, Enable)
    {
        ESceneManager::Engine::SetComponentEnable(this, value);
    }
    // get, set :
    //  컴포넌트의 활성화 여부입니다.
    PROPERTY(Enable);

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
    const char* ClassName()
    {
        return _className.c_str();
    }

    /// <summary>
    /// 이 컴포넌트의 인덱스를 반환합니다. (이 컴포넌트가 추가된 오브젝트에서의 기준)
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
    std::string _className;

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
    GameObject* _gameObect;
    std::weak_ptr<Component> _weakPtr;
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
