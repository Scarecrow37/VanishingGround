#pragma once
class GameObject;

template<typename T>
concept IS_BASE_GAMEOBJECT_C = std::is_base_of_v<GameObject, T>;

class EGameObjectFactory : public File::FileEventNotifier
{
    friend class EngineCores;
private:
    EGameObjectFactory();
    ~EGameObjectFactory();

#ifndef SCRIPTS_PROJECT
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
    struct Engine 
    {
        //SceneManager에서 오브젝트를 Destroy 할때 Instance ID를 반납하기 위한 함수입니다.
        static void ReturnInstanceID(int id);

        /// <summary>
        /// 모든 게임 오브젝트의 키들을 반환합니다.
        /// </summary>
        /// <returns></returns>
        static const std::vector<std::string>& GetGameObjectKeys();

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
    bool DeserializeToYaml(YAML::Node* gameObjectNode);

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
};
