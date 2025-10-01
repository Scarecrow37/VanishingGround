#include "pchScripts.h"
#include "MapManagerTracker.h"
#include "MapManager.h"

UMREAL_COMPONENT(MapManagerTracker)

MapManagerTracker::MapManagerTracker() = default;
MapManagerTracker::~MapManagerTracker() = default;

void MapManagerTracker::Start() 
{
    Base::Start();
    if (MapManager* manager = SingletonComponent<MapManager>::GetInstance())
    {
        if (manager->EnableInHierarchy)
        {
            GameObject::DontDestroyOnLoad(gameObject);
            transform->SetParent(manager->transform);
        }
    }
}
