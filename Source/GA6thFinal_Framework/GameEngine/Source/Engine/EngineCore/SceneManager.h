#pragma once
class GameObject;
class Component;
class ESceneManager;
class InputReceiver;
class GraphicsBase;
class MeshComponent;
class LightComponent;
class CameraComponent;
class Model;
class Font;
class SDFFont;
namespace Command::EditorScene
{
    class NewGameObjectCommand;
    class DestroyGameObjectCommand;
    class AddComponentCommand;
    class DestroyComponentCommand;
    class DuplicateCommand;
    class PasteObjectCommand;
};

//참고 
// Unity SceneManager https://docs.unity3d.com/6000.0/Documentation/ScriptReference/SceneManagement.SceneManager.html
// Unity Scene https://docs.unity3d.com/6000.0/Documentation/ScriptReference/SceneManagement.Scene.html

//이 구조체는 단순히 씬의 정보를 나열한 구조체에 불과함.
//일단 선언만. 구현은 나중에.
struct Scene
{
    USING_PROPERTY(Scene)
    friend class ESceneManager;
    friend class EditorSceneMenu;

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
        return (std::string)Path != (std::string)other.Path;
    }
    /// <summary>
    /// 같은 Scene인지 경로를 비교합니다.
    /// </summary>
    /// <returns>비교 결과</returns>
    bool operator==(const Scene& other)
    {
        return (std::string)Path == (std::string)other.Path;
    }

    GETTER_ONLY(bool, isLoaded) { return _isLoaded; }
    // get : 이 씬의 로드 여부를 반환합니다.
    PROPERTY(isLoaded)

    GETTER_ONLY(std::string, Name)
    { 
        return _guid.ToPath().stem().string();
    }
    // std::string : 이 씬의 이름을 반환합니다.
    PROPERTY(Name)

    GETTER_ONLY(std::string, Path)
    {
        return _guid.ToPath().string();
    }
    // std::string : 이 씬 파일의 상대 경로를 반환합니다.
    PROPERTY(Path)

    GETTER(bool, IsDirty)
    {
        return _isDirty;
    }
    SETTER(bool, IsDirty);
    PROPERTY(IsDirty)

private:
    bool _isDontDestroyOnLoad = false;
    bool _isDirty   = false;
    bool _isLoaded = false;
    File::Guid _guid = STR_NULL;
    File::Guid _skyBox = STR_NULL;
    File::Guid _skyIBL = STR_NULL;
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
    : 
    File::FileEventSubscriber
{
private:
    USING_PROPERTY(ESceneManager)
    friend class EngineCores;
    friend class Application;
    ESceneManager();
    ~ESceneManager();

    ESceneManager& operator=(const ESceneManager& rhs) = delete;

private:
    void LoadSettingFile();

public:
    //현재 빌드 설정 파일을 저장합니다.
    void SaveSettingFile() const;
public:
    /// <summary>
    /// 전달받은 경로의 씬 파일을 통해 로드된 RootGameObjects를 반환합니다.
    /// </summary>
    /// <param name="sceneName"></param>
    /// <returns></returns>
    static inline auto GetRootGameObjectsByPath(std::string_view path);
private:
    //필터용 함수
    static bool RootGameObjectsFilter(GameObject* obj, std::string_view scenePath);

public:
    class InputSystem;
    static constexpr const char* SCENE_EXTENSION = ".UmScene";
    static constexpr const char* SETTING_FILE_NAME = "SceneManager.setting.json";
    static constexpr const char* EMPTY_SCENE_NAME  = "EmptyScene";
    static constexpr const char* DONT_DESTROY_ON_LOAD_SCENE_NAME = "DontDestroyOnLoad";
    static std::filesystem::path GetSettingFilePath();

    //엔진 접근용 네임스페이스
    struct Engine
    {
        /// <summary>
        /// Scene FileEventSubscriber를 등록합니다.
        /// </summary>
        static void RegisterFileEvents();

        /// <summary>
        /// 씬 매니저가 관리하는 오브젝트를 전부 정리합니다.
        /// </summary>
        static void CleanupSceneManager();

        /// <summary>
        /// 씬 매니저가 관리하는 오브젝트들을 업데이트합니다. 클라이언트에서 매 틱 호출해야합니다.
        /// </summary>
        static void SceneUpdate();

        /// <summary>
        /// 프레임 종료 직전 수행해야하는 업데이트.
        /// </summary>
        static void SceneFinalUpdate();

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
        static void SetGameObjectActive(GameObject* pObject, bool value);

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
        static std::weak_ptr<GameObject> FindGameObjectWithName(const std::string& name);

        /// <summary>
        /// 게임 오브젝트의 이름으로 오브젝트를 탐색해 전부 반환합니다.
        /// </summary>
        /// <param name="name :">찾을 오브젝트의 이름</param>
        /// <returns>성공시 weak_ptr에 담아줍니다.</returns>
        static std::vector<std::weak_ptr<GameObject>> FindGameObjectsWithName(const std::string& name);

        /// <summary>
        /// 게임 오브젝트의 태그로 오브젝트를 탐색합니다.
        /// </summary>
        /// <param name="tag :">찾을 오브젝트의 태그</param>
        /// <returns>성공시 weak_ptr에 담아줍니다.</returns>
        static std::weak_ptr<GameObject> FindGameObjectWithTag(const std::string& tag);

        /// <summary>
        /// 게임 오브젝트의 태그로 오브젝트를 탐색해 전부 반환합니다.
        /// </summary>
        /// <param name="tag :">찾을 오브젝트의 태그</param>
        /// <returns>성공시 weak_ptr에 담아줍니다.</returns>
        static std::vector<std::weak_ptr<GameObject>> FindGameObjectsWithTag(const std::string& tag);

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

        /// <summary>
        /// 시작 씬 설정을 가져옵니다.
        /// </summary>
        /// <returns></returns>
        static std::string& GetStartSceneSetting();

        /// <summary>
        /// 시작 씬을 로드합니다.
        /// </summary>
        static void LoadStartScene();

        /// <summary>
        /// 프리팹 인스턴스를 Swap 합니다. Reset만 호출되며 인스턴스 아이디는 유지됩니다.
        /// </summary>
        static std::shared_ptr<GameObject> SwapPrefabInstance(GameObject* original, GameObject* remake);

        /// <summary>
        /// 씬의 스카이박스를 설정합니다.
        /// </summary>
        static void SetSceneSkyBoxGuid(Scene& scene, const File::Guid& skyBox);

        /// <summary>
        /// 씬의 스카이박스를 설정합니다.
        /// </summary>
        static void SetSceneSkyBoxPath(Scene& scene, std::string_view skyBoxPath);

        /// <summary>
        /// 씬의 IBL 텍스쳐를 설정합니다.
        /// </summary>
        static void SetSceneIBLGuid(Scene& scene, const File::Guid& skyIBL);

        /// <summary>
        /// 씬의 IBL 텍스쳐를 설정합니다.
        /// </summary>
        static void SetSceneIBLPath(Scene& scene, std::string_view skyIBLPath);

        /// <summary>
        /// 오브젝트의 행렬을 명시적으로 업데이트합니다. (성능 하락 주의)
        /// </summary>
        /// <param name="gameObject"></param>
        static void UpdateMatrix(GameObject* gameObject);

        /// <summary>
        /// 게임 오브젝트의 태그를 RuntimeObjectTagMap에 등록합니다.
        /// FindWithTag에 검색될려면 반드시 이 함수로 등록해야합니다.
        /// </summary>
        /// <param name="gameObject :">등록할 오브젝트</param>
        /// <param name="tag :">사용할 태그</param>
        /// <returns></returns>
        static bool InsertGameObjectTag(GameObject* gameObject, std::string_view tag);

        /// <summary>
        /// 게임 오브젝트의 태그를 RuntimeObjectTagMap에서 지웁니다.
        /// 오브젝트가 파괴될때 반드시 지워야합니다.
        /// </summary>
        /// <param name="gameObject :">지울 오브젝트</param>
        /// <param name="tag :">태그</param>
        /// <returns></returns>
        static bool EraseGameObjectTag(GameObject* gameObject, std::string_view tag); 

        /// <summary>
        /// 게임을 렌더링할 메인 카메라를 설정합니다.
        /// </summary>
        static void SetSceneMainCamera(CameraComponent* camera);

        /// <summary>
        /// 메인카메라를 해제합니다.
        /// </summary>
        /// <param name="camera"></param>
        static void ResetSceneMainCamera();

        /// <summary>
        /// 메인 카메라를 반환합니다.
        /// </summary>
        /// <returns></returns>
        static CameraComponent* GetMainCamera();

        /// <summary>
        /// 씬 매니저의 InputSystem을 반환합니다.
        /// </summary>
        /// <returns></returns>
        static ESceneManager::InputSystem& GetInputSystem();

        /// <summary>
        /// 현재 플레이 모드 여부를 반환합니다.
        /// </summary>
        /// <param name="sceneManager"></param>
        /// <returns></returns>
        static constexpr bool IsPlayMode(ESceneManager& sceneManager) { return sceneManager._isPlay; }

        /// <summary>
        /// 런타임에 생성된 메시 컴포넌트를 push합니다. 메시 컴포넌트 생성자에서만 사용합니다.
        /// </summary>
        /// <param name="component"></param>
        static void PushRuntimeMeshComponent(MeshComponent* component);
    };

public:
    /// <summary>
    /// 로드된 모든 씬들을 반환합니다. 로드된 순서를 보장합니다.
    /// </summary>
    inline const std::vector<Scene*>& GetLoadedScenes()
    { 
        return _lodedSceneList;
    }

    /// <summary>
    /// 등록된 모든 씬들을 반환합니다. 씬 GUID가 key로 Scene 객체가 value로 저장되어있습니다.
    /// </summary>
    inline const std::unordered_map<File::Guid, Scene>& GetScenesMap()
    {
        return _scenesMap;
    }

    /// <summary>
    /// 실제 로드된 씬의 개수를 반환합니다.
    /// </summary>
    /// <returns></returns>
    inline size_t LoadedSceneCount() const
    { 
        return _lodedSceneList.size();
    }

    /// <summary>
    /// 등록된 씬의 개수를 반환합니다.
    /// </summary>
    /// <returns></returns>
    inline size_t SceneCount()
    {
        return _scenesMap.size();
    }

    // <summary>
    /// <para> 빈 씬을 UmScene파일로 저장후 로드합니다. </para>
    /// <para> 로드된 이후 매개변수로 등록한 함수를 호출해줍니다. </para>
    /// </summary>
    /// <param name="name :">파일 이름</param>
    /// <param name="outPath :">저장할 경로</param>
    /// <param name="isOverride :">덮어쓰기 안내문구 스킵 여부</param>
    void CreateEmptySceneAndLoad(std::string_view name, std::string_view outPath, const std::function<void()>& loadEvent = std::function<void()>());

    /// <summary>
    /// 씬을 로드합니다.
    /// </summary>
    /// <param name="sceneName :">로드할 씬의 이름 </param>
    /// <param name="mode :">로드 방식             </param>
    void LoadScene(std::string_view sceneName, LoadSceneMode mode = LoadSceneMode::SINGLE);

    /// <summary>
    /// 씬을 언로드합니다. ADDITIVE로 로드한 씬만 Unload 할 수 있습니다.
    /// </summary>
    /// <param name="sceneName :">로드할 씬의 이름 </param>
    void UnloadScene(std::string_view sceneName);

    /// <summary>
    /// 기준이되는 Scene 을 반환합니다.
    /// 가장 최근에 LoadSceneMode::Single 모드로 Load된 Scene이 기준이됩니다.
    /// <para> NewGameObject로 런타임 도중에 생성되는 모든 오브젝트들은 이 Main Scene 에 추가됩니다. </para>
    /// </summary>
    /// <returns>Main Scene</returns>
    Scene* GetMainScene()
    { 
         return GetSceneByName(_setting.MainScene);
    }

    /// <summary>
    /// 씬 로드해도 파괴되지 않는 씬을 가져옵니다.
    /// </summary>
    /// <returns>DontDestroyOnLoad 오브젝트가 없으면 nullptr</returns>
    Scene* GetDontDestroyOnLoadScene();

    /// <summary>
    /// 씬 정보를 이름을 통해 찾아서 반환합니다.
    /// </summary>
    /// <returns>성공시 Scene 의 주소, 실패시 nullptr</returns>
    Scene* GetSceneByName(std::string_view name);

    /// <summary>
    /// 씬을 UmScene파일로 저장합니다. FileSystem의 RootPath 기준으로 저장합니다. 
    /// </summary>
    /// <param name="scene :">저장할 파일</param>
    /// <param name="outPath :">저장할 경로</param>
    /// <param name="isOverride :">덮어쓰기 안내문구 스킵 여부</param>
    void WriteSceneToFile(Scene& scene, std::string_view outPath, bool isOverride = false);

    /// <summary>
    /// <para> 빈 씬을 UmScene파일로 저장합니다. FileSystem의 RootPath 기준으로 저장합니다. </para>
    /// </summary>
    /// <param name="name :">파일 이름</param>
    /// <param name="outPath :">저장할 경로</param>
    /// <param name="isOverride :">덮어쓰기 안내문구 스킵 여부</param>
    void WriteEmptySceneToFile(std::string_view name, std::string_view outPath, bool isOverride = false);

    /// <summary>
    /// 스카이 박스를 설정합니다.
    /// </summary>
    /// <param name="path :">사용할 스카이박스</param>
    /// <returns></returns>
    bool SetSkyBox(const File::Path& path);
    
    /// <summary>
    /// IBL 텍스쳐를 설정합니다.
    /// </summary>
    /// <param name="path :">사용할 스카이박스</param>
    /// <returns></returns>
    bool SetSkyIBL(const File::Path& path);

    /// <summary>
    /// 현재 씬에 존재하는 MeshComponent들을 반환합니다. *매 프레임 호출하면 퍼포먼스가 하락할 수 있습니다.*
    /// </summary>
    /// <returns></returns>
    const std::vector<std::weak_ptr<MeshComponent>>& GetMeshComponents();

    void ClearExpiredMeshComponents();

    class SceneResourceManager
    {
    public:
        SceneResourceManager() = default;
        ~SceneResourceManager() = default;

        struct Engine
        {
            /// <summary>
            /// 해당 리소스 매니저를 업데이트 합니다.
            /// </summary>
            /// <param name="manager"></param>
            static void Update(SceneResourceManager& manager);

            /// <summary>
            /// 해당 리소스 매니저를 정리합니다.
            /// </summary>
            /// <param name="manager"></param>
            static void CleanUp(SceneResourceManager& manager);
        };

        /// <summary>
        /// Model 리소스 로드를 요청합니다.
        /// </summary>
        /// <param name="component :">대상 컴포넌트</param>
        /// <param name="guid :">로드할 리소스의 guid</param>
        /// <param name="func :">리소스 로드후 호출되는 콜백 함수</param>
        void RequestModelResource(const Component* component, const File::Guid& guid, const std::function<void()>& func);
        void RequestModelResource(const Component* component, const File::Path& path, const std::function<void()>& func);
        
        /// <summary>
        /// Texture 리소스 로드를 요청합니다.
        /// </summary>
        /// <param name="component :">대상 컴포넌트</param>
        /// <param name="guid :">로드할 리소스의 guid</param>
        /// <param name="func :">리소스 로드후 호출되는 콜백 함수</param>
        void RequestTextureResource(const Component* component, const File::Guid& guid, const std::function<void()>& func);
        void RequestTextureResource(const Component* component, const File::Path& path, const std::function<void()>& func);

        /// <summary>
        /// Font 리소스 로드를 요청합니다.
        /// </summary>
        /// <param name="component :">대상 컴포넌트</param>
        /// <param name="guid :">로드할 리소스의 guid</param>
        /// <param name="func :">리소스 로드후 호출되는 콜백 함수</param>
        void RequestFontResource(const Component* component, const File::Guid& guid, const std::function<void()>& func);
        void RequestFontResource(const Component* component, const File::Path& path, const std::function<void()>& func);

        /// <summary>
        /// SDFFont 리소스 로드를 요청합니다.
        /// </summary>
        /// <param name="component :">대상 컴포넌트</param>
        /// <param name="guid :">로드할 리소스의 guid</param>
        /// <param name="func :">리소스 로드후 호출되는 콜백 함수</param>
        void RequestSDFFontResource(const Component* component, const File::Guid& guid, const std::function<void()>& func);
        void RequestSDFFontResource(const Component* component, const File::Path& path, const std::function<void()>& func);

        bool CheckAllResourceLoad();
    private:
        template <typename T>
        struct RenderResource
        {
            using ResourceQueue =  Concurrency::concurrent_queue<std::tuple<std::weak_ptr<Component>, File::Path, std::function<void()>>>;

            ResourceQueue                                      ResourceLoadQueue;
            std::unordered_map<File::Path, std::shared_ptr<T>> RenderResource;
        };
        RenderResource<Model>   _models;
        RenderResource<Texture> _textures;
        RenderResource<Font>    _fonts;
        RenderResource<SDFFont> _sdfFonts;

        template <typename T>
        void UpdateRenderResource(RenderResource<T>& resource);
    };
    /// <summary>
    /// 씬 리소스 관리를 위한 맴버입니다.
    /// </summary>
    SceneResourceManager ResourceManager;
    
public:
    class InputSystem
    {
    public:
        enum class ControllerButton
        {
            DPAD_UP,
            DPAD_DOWN,
            DPAD_LEFT,
            DPAD_RIGHT,
            START,
            BACK,
            LEFT_THUMB_BUTTON,
            RIGHT_THUMB_BUTTON,
            LEFT_SHOULDER,
            RIGHT_SHOULDER,
            LEFT_TRIGGER,
            RIGHT_TRIGGER,
            A,
            B,
            X,
            Y,
            LEFT_THUMB_STICK,
            RIGHT_THUMB_STICK,
            UNKNOWN
        };

        enum class Action
        {
            IDLE,
            RELEASED, 
            HELD,
            PRESSED,
            UNKNOWN
        };

        /// <summary>
        /// 인풋 시스템을 업데이트 합니다. 
        /// </summary>
        void UpdateInput();

        /// <summary>
        /// InputReceiver를 등록합니다.
        /// </summary>
        /// <param name="receiver :">등록할 receiver</param>
        /// <param name="buttonIndex :">버튼 index</param>
        /// <param name="actionIndex :">액션 index</param>
        /// <param name="func :">이벤트 함수</param>
        void RegisterInputReceiver(InputReceiver& receiver, int buttonIndex, int actionIndex, std::function<void(const Input::Controller& controller)> func);

        /// <summary>
        /// InputReceiver 레이어를 스택에 Push 합니다.
        /// </summary>
        /// <param name="receiver :">레이어 리시버</param>
        bool PushReceiverToInputStack(InputReceiver& receiver);

        /// <summary>
        /// InputReceiver 레이어를 Pop 합니다.
        /// </summary>
        bool PopReceiverToInputStack(InputReceiver& receiver);

        /// <summary>
        /// 등록된 모든 Receiver을 해제합니다.
        /// </summary>
        void CleanupInputReceivers();

        /// <summary>
        /// Controller를 반환합니다.
        /// </summary>
        /// <returns></returns>
        Input::Controller& GetController() { return _inputController; }

        /// <summary>
        /// 컨트롤러의 진동을 설정합니다.
        /// </summary>
        /// <param name="vibration">적용할 진동 설정을 나타내는 Vibration 구조체입니다.</param>
        void Vibrate(Input::ControllerTypes::Vibration vibration);

        /// <summary>
        /// 컨트롤러의 진동을 중지합니다.
        /// </summary>
        void StopVibration();

    private:
        static constexpr size_t ACTION_COUNT = (size_t)Action::UNKNOWN;
        static constexpr size_t CONTROLLER_BUTTON_COUNT = (size_t)ControllerButton::UNKNOWN;

        Input::Adapter                                  _inputAdapter;
        Input::Controller                               _inputController{&_inputAdapter};
        bool                                            _isConnect = false;
        std::array<Action, CONTROLLER_BUTTON_COUNT>     _actionTracker{Action::IDLE,};
        std::array<bool, CONTROLLER_BUTTON_COUNT>       _actionChecker{false,};

        std::array<std::array<std::vector<std::pair<std::shared_ptr<bool>, std::function<void(const Input::Controller&)>>>, 
            ACTION_COUNT>,
            CONTROLLER_BUTTON_COUNT>
            _receivers;

        std::deque<std::pair<bool*, std::weak_ptr<bool>>> _layerStack;

    private:
        void UpdateTracker(Input::Controller::Button button);
        void UpdateAnalogButtons();
    };

private:
    InputSystem _inputSystem;

private:
#ifdef _UMEDITOR
    //play 여부
    bool _isPlay = true;
#else
    static constexpr bool _isPlay = true;
#endif

private:
    //Life cycle 을 수행. 클라에서 매틱 호출해야함.
    void SceneUpdate();
 
private:
    bool ResourceLoadWait();
    void ObjectsInputUpdate();       //Input을 사용하는 Component들의 Event를 Update합니다.
    void ObjectsFixedUpdate();       //FixedUpdate를 호출합니다.
    void ObjectsUpdate();            //Update 를 호출합니다.
    void ObjectsLateUpdate();        //LateUpdate를 호출합니다.

    void ObjectsAddRuntime();        //추가 대기중인 오브젝트, 컴포넌트를 라이프 사이클에 포함시킵니다.
    void ResourceManagerUpdate();    //리로스 매니저를 업데이트합니다.
    void ObjectsOnEnable();          //OnEnable 예정인 컴포넌트들의 OnEnable 함수를 호출합니다.
    void ObjectsOnDisable();         //OnDisable 예정인 컴포넌트들의 OnDisable 함수를 호출해줍니다.
    void ObjectsAwake();             //Awake 예정인 컴포넌트들의 Awake 함수를 호출합니다.
    void ObjectsStart();             //Start 예정인 컴포넌트들의 Start 함수를 호출합니다.
    void ObjectsApplicationQuit();   //OnApplicationQuit를 호출합니다.
    void ObjectsDestroy();           //Destroy 예정인 컴포넌트들의 OnDestroy 함수를 호출 한 뒤 파괴합니다.
    void ObjectsMatrixUpdate();      //오브젝트들의 행렬을 업데이트합니다.
    void ObjectsAddLoadScene();      //다음에 로드할 씬의 오브젝트들을 추가합니다.

private:
    //마지막 씬 업데이트를 수행. 클라에서 매틱 호출해야함.
    void SceneFinalUpdate();

private:
    void ObjectsTransformFlagReset();       //Trnasofrm의 DirtyFlag를 초기화합니다.
    void ObjectsPrevFrameEnableUpdate();    //이전 프레임의 Enable 여부를 갱신합니다.

private:
    /*게임오브젝트의 Life cycle 수행 여부를 확인하는 함수*/
    bool IsRuntimeActive(std::shared_ptr<GameObject>& obj);

    /*
        Awake 랑 Start 호출 대기중인 컴포넌트를 찾아서 대기열에서 삭제합니다.
        호출 전에 파괴되는 컴포넌트를 위해 존재하는 함수입니다.
    */
    void NotInitDestroyComponentEraseToWaitVec(Component* destroyComponent);

    /// <summary>
    /// 게임 오브젝트를 이름 맵에 추가합니다. 
    /// </summary>
    /// <param name="pInsertObject"></param>
    bool InsertGameObjectMap(std::shared_ptr<GameObject>& pInsertObject);

    /// <summary>
    /// 게임 오브젝트의 맵에서 제거합니다.
    /// </summary>
    void EraseGameObjectMap(std::shared_ptr<GameObject>& pEraseObject);

    /// <summary>
    /// 자식까지 모두 포함한 오브젝트들을 삭제 대기열에 추가합니다.
    /// </summary>
    void AddDestroyObjectQueue(GameObject* gameObject);

    /// <summary>
    /// 컴포넌트를 파괴 대기열에 추가합니다.
    /// </summary>
    void AddDestroyComponentQueue(Component* component);

    /// <summary>
    /// 오브젝트의 OwnerScene을 변경합니다.
    /// </summary>
    void SetObjectOwnerScene(GameObject* object, std::string_view sceneName);

    /// <summary>
    /// 스카이박스를 변경합니다.
    /// </summary>
    /// <param name="scene"></param>
    void SetRendererSkyBox(Scene* scene);

private:
    //Life cycle 에 포함되는 실제 오브젝트들 항목
    std::vector<std::shared_ptr<GameObject>> _runtimeObjects;

    //오브젝트 이름과 포인터로 관리하는 map
    std::unordered_map<std::string, std::unordered_set<std::shared_ptr<GameObject>>> _runtimeObjectsUnorderedMap;

    //오브젝트 태그 추적용 unordered_map
    std::unordered_map<std::string, std::unordered_set<GameObject*>> _runtimeObjectsTagMap;

    //오브젝트 추가 대기열 
    std::vector<std::shared_ptr<GameObject>> _addGameObjectsQueue;
    std::vector<std::pair<std::weak_ptr<GameObject>, std::shared_ptr<Component>>> _addComponentsQueue;

    //오브젝트 삭제 대기열
    std::pair<std::unordered_set<GameObject*>, std::vector<GameObject*>> _destroyObjectsQueue;
    std::vector<GameObject*>                                             _destroyObjectTemp;
    std::pair<std::unordered_set<Component*>, std::vector<Component*>>   _destroyComponentsQueue;
    std::vector<Component*>                                              _destroyComponentsTemp;

    //초기화 함수 호출 대기열
    std::vector<std::shared_ptr<Component>> _waitAwakeVec;
    std::vector<std::shared_ptr<Component>> _waitStartVec;

    //OnEnable, OnDisable을 set과 같이 관리
    using OnComponentQueue =  std::pair<std::unordered_set<Component*>, std::vector<std::weak_ptr<Component>>>;   
    OnComponentQueue  _onEnableQueue;
    OnComponentQueue  _onDisableQueue;

    //Scene에 실행중인 Render component들
    std::vector<std::weak_ptr<MeshComponent>> _runtimeMeshComponents;

    //현재 Scene의 MainCamera
    CameraComponent* _mainCamera;

private:
    // 이전에 로드한 씬 이름입니다.
    std::string _prevScene = STR_NULL;
    struct
    {
        // 현재 Single로 로드된 씬 이름입니다. NewGameObject를 하면 이 씬에 오브젝트가 생성됩니다.
        std::string MainScene = STR_NULL;

       //에디터가 아닌 상태에서 처음으로 로드할 씬
       std::string StartScene = STR_NULL;
    } 
    _setting;
    std::function<void()> _loadFuncEvent;

    //생성한 씬을 찾기 위한 맵입니다. key : 파일 확장자를 제외한 파일 이름 
    std::unordered_map<std::string, std::unordered_set<File::Guid>> _scenesFindMap;

    //생성된 씬 맵(GUID)
    std::unordered_map<File::Guid, Scene> _scenesMap;

    //로드된 씬 항목
    std::vector<Scene*> _lodedSceneList;

    //다음에 로드할 씬
    File::Guid _nextSceneGuid;

    //다음에 로드할 스카이박스
    Scene* _nextSceneSkybox;

    bool _waitResourceLoad = false;
    bool _checkResourceLoad = false;

protected:
    /// <summary>
    /// 씬을 Yaml 형식으로 직렬화합니다.
    /// </summary>
    /// <param name="scene :">직렬화할 씬</param>
    /// <returns></returns>
    YAML::Node SerializeToYaml(const Scene& scene);

    /// <summary>
    /// Yaml 형식으로 직렬화된 씬을 로드합니다.
    /// </summary>
    /// <param name="sceneNode :"></param>
    /// <returns></returns>
    bool DeserializeToYaml(YAML::Node* sceneNode);

    /// <summary>
    /// GUID를 통해 파일을 읽어 씬을 로드합니다..
    /// </summary>
    /// <param name="guid">생성할 프리팹 GUID</param>
    /// <returns></returns>
    bool DeserializeToGuid(const File::Guid& guid);

    /// <summary>
    /// RootPath 기준으로 씬 파일을 작성합니다.
    /// </summary>
    /// <param name="sceneName"></param>
    /// <param name="outPath"></param>
    /// <returns></returns>
    bool WriteUmSceneFile(
        Scene& scene, 
        std::string_view sceneName, 
        std::string_view outPath,
        bool isOverride = false,
        bool isEmptyScene = false);

    // FileEventSubscriber을(를) 통해 상속됨
    virtual void OnFileRegistered(const File::Path& path) override;
    virtual void OnFileUnregistered(const File::Path& path) override;
    virtual void OnFileModified(const File::Path& path) override;
    virtual void OnFileRemoved(const File::Path& path) override;
    virtual void OnFileRenamed(const File::Path& oldPath, const File::Path& newPath) override;
    virtual void OnFileMoved(const File::Path& oldPath, const File::Path& newPath) override;

    virtual void OnRequestedOpen(const File::Path& path);
    virtual void OnRequestedCopy(const File::Path& path);
    virtual void OnRequestedPaste(const File::Path& path);

    virtual void OnRequestedSave() override;
    virtual void OnRequestedLoad() override;
    virtual void OnPostRequestedLoad() override;

    //씬 이름 변경시
    void RenameScene(Scene& scene, std::string_view oldName, std::string_view newName);

    //씬 오너 이름 변경시
    void ResetOwnerScene(std::string_view oldName, std::string_view newName);

    //메인 씬 변경 체크
    void CheckMainSceneRename(Scene& renameScene, const File::Path& newPath);

    //관리하는 씬 파일 파괴시 호출
    void EraseSceneGUID(std::string_view sceneName, const File::Guid guid);

    //직렬화된 씬들 캐싱용
    std::unordered_map<File::Guid, YAML::Node> _sceneDataMap;

public:
    //커맨드 모음
    friend class Command::EditorScene::NewGameObjectCommand;
    friend class Command::EditorScene::DestroyGameObjectCommand;
    friend class Command::EditorScene::AddComponentCommand;
    friend class Command::EditorScene::DestroyComponentCommand;
    friend class Command::EditorScene::DuplicateCommand;
    friend class Command::EditorScene::PasteObjectCommand;
};

inline auto ESceneManager::GetRootGameObjectsByPath(std::string_view path) 
{
    const auto& objectsList = ESceneManager::Engine::GetRuntimeObjects();
    std::string scenePath = path.data();
    auto rootObjects = objectsList | std::ranges::views::filter(
    [scenePath](auto& obj)
    {
        if (obj == nullptr)
            return false;

        return RootGameObjectsFilter(obj.get(), scenePath);
    });
    return rootObjects;
}

inline auto Scene::GetRootGameObjects() const
{
    std::string path;
    if (_isDontDestroyOnLoad)
    {
        path = ESceneManager::DONT_DESTROY_ON_LOAD_SCENE_NAME;
    }
    else
    {
        path = Path;
    }
    return ESceneManager::GetRootGameObjectsByPath(path);
}

