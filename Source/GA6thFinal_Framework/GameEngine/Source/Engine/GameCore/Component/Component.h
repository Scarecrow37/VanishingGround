#pragma once

//참고 Unity Game Loop https://docs.unity3d.com/kr/2022.3/Manual/ExecutionOrder.html
class Component :
    public ReflectSerializer
{
    friend class GameObject;
    friend class EComponentFactory;
    friend class ESceneManager;
    USING_PROPERTY(Component)
public:
    Component();
    virtual ~Component();

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
    bool        _enable = true;
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
    int GetComponentIndex() const { return _index; }

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
    int         _index;
};