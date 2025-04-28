#pragma once
class Component;

//참고
//Unity GameObject https://docs.unity3d.com/kr/2021.1/Manual/class-GameObject.html
//Unity GameObject Script https://docs.unity3d.com/6000.0/Documentation/ScriptReference/GameObject.html

//오브젝트 생성용 전역함수
template<IS_BASE_GAMEOBJECT_C TObject>
std::shared_ptr<TObject> NewGameObject(
    std::string_view name)
{
    std::shared_ptr<GameObject> shared_object = Global::engineCore->GameObjectFactory.NewGameObject(typeid(TObject).name(), name);
    return std::static_pointer_cast<TObject>(shared_object);
}

//함수는 일단 선언만. 구현은 나중에. 
class GameObject : 
    public ReflectSerializer,
    public IEditorObject
{
    friend class EGameObjectFactory;
    friend class EComponentFactory;
    friend class ESceneManager;
    USING_PROPERTY(GameObject)

    //public static 함수
public:
    struct Helper
    {
        /// <summary>
        /// 고유한 이름을 만들어줍니다. baseName (i) 형식으로 만듭니다.
        /// </summary>
        /// <param name="baseName"></param>
        /// <returns>생성된 이름</returns>
        static std::string GenerateUniqueName(std::string_view baseName);
    };

    /// <summary>
    /// <para> 매개변수와 같은 이름을 가진 GameObject를 찾아 반환합니다. </para>
    /// <para> 같은 이름의 GameObject가 없으면 nullptr를 반환합니다.    </para>
    /// <para> 참고 : 같은 이름의 오브젝트가 여러개 있으면 특정 오브젝트 반환을 보장하지 못합니다.  </para>
    /// </summary>
    /// <param name="name :">검색할 오브젝트의 이름</param>
    /// <returns>찾은 오브젝트를 weak_ptr에 담아준다.</returns>
    static std::weak_ptr<GameObject> Find(std::string_view name) 
    {  
        return ESceneManager::Engine::FindGameObjectWithName(name);
    }

    /// <summary>
    /// <para> 매개변수와 같은 이름을 가진 GameObject를 찾아 전부 반환합니다. </para>
    /// <para> 같은 이름의 GameObject가 없으면 nullptr를 반환합니다.         </para>
    /// </summary>
    /// <param name="name :">검색할 오브젝트의 이름</param>
    /// <returns>찾은 오브젝트를 weak_ptr에 담아준다.</returns>
    static std::vector<std::weak_ptr<GameObject>> FindGameObjects(std::string_view name)
    {
        return ESceneManager::Engine::FindGameObjectsWithName(name);
    }

    /// <summary>
    /// <para> 구현 X                                                      </para>
    /// <para> 매개변수와 같은 태그가 설정된 GameObject들의 배열을 반환합니다. </para>
    /// <para> 태그가 있는 GameObject가 없으면 빈 배열을 반환합니다.          </para>
    /// </summary>
    /// <param name="tag :">검색할 태그</param>
    /// <returns>찾은 오브젝트들을 담은 weak_ptr배열</returns>
    static std::vector<std::weak_ptr<GameObject>> FindGameObjectsWithTag(std::wstring_view tag) { return std::vector<std::weak_ptr<GameObject>>(); }

    /// <summary>
    /// <para> 구현 X                                                                               </para>
    /// <para> 매개변수와 같은 태그가 설정된 GameObject를 찾아 반환합니다.                             </para>
    /// <para> 참고 : 같은 태그가 설정된 오브젝트가 여러개 있으면 특정 오브젝트 반환을 보장하지 못합니다. </para>
    /// </summary>
    /// <param name="tag :">검색할 태그</param>
    /// <returns>찾은 오브젝트를 weak_ptr에 담아준다.</returns>
    static std::weak_ptr<GameObject> FindWithTag(std::wstring_view tag) { return std::weak_ptr<GameObject>(); }

    /// <summary>
    /// <para>전달받은 오브젝트 or 컴포넌트를 파괴합니다. </para>
    /// </summary>
    /// <param name="Object :">파괴할 오브젝트</param>
    /// <param name="t :">딜레이 시간</param>
    static void Destroy(Component& component, float t = 0.f);
    /// <summary>
    /// <para>전달받은 오브젝트 or 컴포넌트를 파괴합니다. </para>
    /// </summary>
    /// <param name="Object :">파괴할 오브젝트</param>
    /// <param name="t :">딜레이 시간</param>
    static void Destroy(Component* component, float t = 0.f)
    {
        Destroy(*component, t);
    }
    /// <summary>
    /// <para>전달받은 오브젝트 or 컴포넌트를 파괴합니다. </para>
    /// </summary>
    /// <param name="Object :">파괴할 오브젝트</param>
    /// <param name="t :">딜레이 시간</param>
    static void Destroy(GameObject& gameObject, float t = 0.f);
    /// <summary>
    /// <para>전달받은 오브젝트 or 컴포넌트를 파괴합니다. </para>
    /// </summary>
    /// <param name="Object :">파괴할 오브젝트</param>
    /// <param name="t :">딜레이 시간</param>
    static void Destroy(GameObject* gameObject, float t = 0.f)
    {
        Destroy(*gameObject, t);
    }

    /// <summary>                                                           </para>
    /// <para>전달받은 오브젝트가 다른 Scene을 로드 할 때 파괴되지 않도록 합니다.</para>
    /// </summary>
    /// <param name="Object :">대상 오브젝트</param>
    static void DontDestroyOnLoad(GameObject& gameObject);
    /// <summary>                                                           </para>
    /// <para>전달받은 오브젝트가 다른 Scene을 로드 할 때 파괴하지 않도록 합니다.</para>
    /// </summary>
    /// <param name="Object :">대상 오브젝트</param>
    static void DontDestroyOnLoad(GameObject* gameObject)
    {
        DontDestroyOnLoad(*gameObject);
    }

    /// <summary>
    /// <para> 대상 오브젝트의 복사본을 현재 씬에 생성합니다.  </para>
    /// </summary>
    /// <param name="gameObject :">복사할 오브젝트</param>
    static void Instantiate(GameObject& original);
    /// <summary>
    /// <para> 대상 오브젝트의 복사본을 현재 씬에 생성합니다.  </para>
    /// </summary>
    /// <param name="gameObject :">복사할 오브젝트</param>
    static void Instantiate(GameObject* original)
    {
        Instantiate(*original);
    }
   
public:
    GameObject();
    ~GameObject();

public:
    /// <summary>
    /// 이 게임오브젝트의 weak_ptr을 반환합니다.
    /// </summary>
    /// <returns>weak_ptr this</returns>
    std::weak_ptr<GameObject> GetWeakPtr() const;

    /// <summary>
    /// <para> 전달받은 GameObject가 속해있는 Scene을 반환합니다. </para>
    /// </summary>
    /// <returns>Scene 정보</returns>
    Scene& GetScene();

    /// <summary>
    /// <para>이 GameObject의 InstanceID를 반환합니다.                                 </para>
    /// <para>참고 : InstanceID는 매 런타임마다 달라집니다. 즉 UUID로 사용할 수 없습니다. </para>
    /// </summary>
    /// <returns>int InstanceID</returns>
    int GetInstanceID() const { return _instanceID; }

    /// <summary>
    /// 이 GameObject의 ActiveSelf 여부를 변경합니다.
    /// </summary>
    void SetActive(bool value)
    {
        ESceneManager::Engine::SetGameObjectActive(_instanceID, value);
    }

    /// <summary>
    /// <para> 이 GameObject의 이름을 반환합니다. </para>
    /// </summary>
    /// <returns>std::string_view 오브젝트의 이름</returns>
    std::string_view ToString() { return ReflectFields->_name; }

    /// <summary>
    /// 컴포넌트를 추가합니다.
    /// </summary>
    /// <typeparam name="TComponent :">추가할 컴포넌트 타입</typeparam>
    template<IS_BASE_COMPONENT_C TComponent>
    inline TComponent& AddComponent();

    /// <summary>
    /// <para> TComponent 타입의 컴포넌트를 찾아서 반환합니다. </para>
    /// <para> 실패시 nullptr를 반환합니다.                     </para>
    /// </summary>
    /// <typeparam name="TComponent :">검색할 컴포넌트 타입</typeparam>
    /// <returns>해당 타입 컴포넌트의 ptr</returns>
    template<IS_BASE_COMPONENT_C TComponent>
    inline TComponent* GetComponent() const;

    /// <summary>
    /// 전달받은 인덱스의 컴포넌트를 TComponent 타입으로 dynamic_cast를 시도해 반환합니다.
    /// </summary>
    /// <typeparam name="TComponent :">캐스팅할 컴포넌트 타입</typeparam>
    /// <param name="index :">컴포넌트 인덱스</param>
    /// <returns>해당 타입 컴포넌트의 ptr</returns>
    template<IS_BASE_COMPONENT_C TComponent>
    inline TComponent* GetComponentAtIndex(size_t index) const;

    /// <summary>
    /// <para> TComponent 타입의 컴포넌트를 전부 찾아서 반환합니다. </para>
    /// <para> 실패시 empty를 반환합니다.                         </para>
    /// </summary>
    /// <typeparam name="TComponent"></typeparam>
    /// <returns>찾은 모든 컴포넌트에 대한 배열</returns>
    template<IS_BASE_COMPONENT_C TComponent>
    inline std::vector<TComponent*> GetComponents() const;

    /// <summary>
    /// 이 오브젝트에 부착된 컴포넌트 개수를 반환합니다.
    /// </summary>
    /// <returns>이 오브젝트에 부착된 컴포넌트 개수.</returns>
    inline size_t GetComponentCount() const { return _components.size(); }

    /// <summary>
    /// <para> 전달받은 컴포넌트가 이 오브젝트에 존재하면 인덱스를 반환합니다.</para>
    /// <para> 실패시 -1을 반환합니다.                         </para>
    /// </summary>
    inline int GetComponentIndex(const Component* pComponent) const;


    //IEditorObject에서 상속
 private:
    /* InspectorView에 SetFocus 될 때 호출 구현 X */
    virtual void OnInspectorViewEnter();
    /* InspectorView의 Draw단계에 호출 */
    virtual void OnInspectorStay();


//프로퍼티
public:
    GETTER_ONLY(bool, ActiveInHierarchy)
    {
        Transform* curr = &_transform;
        while (curr != nullptr)
        {
            if (!curr->gameObject->ReflectFields->_activeSelf)
                return false;

            curr = curr->Parent;
        }
        return true;
    }
    // get : 실제 활성화 여부 (부모가 false면 false)
    PROPERTY(ActiveInHierarchy);

    GETTER_ONLY(Transform&, transform)
    { 
        return _transform;
    }
    //get : object Transform
    PROPERTY(transform)
    
    SETTER(bool, ActiveSelf)
    {
        SetActive(value);
    }
    GETTER(bool, ActiveSelf)
    {
        return ReflectFields->_activeSelf;
    }
    // get, set :
    //  자신의 local active 여부 (실제 활성화 여부)
    PROPERTY(ActiveSelf);
   
    GETTER(bool, IsStatic)
    {
        return ReflectFields->_isStatic;
    }
    SETTER(bool, IsStatic)
    {
        ReflectFields->_isStatic = value;
    }
    // get, set :
    //  게임 오브젝트에 대해 IsStatic 플래그가 설정되어 있는지 여부.
    PROPERTY(IsStatic);
    
    GETTER(std::string_view, Name)
    {
        return ReflectFields->_name;
    }
    SETTER(std::string_view, Name)
    {
        ESceneManager::Engine::RenameGameObject(this, value);
    }
    // get, set:
    //  게임 오브젝트의 이름
    PROPERTY(Name)

    GETTER_ONLY(bool, IsPrefabInstacne) 
    { 
        return _prefabGuid != STR_NULL; 
    }
    //get : 프리팹으로 인스턴스화된 게임오브젝트 여부를 반환합니다.
    PROPERTY(IsPrefabInstacne)

    GETTER_ONLY(std::string, PrefabPath) 
    { 
        return _prefabGuid.ToPath().string();
    }
    //이 오브젝트가 참조하고있는 프리팹을 반환합니다.
    PROPERTY(PrefabPath)

    //에디터 편집을 허용할 프로퍼티.
    REFLECT_PROPERTY(
        Name,
        ActiveSelf,
        IsStatic
    )
private:
    Transform _transform;
protected:
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    std::string                              _name = STR_NULL;
    bool                                     _activeSelf = true;
    bool                                     _isStatic = false;
    REFLECT_FIELDS_END(GameObject)

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

private:
    std::weak_ptr<GameObject>                _weakPtr;
    std::string                              _ownerScene;
    File::Guid                               _prefabGuid;
    std::vector<std::shared_ptr<Component>>  _components;
    int                                      _instanceID;

public:
    //activeInHierarchy와 같음.
    operator bool() { return ActiveInHierarchy; }
    bool operator != (const GameObject& rhs)
    {
        return this != &rhs;
    }
    bool operator == (const GameObject& rhs)
    {
        return this == &rhs;
    }
};

#include "Engine/EngineCore/EngineCores.h"
template<IS_BASE_COMPONENT_C TComponent >
inline TComponent& GameObject::AddComponent()
{
    EComponentFactory& factory = Global::engineCore->ComponentFactory;
    Component* component = factory.AddComponentToObject(this, typeid(TComponent).name());
    if (component)
    {
        return static_cast<TComponent&>(*component);
    }
    else
    {
        //컴포넌트 생성 실패
        __debugbreak();
    } 
}

template<IS_BASE_COMPONENT_C TComponent>
inline TComponent* GameObject::GetComponent() const
{
    TComponent* result = nullptr;
    for (auto& component : _components)
    {
        if (typeid(TComponent) == typeid(*component))
        {
            result = static_cast<TComponent*>(component.get());
            break;
        }
    }
    return result;
}

template<IS_BASE_COMPONENT_C TComponent>
inline TComponent* GameObject::GetComponentAtIndex(size_t index) const
{
    TComponent* result = nullptr;
    if (index >= _components.size())
    {
        return result;
    }
    else
    {
        result = static_cast<TComponent*>(_components[index].get());
        return result;
    }
}

template<IS_BASE_COMPONENT_C TComponent>
inline std::vector<TComponent*> GameObject::GetComponents() const
{
    std::vector<TComponent*> result;
    for (auto& component : _components)
    {
        if (typeid(TComponent) == typeid(*component))
        {
            result.emplace_back(static_cast<TComponent*>(component));
        }
    }
    return result;
}

inline int GameObject::GetComponentIndex(const Component* pComponent) const
{
    for (int i = 0; i < _components.size(); ++i)
    {
        if (_components[i].get() == pComponent)
        {
            return i;
        }
    }
    return -1;
}