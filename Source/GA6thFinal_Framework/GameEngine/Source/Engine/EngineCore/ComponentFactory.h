#pragma once
class GameObject;
class Component;
class MissingComponent;
class EngineCores;

template<typename T>
concept IS_BASE_COMPONENT_C = std::is_base_of_v<Component, T>;

class EComponentFactory
{
    friend class EngineCores;
public:
    //엔진 접근용 네임스페이스
    struct Engine
    {
        static constexpr const wchar_t* SCRIPTS_DLL_EDITOR_DEBUG_PATH = L"..\\GameScripts\\bin\\DebugEditor";
        static constexpr const wchar_t* BUILD_BATCH_EDITOR_DEBUG_PATH = L"..\\GameScripts\\build_debug_editor.bat";

        static constexpr const wchar_t* SCRIPTS_DLL_EDITOR_RELEASE_PATH = L"..\\GameScripts\\bin\\ReleaseEditor";
        static constexpr const wchar_t* BUILD_BATCH_EDITOR_RELEASE_PATH = L"..\\GameScripts\\build_release_editor.bat";

        static constexpr const wchar_t* SCRIPTS_DLL_DEBUG_PATH = L"..\\GameScripts\\bin\\Debug";
        static constexpr const wchar_t* BUILD_BATCH_DEBUG_PATH = L"..\\GameScripts\\build_debug.bat";

        static constexpr const wchar_t* SCRIPTS_DLL_RELEASE_PATH = L"..\\GameScripts\\bin\\Release";
        static constexpr const wchar_t* BUILD_BATCH_RELEASE_PATH = L"..\\GameScripts\\build_release.bat";
#ifdef _UMEDITOR
#ifdef _DEBUG
        static constexpr const wchar_t* SCRIPTS_DLL_PATH = SCRIPTS_DLL_EDITOR_DEBUG_PATH;
        static constexpr const wchar_t* BUILD_BATCH_PATH = BUILD_BATCH_EDITOR_DEBUG_PATH;
#else
        static constexpr const wchar_t* SCRIPTS_DLL_PATH = SCRIPTS_DLL_EDITOR_RELEASE_PATH;
        static constexpr const wchar_t* BUILD_BATCH_PATH = BUILD_BATCH_EDITOR_RELEASE_PATH;
#endif
#else
#ifdef _DEBUG
        static constexpr const wchar_t* SCRIPTS_DLL_PATH = SCRIPTS_DLL_DEBUG_PATH;
        static constexpr const wchar_t* BUILD_BATCH_PATH = BUILD_BATCH_DEBUG_PATH;
#else
        static constexpr const wchar_t* SCRIPTS_DLL_PATH = SCRIPTS_DLL_RELEASE_PATH;
        static constexpr const wchar_t* BUILD_BATCH_PATH = BUILD_BATCH_RELEASE_PATH;
#endif
#endif  
    };

private:
    EComponentFactory();
    ~EComponentFactory();

#ifndef _SCRIPTS_PROJECT
    /// <summary>
    /// 엔진 컴포넌트를 등록하는 함수입니다. 생성자에서 호출해야 합니다.
    /// </summary>
    /// <typeparam name="TGameObject :">등록할 타입 파라미터</typeparam>
    template <IS_BASE_COMPONENT_C TComponent>
    inline void RegisterEngineComponent()
    {
        const char* key = typeid(TComponent).name();
        _engineComponets.emplace_back(key, []()->Component*{ return new TComponent; });
    }
#endif
public:
    /// <summary>
    /// <summary>컴포넌트 팩토리를 초기화합니다. </summary>
    /// <para> DLL 빌드 -> DLL 로드 -> 팩토리 초기화 순으로 진행됩니다. </para>
    /// <para> 이미 초기화되어있으면 DLL언로드 후 다시 로드합니다.      </para>
    /// <para> 생성되어있던 모든 컴포넌트들도 파괴 후 다시 생성합니다.   </para>
    /// </summary>
    /// <returns>성공 여부</returns>
    bool InitalizeComponentFactory();

    /// <summary>
    /// 스크립트 DLL을 언로드합니다.
    /// </summary>
    void UninitalizeComponentFactory();

    /// <summary>
    /// 컴포넌트 클래스 typeid로 컴포넌트를 생성합니다.
    /// </summary>
    /// <param name="ownerObject :">컴포넌트를 추가할 오브젝트</param>
    /// <param name="typeid_name :">컴포넌트 typeid().name()</param>
    /// <returns>성공 여부</returns>
    Component* AddComponentToObject(GameObject* ownerObject, std::string_view typeid_name);

    /// <summary>
    /// 스크립트 DLL의 모든 컴포넌트 생성 키들을 반환합니다.
    /// </summary>
    /// <returns></returns>
    const std::vector<std::string>& GetNewComponentKeyList()
    {
        return m_NewScriptsKeyVec;
    }

    /// <summary>
    /// 스크립트 DLL 여부를 확인합니다.
    /// </summary>
    /// <returns></returns>
    inline bool HasScript() const
    { 
        return (m_scriptsDll != NULL) ? true : false;
    }

    /// <summary>
    /// 컴포넌트 존재 유무를 확인합니다.
    /// </summary>
    /// <param name="typeid_name :">확인할 컴포넌트 typeid_name</param>
    /// <returns></returns>
    bool HasComponent(std::string_view typeid_name)
    {
        auto findIter = _newScriptsFunctionMap.find(typeid_name.data());
        if (findIter != _newScriptsFunctionMap.end())
        {
            return true;
        }
        return false;
    }

    /// <summary>
    /// <para> 새로운 컴포넌트 스크립트 파일을 생성합니다.        </para>
    /// <para> 파일 이름과 같은 컴포넌트 클래스를 정의해줍니다.   </para>
    /// </summary>
    /// <param name="fileName :">사용할 파일 이름</param>
    void MakeScriptFile(const char* fileName) const;

    /// <summary>
    /// 컴포넌트를 YAML로 직렬화합니다.
    /// </summary>
    YAML::Node SerializeToYaml(Component* component);

    /// <summary>
    /// Yaml 형식으로 직렬화된 컴포넌트를 런타임 오브젝트에 추가합니다. 
    /// </summary>
    /// <returns></returns>
    bool AddComponentToYamlLifeCycle(GameObject* ownerObject, YAML::Node* componentNode);

    /// <summary>
    /// Yaml 형식으로 직렬화된 컴포넌트를 즉시 오브젝트에 추가합니다. (리소스 프리팹 전용)
    /// </summary>
    /// <returns></returns>
    bool AddComponentToYamlNow(GameObject* ownerObject, YAML::Node* componentNode);

private:
    using InitScripts = void(*)(const std::shared_ptr<EngineCores>, ImGuiContext*);
    using MakeUmScriptsFile = void(*)(const char* fileName);
    using NewScripts = Component*(*)();

   std::vector<std::pair<std::string, std::weak_ptr<Component>>> _componentInstanceVec;

    HMODULE m_scriptsDll{};
    std::map<std::string, std::function<Component*()>> _newScriptsFunctionMap{};
    std::vector<std::string> m_NewScriptsKeyVec{}; 

    std::vector<std::pair<std::string, std::function<Component*()>>> _engineComponets;

    MakeUmScriptsFile MakeScriptFunc = nullptr;
private:
    //엔진 컴포넌트들을 m_NewScriptsFunctionMap에 등록하는 함수.
    void AddEngineComponentsToScripts();

    //컴포넌트를 동적할당후 shared_ptr로 반환합니다.
    //매개변수로 생성할 컴포넌트 typeid().name()을 전달해야합니다.
    std::shared_ptr<Component> NewComponent(std::string_view typeid_name);

    //내부에서 사용하는 Missing Component 생성용 함수
    std::shared_ptr<MissingComponent> NewMissingComponent();

    //컴포넌트를 엔진에 사용하기 위해 초기화합니다.
    //초기화 후 컴포넌트의 Reset을 호출합니다.
    void ResetComponent(GameObject* ownerObject, Component* component);

    //컴포넌트를 Yaml로 직렬화
    YAML::Node MakeYamlToComponent(Component* component);

    //Yaml로 컴포넌트 생성 Reset도 호출함.
    std::shared_ptr<Component> MakeComponentToYaml(GameObject* ownerObject, YAML::Node* componentNode);

};