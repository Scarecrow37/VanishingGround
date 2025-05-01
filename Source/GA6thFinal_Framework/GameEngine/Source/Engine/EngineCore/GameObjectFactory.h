#pragma once
class GameObject;
class Transform;

template<typename T>
concept IS_BASE_GAMEOBJECT_C = std::is_base_of_v<GameObject, T>;

class EGameObjectFactory : public File::FileEventNotifier
{
    friend class EngineCores;
private:
    EGameObjectFactory();
    ~EGameObjectFactory();

#ifndef _SCRIPTS_PROJECT
    /// <summary>
    /// 게임 오브젝트 클래스를 팩토리에서 생성 가능하도록 등록합니다. 생성자에서 호출 해야합니다.
    /// </summary>
    /// <typeparam name="TGameObject :">등록할 타입 파라미터</typeparam>
    template <IS_BASE_GAMEOBJECT_C TGameObject>
    inline void RegisterGameObject()
    {
        const char* key      = typeid(TGameObject).name();
        auto        findIter = _NewGameObjectFuncMap.find(key);
        if (findIter == _NewGameObjectFuncMap.end())
        {
            _NewGameObjectFuncMap[key] = [] { return new TGameObject; };
            _NewGameObjectKeyVec.emplace_back(key);
        }
        else
        {
            assert(!"This key is already registered.");
        }
    }
#endif 
public:
    static constexpr const char* PREFAB_EXTENSION = ".UmPrefab";
    struct Engine 
    {
        //SceneManager에서 오브젝트를 Destroy 할때 Instance ID를 반납하기 위한 함수입니다.
        static void ReturnInstanceID(int id);

        /// <summary>
        /// 모든 게임 오브젝트의 키들을 반환합니다.
        /// </summary>
        /// <returns></returns>
        static const std::vector<std::string>& GetGameObjectKeys();

        /// <summary>
        /// Prefab FileEventNotifier를 등록합니다. 
        /// </summary>
        static void RegisterFileEvents();
    };
    /// <summary>
    /// 게임 오브젝트를 생성합니다. 생성된 오브젝트는 자동으로 씬에 등록됩니다.
    /// </summary>
    /// <param name="typeid_name :">생성할 오브젝트 typeid().name()</param>
    /// <param name="name :">오브젝트 이름</param>
    std::shared_ptr<GameObject> NewGameObject(
        std::string_view typeid_name,
        std::string_view name);

    /// <summary>
    /// 게임 오브젝트를 Yaml 형식으로 직렬화합니다.
    /// </summary>
    /// <param name="gameObject :">직렬화할 오브젝트</param>
    /// <returns></returns>
    YAML::Node SerializeToYaml(GameObject* gameObject);

    /// <summary>
    /// Yaml 형식으로 직렬화된 오브젝트를 씬에 추가합니다.
    /// </summary>
    /// <param name="node"></param>
    /// <returns></returns>
    std::shared_ptr<GameObject> DeserializeToYaml(YAML::Node* gameObjectNode);

    /// <summary>
    /// GUID를 통해 파일을 읽어 오브젝트를 씬에 추가합니다.
    /// </summary>
    /// <param name="guid">생성할 프리팹 GUID</param>
    /// <returns></returns>
    std::shared_ptr<GameObject> DeserializeToGuid(const File::Guid& guid);

    /// <summary>
    /// 게임 오브젝트를 UmPrefab파일로 저장합니다. FileSystem의 RootPath 기준으로 저장합니다. 
    /// </summary>
    /// <param name="transform"></param>
    /// <param name="outPath"></param>
    void WriteGameObjectFile(Transform* transform, std::string_view outPath);

    /// <summary>
    /// 게임 오브젝트를 프리팹 인스턴스로 만듭니다.
    /// </summary>
    /// <param name="object"></param>
    /// <returns></returns>
    bool PackPrefab(GameObject* object, const File::Guid& guid);

    /// <summary>
    /// <para> 프리팹 원본 객체를 가져옵니다. 계층 구조가 BFS 순으로 담아져 있습니다. </para>
    /// <para> 실패시 nullptr을 반환합니다. </para>
    /// </summary>
    /// <param name="guid :">가져올 프리팹 GUID</param>
    /// <returns>해당 GUID의 프리팹</returns>
    const std::vector<std::shared_ptr<GameObject>>* GetOriginPrefab(const File::Guid& guid);

    /// <summary>
    /// 프리팹 인스턴스를 일반 게임 오브젝트로 변경합니다.
    /// </summary>
    /// <returns></returns>
    bool UnpackPrefab(GameObject* object);

    /// <summary>
    /// 필드 오버라이드 여부를 확인합니다. 
    /// </summary>
    /// <param name="pFiled :">확인할 맴버 변수의 주소</param>
    /// <returns>해당 주소의 오버라이드 플래그 여부</returns>
    bool IsOverrideField(void* pField);

    /// <summary>
    /// 필드 오버라이드 여부를 설정합니다.
    /// </summary>
    /// <param name="pFiled :">설정할 맴버 변수의 주소</param>
    /// <returns>결과</returns>
    bool SetOverrideFlag(void* pField);

    /// <summary>
    /// 필드 오버라이드 여부를 초기화합니다.
    /// </summary>
    /// <param name="pFiled :">초기화할 맴버 변수의 주소</param>
    /// <returns>결과</returns>
    bool UnsetOverrideFlag(void* pField);



private:
    //컴포넌트를 동적할당후 shared_ptr로 반환합니다.
    //매개변수로 생성할 컴포넌트 typeid().name()을 전달해야합니다.
    std::shared_ptr<GameObject> MakeGameObject(std::string_view typeid_name);

    //게임오브젝트를 엔진에 사용하기 위해 초기화합니다.
    void ResetGameObject(
        GameObject* ownerObject,
        std::string_view name);
    
   //게임 오브젝트를 Yaml로 반환
   YAML::Node MakeYamlToGameObject(GameObject* gameObject);

   //Yaml을 오브젝트로 반환. Reset도 해줌.
   std::shared_ptr<GameObject> MakeGameObjectToYaml(YAML::Node* objectNode);

   //게임 오브젝트를 YAML로 초기화
   void ParsingYaml(GameObject* pObject, YAML::Node& objectNode);

   //오브젝트 계층구조를 포함한 Yaml 직렬화 데이터로 GameObject들을 만들어서 반환합니다.
   std::vector<std::shared_ptr<GameObject>> MakeObjectsGraphToYaml(YAML::Node* pObjectNode, bool useResource = false);

   void RegisterGameObjects();
private:
    std::map<std::string, std::function<GameObject* ()>> _NewGameObjectFuncMap;    //생성용 맵
    std::vector<std::string>                             _NewGameObjectKeyVec;     //키 항목 모음

    //인스턴스 아이디를 추적하기 위한 맴버
    struct
    {
        int BackID = 0;
        std::vector<int> EmptyID;
    }
    instanceIDManager;
private:
    //Prefab의 GUID만 다시 작성합니다.
    void WritePrefabGuid(const File::Path& path, YAML::Node& data);

    // FileEventNotifier을(를) 통해 상속됨
    void OnFileRegistered(const File::Path& path) override;
    void OnFileUnregistered(const File::Path& path) override;
    void OnFileModified(const File::Path& path) override;
    void OnFileRemoved(const File::Path& path) override;
    void OnFileRenamed(const File::Path& oldPath, const File::Path& newPath) override;
    void OnFileMoved(const File::Path& oldPath, const File::Path& newPath) override;

    void OnRequestedOpen(const File::Path& path) override {}
    void OnRequestedCopy(const File::Path& path) override {}
    void OnRequestedPaste(const File::Path& path) override {}

    //프리팹 오브젝트 모음
    std::unordered_map<File::Guid, std::vector<std::shared_ptr<GameObject>>> _prefabObjectMap;
     
    //프리팹 인스턴스 GUID 등록 대기용 큐
    std::unordered_map<File::Path, std::vector<std::weak_ptr<GameObject>>> _prefabGuidQueue;

    //인스턴스화된 프리팹 추적용
    std::unordered_map<File::Guid, std::vector<std::weak_ptr<GameObject>>> _prefabInstanceList;     

    //프리팹 인스턴스 ovrride 추적용
    std::unordered_set<void*> _prefabInstanceOverride;

};
