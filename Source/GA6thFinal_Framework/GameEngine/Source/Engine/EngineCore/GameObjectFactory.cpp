#include "pch.h"
using namespace Global;

EGameObjectFactory::EGameObjectFactory()
{
    //엔진에 존재하는 모든 오브젝트 타입들을 등록해야합니다.
    RegisterGameObject<GameObject>();

}

EGameObjectFactory::~EGameObjectFactory() = default;

std::shared_ptr<GameObject> EGameObjectFactory::NewGameObject(std::string_view typeid_name, std::string_view name)
{
    auto sptr_object = MakeGameObject(typeid_name);
    ResetGameObject(sptr_object.get(), name);
    ESceneManager::Engine::AddGameObjectToLifeCycle(sptr_object);
    return sptr_object;
}

YAML::Node EGameObjectFactory::SerializeToYaml(GameObject* gameObject)
{
    YAML::Node nodes;
    std::map<Transform*, int> transformParentLevelMap;
    int parentIndex = 0;
    Transform::ForeachBFS(
        gameObject->transform, 
        [&](Transform* curr) 
        {
            YAML::Node objectNode = MakeYamlToGameObject(&curr->gameObject);
            transformParentLevelMap[curr] = parentIndex;
            YAML::Node transformNode = objectNode["Transform"].as<YAML::Node>();
            transformNode["TransformIndex"] = parentIndex;
            if (curr->Parent != nullptr)
            {
                transformNode["ParentIndex"] = transformParentLevelMap[curr->Parent];
            }           
            ++parentIndex;
            nodes.push_back(objectNode);
        });
    return nodes;
}

bool EGameObjectFactory::DeserializeToYaml(YAML::Node* pGameObjectNode)
{
    YAML::Node& nodes = *pGameObjectNode;
    std::map<int, Transform*> transformParentLevelMap;
    for (auto node : nodes)
    {
        YAML::Node& currNode = node;
        YAML::Node transformNode = currNode["Transform"].as<YAML::Node>();
        std::shared_ptr<GameObject> gameObject = MakeGameObjectToYaml(&currNode);

        int TransformIndex = transformNode["TransformIndex"].as<int>();
        transformParentLevelMap[TransformIndex] = &gameObject->transform;
        if (transformNode["ParentIndex"])
        {
            int        ParentIndex = transformNode["ParentIndex"].as<int>();
            Transform* pParent     = transformParentLevelMap[ParentIndex];
            gameObject->transform.SetParent(pParent);
        }
        ESceneManager::Engine::AddGameObjectToLifeCycle(gameObject);
    }
    return true;
}

std::shared_ptr<GameObject> EGameObjectFactory::MakeGameObject(
    std::string_view typeid_name)
{
    std::shared_ptr<GameObject> newObject;
    auto findIter = _NewGameObjectFuncMap.find(typeid_name.data());
    if (findIter != _NewGameObjectFuncMap.end())
    {
        auto& [key, NewObjectFunc] = *findIter;
        newObject.reset(NewObjectFunc());
    }
    return newObject;
}

void EGameObjectFactory::ResetGameObject(
    GameObject* ownerObject, 
    std::string_view name)
{
    ownerObject->_ownerScene = engineCore->SceneManager.GetMainScene().name;
    ownerObject->ReflectFields->_name = name;
    ownerObject->ReflectFields->_isStatic = false;
    ownerObject->ReflectFields->_activeSelf = true;
  
    //인스턴스 아이디 부여
    int instanceID = -1;
    if (instanceIDManager.EmptyID.empty())
    {
        instanceID = instanceIDManager.BackID++;
    }
    else
    {
        instanceID = instanceIDManager.EmptyID.back();
        instanceIDManager.EmptyID.pop_back();
    }
    ownerObject->_instanceID = instanceID;
}

YAML::Node EGameObjectFactory::MakeYamlToGameObject(GameObject* gameObject)
{
    if (typeid(*gameObject) == typeid(GameObject))
    {
        YAML::Node objectNode;
        objectNode["SerializeVersion"] = 0;
        objectNode["Type"]             = typeid(GameObject).name();

        objectNode["ReflectFields"] = gameObject->SerializedReflectFields();
        {
            YAML::Node transformNode;
            transformNode["ReflectFields"] =
                gameObject->transform.SerializedReflectFields();
            objectNode["Transform"] = transformNode;
        }
        return objectNode;
    }
    else
    {
#ifdef _DEBUG
        assert(!"유효하지 않는 오브젝트 타입입니다.");
#else   
        __debugbreak();
#endif // _DEBUG
        return YAML::Node();
    }  
}

std::shared_ptr<GameObject> EGameObjectFactory::MakeGameObjectToYaml(YAML::Node* pObjectNode)
{
    YAML::Node& objectNode = *pObjectNode;

    const int SerializeVersion = objectNode["SerializeVersion"].as<int>();
    std::string Type           = objectNode["Type"].as<std::string>();

    std::shared_ptr<GameObject> object = MakeGameObject(Type);
    ResetGameObject(object.get(), "null");

    std::string ReflectFields = objectNode["ReflectFields"].as<std::string>();
    object->DeserializedReflectFields(ReflectFields);
    {
        YAML::Node transformNode = objectNode["Transform"].as<YAML::Node>();
        std::string ReflectFields = transformNode["ReflectFields"].as<std::string>();
        object->transform.DeserializedReflectFields(ReflectFields);
    }


    return object;
}

void EGameObjectFactory::Engine::ReturnInstanceID(int id)
{
    std::vector<int>& emptyID = engineCore->GameObjectFactory.instanceIDManager.EmptyID;
    emptyID.push_back(id);
    std::sort(emptyID.begin(), emptyID.end(), [](int a, int b)
        {
            return a > b; //내림차순 정렬
        });
}

const std::vector<std::string>& EGameObjectFactory::Engine::GetGameObjectKeys()
{
    return engineCore->GameObjectFactory._NewGameObjectKeyVec;
}
