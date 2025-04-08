#pragma once
class Component;

//참고
//Unity GameObject https://docs.unity3d.com/kr/2021.1/Manual/class-GameObject.html
//Unity GameObject Script https://docs.unity3d.com/6000.0/Documentation/ScriptReference/GameObject.html

//오브젝트 생성용 전역함수
template<IS_BASE_GAMEOBJECT_C TObject>
std::weak_ptr<TObject> NewGameObject(
    std::string_view name)
{
    std::shared_ptr<GameObject> shared_object = Global::engineCore->GameObjectFactory.NewGameObject(typeid(TObject).name(), name);
    return std::static_pointer_cast<TObject>(shared_object);
}

//함수는 일단 선언만. 구현은 나중에. 
class GameObject : 
    public ReflectSerializer
{
    friend class EGameObjectFactory;
    friend class EComponentFactory;
    friend class ESceneManager;
    USING_PROPERTY(GameObject)

    //public static 함수
public:
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

    /// <summary>
    /// <para>구현 X                                                         </para>
    /// <para>전달받은 오브젝트가 다른 Scene을 로드 할 때 파괴되지 않도록 합니다.</para>
    /// </summary>
    /// <param name="Object :">대상 오브젝트</param>
    static void DontDestroyOnLoad(GameObject& gameObject);
    /// <summary>
    /// <para>구현 X                                                         </para>
    /// <para>전달받은 오브젝트가 다른 Scene을 로드 할 때 파괴하지 않도록 합니다.</para>
    /// </summary>
    /// <param name="Object :">대상 오브젝트</param>
    static void DontDestroyOnLoad(GameObject* gameObject)
    {
        DontDestroyOnLoad(*gameObject);
    }

    /// <summary>
    /// <para> 구현 X                                      </para>
    /// <para> 대상 오브젝트의 복사본을 현재 씬에 생성합니다.  </para>
    /// </summary>
    /// <param name="gameObject :">복사할 오브젝트</param>
    static void Instantiate(GameObject& original);
    /// <summary>
    /// <para> 구현 X                                      </para>
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
    std::string_view ToString() { return ReflectionFields->_name; }

    /// <summary>
    /// 컴포넌트를 추가합니다.
    /// </summary>
    /// <typeparam name="TComponent :">추가할 컴포넌트 타입</typeparam>
    template<IS_BASE_COMPONENT_C TComponent>
    inline TComponent& AddComponent();

    /// <summary>
    /// <para> TComponent 타입의 컴포넌트를 찾아서 반환합니다. </para>
    /// <para> 실패시 empty를 반환합니다.                     </para>
    /// </summary>
    /// <typeparam name="TComponent :">검색할 컴포넌트 타입</typeparam>
    /// <returns>해당 타입 컴포넌트의 weak_ptr</returns>
    template<IS_BASE_COMPONENT_C TComponent>
    inline std::weak_ptr<TComponent> GetComponent();

    /// <summary>
    /// 전달받은 인덱스의 컴포넌트를 TComponent 타입으로 dynamic_cast를 시도해 반환합니다.
    /// </summary>
    /// <typeparam name="TComponent :">캐스팅할 컴포넌트 타입</typeparam>
    /// <param name="index :">컴포넌트 인덱스</param>
    /// <returns>해당 타입 컴포넌트의 weak_ptr</returns>
    template<IS_BASE_COMPONENT_C TComponent>
    inline std::weak_ptr<TComponent> GetComponentAtIndex(size_t index);

    /// <summary>
    /// <para> TComponent 타입의 컴포넌트를 전부 찾아서 반환합니다. </para>
    /// <para> 실패시 empty를 반환합니다.                         </para>
    /// </summary>
    /// <typeparam name="TComponent"></typeparam>
    /// <returns>찾은 모든 컴포넌트에 대한 배열</returns>
    template<IS_BASE_COMPONENT_C TComponent>
    inline std::vector<std::weak_ptr<TComponent>> GetComponents();

    /// <summary>
    /// 이 오브젝트에 부착된 컴포넌트 개수를 반환합니다.
    /// </summary>
    /// <returns>이 오브젝트에 부착된 컴포넌트 개수.</returns>
    inline size_t GetComponentCount() { return _components.size(); }


//프로퍼티
public:
    GETTER_ONLY(bool, ActiveInHierarchy)
    {
        Transform* curr = &transform;
        while (curr != nullptr)
        {
            if (!curr->gameObject.ReflectionFields->_activeSelf)
                return false;

            curr = curr->Parent;
        }
        return true;
    }
    // get : 실제 활성화 여부 (부모가 false면 false)
    PROPERTY(ActiveInHierarchy);
    
    SETTER(bool, ActiveSelf)
    {
        SetActive(value);
    }
    GETTER(bool, ActiveSelf)
    {
        return ReflectionFields->_activeSelf;
    }
    // get, set :
    //  자신의 local active 여부 (실제 활성화 여부)
    PROPERTY(ActiveSelf);
   
    GETTER(bool, IsStatic)
    {
        return ReflectionFields->_isStatic;
    }
    SETTER(bool, IsStatic)
    {
        ReflectionFields->_isStatic = value;
    }
    // get, set :
    //  게임 오브젝트에 대해 IsStatic 플래그가 설정되어 있는지 여부.
    PROPERTY(IsStatic);
    

    GETTER(std::string_view, Name)
    {
        return ReflectionFields->_name;
    }
    SETTER(std::string_view, Name)
    {
        ESceneManager::Engine::RenameGameObject(this, value);
    }
    // get, set:
    //  게임 오브젝트의 이름
    PROPERTY(Name)

    //에디터 편집을 허용할 프로퍼티.
    REFLECT_PROPERTY(
        Name,
        ActiveSelf,
        IsStatic
    )

public:
    Transform transform;
private:
    using Base = ReflectSerializer; struct reflect_fields_struct 
    {
        rfl::Flatten<Base::reflect_fields_struct> Basefields{};
        std::string                              _name = "null";
        bool                                     _activeSelf = true;
        bool                                     _isStatic = false;
    REFLECT_FIELDS_END(GameObject)

private:
    std::string                              _ownerScene;
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

    bool result = factory.AddComponentToObject(this, typeid(TComponent).name());
    if (result)
    {
        return *(this->_components.back());
    }
    else
    {
        //존재하지 않는 컴포넌트.
        __debugbreak();
    } 
}

template<IS_BASE_COMPONENT_C TComponent>
inline std::weak_ptr<TComponent> GameObject::GetComponent()
{
    std::weak_ptr<TComponent> result;
    for (auto& component : _components)
    {
        if (typeid(TComponent) == typeid(*component))
        {
            result = std::static_pointer_cast<TComponent>(component);
            break;
        }
    }
    return result;
}

template<IS_BASE_COMPONENT_C TComponent>
inline std::weak_ptr<TComponent> GameObject::GetComponentAtIndex(size_t index)
{
    std::weak_ptr<TComponent> result;
    if (index >= _components.size())
    {
        return result;
    }
    else
    {
        result = std::dynamic_pointer_cast<TComponent>(_components[index]);
        return result;
    }
}

template<IS_BASE_COMPONENT_C TComponent>
inline std::vector<std::weak_ptr<TComponent>> GameObject::GetComponents()
{
    std::vector<std::weak_ptr<TComponent>> result;
    for (auto& component : _components)
    {
        if (typeid(TComponent) == typeid(*component))
        {
            result.emplace_back(std::static_pointer_cast<TComponent>(component));
        }
    }
    return result;
}
