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

std::shared_ptr<GameObject> EGameObjectFactory::MakeGameObject(std::string_view typeid_name)
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
    ownerObject->ReflectionFields->_name = name;
    ownerObject->ReflectionFields->_isStatic = false;
    ownerObject->ReflectionFields->_activeSelf = true;
  
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

void EGameObjectFactory::Engine::ReturnInstanceID(int id)
{
    std::vector<int>& emptyID = engineCore->GameObjectFactory.instanceIDManager.EmptyID;
    emptyID.push_back(id);
    std::sort(emptyID.begin(), emptyID.end(), [](int a, int b)
        {
            return a > b; //내림차순 정렬
        });
}
