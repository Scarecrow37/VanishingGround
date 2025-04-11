#pragma once
class GameObject;
class ESceneManager;

//참고 
// Unity SceneManager https://docs.unity3d.com/6000.0/Documentation/ScriptReference/SceneManagement.SceneManager.html
// Unity Scene https://docs.unity3d.com/6000.0/Documentation/ScriptReference/SceneManagement.Scene.html

//이 구조체는 단순히 씬의 정보를 나열한 구조체에 불과함.
//일단 선언만. 구현은 나중에.
struct Scene :
    public ReflectSerializer
{
    USING_PROPERTY(Scene)
    friend class ESceneManager;
    Scene() = default;
    ~Scene() = default;
public:
    /// <summary>
    /// <para> 씬에 속한 Root GameObject들을 반환합니다. (자식 오브젝트는 포함하지 않습니다.)   </para>
    /// </summary>
    /// <returns>std::ranges::filter_view 객체를 반환합니다.</returns>
    inline auto GetRootGameObjects() const;

    /// <summary>
    /// 다른 Scene인지 경로를 비교합니다.
    /// </summary>
    /// <returns>비교 결과</returns>
    bool operator!=(const Scene& other)
    {
        return (std::string)path != (std::string)other.path;
    }
    /// <summary>
    /// 같은 Scene인지 경로를 비교합니다.
    /// </summary>
    /// <returns>비교 결과</returns>
    bool operator==(const Scene& other)
    {
        return (std::string)path == (std::string)other.path;
    }

    GETTER_ONLY(size_t, buildIndex) { return ReflectFields->_buildIndex; }
    // get : 빌드 설정에서의 Scene 인덱스를 반환합니다. 포함안된 씬들은 -1을
    // 반환합니다.
    PROPERTY(buildIndex)

    GETTER_ONLY(bool, isLoaded) { return _isLoaded; }
    // get : 이 씬의 로드 여부를 반환합니다.
    PROPERTY(isLoaded)

    GETTER_ONLY(std::string, name)
    {
        return ReflectFields->_filePath.stem().string();
    }
    // get : 이 씬의 이름을 반환합니다.
    PROPERTY(name)

    GETTER_ONLY(std::string, path) { return ReflectFields->_filePath.string(); }
    // get : 이 씬 파일의 상대 경로를 반환합니다.
    PROPERTY(path)
private:
    //필터용 함수
    bool RootGameObjectsFilter(GameObject* obj) const;

private:
    bool _isLoaded = false;

    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    int _buildIndex = -1;
    std::filesystem::path _filePath; //파일 경로
    REFLECT_FIELDS_END(Scene)
};

/// <summary>
/// LoadScene 함수에서 매개변수로 사용되는 enum. 씬 로드 방식을 결정합니다.
/// </summary>
enum class LoadSceneMode
{
    /*모든 오브젝트들을 파괴 시킨 뒤, 새로운 씬을 로드합니다.*/
    SINGLE,
    /*오브젝트를 파괴 시키지 않고, 씬을 추가합니다.*/
    ADDITIVE
};

//함수는 일단 선언만. 구현은 나중에.
class ESceneManager
{
    friend class EngineCores;
    ESceneManager();
    ~ESceneManager();
    USING_PROPERTY(ESceneManager)
public:
    //엔진 접근용 네임스페이스
    struct Engine
    {
        /// <summary>
        /// 씬 매니저가 관리하는 오브젝트를 전부 정리합니다.
        /// </summary>
        static void CleanupSceneManager();

        /// <summary>
        /// 씬 매니저가 관리하는 오브젝트들을 업데이트합니다. 클라이언트에서 매 틱 호출해야합니다.
        /// </summary>
        static void SceneUpdate();

        /// <summary>
        /// 씬에 게임 오브젝트를 추가합니다.
        /// </summary>
        /// <param name="GameObject :">추가할 오브젝트</param>
        static void AddGameObjectToLifeCycle(std::shared_ptr<GameObject> gameObject);

        /// <summary>
        /// 컴포넌트를 Awake, Start를 호출하기 위한 대기열에 추가합니다.
        /// </summary>
        /// <param name="component"></param>
        static void AddComponentToLifeCycle(std::shared_ptr<Component> component);

        /// <summary>
        /// <para> 게임 오브젝트의 라이프 사이클 활성화 여부를 변경합니다. </para>
        /// <para> false로 설정시 라이프 사이클 함수를 수행하지 않습니다. </para>
        /// <param name="instanceID :">적용 대상의 instanceID</param>
        /// <param name="value :">적용 값</param>
        /// </summary>
        static void SetGameObjectActive(int instanceID, bool value);

        /// <summary>
        /// <para> 컴포넌트의 라이프 사이클 활성화 여부를 변경합니다.     </para>
        /// <para> false로 설정시 라이프 사이클 함수를 수행하지 않습니다. </para>
        /// </summary>
        /// <param name="component :">적용 대상</param>
        /// <param name="value :">적용 값</param>
        static void SetComponentEnable(Component* component, bool value);

        /// <summary>
        /// 게임 오브젝트의 이름으로 오브젝트를 탐색합니다.
        /// </summary>
        /// <param name="name :">찾을 오브젝트의 이름</param>
        /// <returns>성공시 weak_ptr에 담아줍니다.</returns>
        static std::weak_ptr<GameObject> FindGameObjectWithName(std::string_view name);

        /// <summary>
        /// 게임 오브젝트의 이름으로 오브젝트를 탐색해 전부 반환합니다.
        /// </summary>
        /// <param name="name :">찾을 오브젝트의 이름</param>
        /// <returns>성공시 weak_ptr에 담아줍니다.</returns>
        static std::vector<std::weak_ptr<GameObject>> FindGameObjectsWithName(std::string_view name);

        /// <summary>
        /// 게임 오브젝트의 이름을 변경합니다.
        /// </summary>
        /// <param name="gameObject :">대상</param>
        /// <param name="newName :">새로운 이름</param>
        static void RenameGameObject(GameObject* gameObject, std::string_view newName);

        /// <summary>
        /// 현재 라이프사이클이 활성화된 오브젝트들을 전부 반환합니다.
        /// </summary>
        static const std::vector<std::shared_ptr<GameObject>>& GetRuntimeObjects();

        /// <summary>
        /// 오브젝트를 파괴하도록 합니다.
        /// </summary>
        /// <param name="component"></param>
        static void DestroyObject(Component* component);
        static void DestroyObject(Component& component);
        static void DestroyObject(GameObject* gameObject);
        static void DestroyObject(GameObject& gameObject);

        /// <summary>
        /// 씬이 새로 로드되도 파괴되지 않는 오브젝트로 만듭니다.
        /// </summary>
        static void DontDestroyOnLoadObject(GameObject* gameObject);
        static void DontDestroyOnLoadObject(GameObject& gameObject);
    };

public:
    /// <summary>
    /// 등록된 모든 씬들을 반환합니다. 씬 이름이 key로 Scene 객체가 value로 저장되어있습니다.
    /// </summary>
    inline const std::unordered_map<std::string, Scene>& GetBuildScenes()
    {
        return _buildScnes;
    }

    /// <summary>
    /// 실제 로드된 씬의 개수를 반환합니다.
    /// </summary>
    /// <returns></returns>
    inline size_t LoadedSceneCount()
    {
        size_t count = 0;
        for (auto& [name, scene] : _buildScnes)
        {
            if (scene._isLoaded)
            {
                count++;
            }
        }
        return count;
    }

    /// <summary>
    /// 등록된 씬의 개수를 반환합니다.
    /// </summary>
    /// <returns></returns>
    inline size_t SceneCount()
    {
        return _buildScnes.size();
    }

    /// <summary>
    /// 씬을 생성합니다. 만든 씬은 로드되는게 아니라 buildScnes 항목에만 추가됩니다.
    /// </summary>
    Scene& CreateScene(std::string_view sceneName);

    /// <summary>
    /// 씬을 로드합니다.
    /// </summary>
    /// <param name="sceneName :">로드한 씬의 이름 </param>
    /// <param name="mode :">로드 방식             </param>
    void LoadScene(std::string_view sceneName, LoadSceneMode mode = LoadSceneMode::SINGLE);

    /// <summary>
    /// 기준이되는 Scene 을 반환합니다.
    /// 가장 최근에 LoadSceneMode::Single 모드로 Load된 Scene이 기준이됩니다.
    /// <para> NewGameObject로 런타임 도중에 생성되는 모든 오브젝트들은 이 Main Scene 에 추가됩니다. </para>
    /// </summary>
    /// <returns>Main Scene</returns>
    Scene& GetMainScene()
    {
        return _buildScnes[_mainScene];
    }

    /// <summary>
    /// 씬 정보를 이름을 통해 찾아서 반환합니다.
    /// </summary>
    /// <returns>성공시 Scene 의 주소, 실패시 nullptr</returns>
    Scene* GetSceneByName(std::string_view name);
private:
    //Life cycle 을 수행. 클라에서 매틱 호출해야함.
    void SceneUpdate();

private:
    void ObjectsAddRuntime();        //추가 대기중인 오브젝트, 컴포넌트를 라이프 사이클에 포함시킵니다.
    void ObjectsAwake();             //Awake 예정인 컴포넌트들의 Awake 함수를 호출합니다.
    void ObjectsOnEnable();          //OnEnable 예정인 컴포넌트들의 OnEnable 함수를 호출합니다.
    void ObjectsStart();             //Start 예정인 컴포넌트들의 Start 함수를 호출합니다.
    void ObjectsFixedUpdate();       //FixedUpdate를 호출합니다.
    void ObjectsUpdate();            //Update 를 호출합니다.
    void ObjectsLateUpdate();        //LateUpdate를 호출합니다.
    void ObjectsMatrixUpdate();      //오브젝트들의 행렬을 업데이트합니다.
    void ObjectsApplicationQuit();   //OnApplicationQuit를 호출합니다.
    void ObjectsOnDisable();         //OnDisable 예정인 컴포넌트들의 OnDisable 함수를 호출해줍니다.
    void ObjectsDestroy();           //Destroy 예정인 컴포넌트들의 OnDestroy 함수를 호출 한 뒤 파괴합니다.
private:
    /*게임오브젝트의 Life cycle 수행 여부를 확인하는 함수*/
    bool IsRuntimeActive(std::shared_ptr<GameObject>& obj);

    /*
        Awake 랑 Start 호출 대기중인 컴포넌트를 찾아서 대기열에서 삭제합니다.
        호출 전에 파괴되는 컴포넌트를 위해 존재하는 함수입니다.
    */
    void NotInitDestroyComponentEraseToWaitVec(Component* destroyComponent);
private:
    //Life cycle 에 포함되는 실제 오브젝트들 항목
    std::vector<std::shared_ptr<GameObject>> _runtimeObjects;

    //오브젝트 이름과 포인터로 관리하는 map
    std::unordered_map<std::string, std::unordered_set<std::shared_ptr<GameObject>>> _runtimeObjectsUnorderedMap;

    //오브젝트 추가 대기열 
    std::vector<std::shared_ptr<GameObject>> _addGameObjectsQueue;
    std::vector<std::shared_ptr<Component>> _addComponentsQueue;

    //오브젝트 삭제 대기열
    std::pair<std::unordered_set<GameObject*>, std::vector<GameObject*>> _destroyObjectsQueue;
    std::pair<std::unordered_set<Component*>, std::vector<Component*>> _destroyComponentsQueue;

    //초기화 함수 호출 대기열
    std::vector<std::shared_ptr<Component>> _waitAwakeVec;
    std::vector<std::shared_ptr<Component>> _waitStartVec;

    //OnEnable, OnDisable을 set과 같이 관리
    std::tuple<std::unordered_set<Component*>, std::vector<Component*>, std::vector<bool*>> _onEnableQueue;
    std::tuple<std::unordered_set<Component*>, std::vector<Component*>, std::vector<bool*>> _onDisableQueue;

private:
    //생성한 씬 파일들 입니다. key : 파일 확장자를 제외한 파일 이름, value : 해당 씬의 정보 
    std::unordered_map<std::string, Scene> _buildScnes;
    //현재 Single로 로드된 씬 이름입니다. NewGameObject를 하면 이 씬에 오브젝트가 생성됩니다.
    std::string _mainScene = "Null";
};

inline auto Scene::GetRootGameObjects() const
{
    const auto& objectsList = ESceneManager::Engine::GetRuntimeObjects();
    auto rootObjects = objectsList | std::ranges::views::filter([this](auto& obj)
        {
            if (obj == nullptr)
                return false;

            return RootGameObjectsFilter(obj.get());
        });
    return rootObjects;
}