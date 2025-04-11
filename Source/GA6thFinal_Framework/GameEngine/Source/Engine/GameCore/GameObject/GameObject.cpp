#include "pch.h"
using namespace Global;

void GameObject::DontDestroyOnLoad(GameObject& gameObject)
{
    ESceneManager::Engine::DontDestroyOnLoadObject(gameObject);
}

void GameObject::Instantiate(GameObject& gameObject)
{

}

void GameObject::Destroy(Component& component, float t)
{
    ESceneManager::Engine::DestroyObject(component);
}

void GameObject::Destroy(GameObject& gameObject, float t)
{
    ESceneManager::Engine::DestroyObject(gameObject);
}

GameObject::GameObject()
    : 
    transform(*this),
    _ownerScene("null"),
    _components(),
    _instanceID(-1)
{

}

GameObject::~GameObject()
{
    _components.clear();
}

Scene& GameObject::GetScene()
{
    return *engineCore->SceneManager.GetSceneByName(_ownerScene);
}


