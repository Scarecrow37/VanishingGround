#include "pchScripts.h"
#include "MapManagerTracker.h"
#include "MapManager.h"

UMREAL_COMPONENT(MapManagerTracker)

MapManagerTracker::MapManagerTracker() = default;
MapManagerTracker::~MapManagerTracker() = default;

void MapManagerTracker::Awake() 
{
    Base::Awake();
    if (GameObject* manager = SingletonObject<MapManager>::GetInstance())
    {
        if (manager->ActiveInHierarchy)
        {
            GameObject::DontDestroyOnLoad(gameObject);
            transform->SetParent(manager->transform);
        }
    }
}
