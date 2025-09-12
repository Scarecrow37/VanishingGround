#include "pchScripts.h"
#include "AccessorySystem.h"
#include "TurnSystem/TurnAction/TurnActionFactory.h"

AccessorySystem::AccessorySystem()
{

}

AccessorySystem::~AccessorySystem()
{

}

void AccessorySystem::ImGuiDrawPropertysEvent() 
{

}

void AccessorySystem::SerializedReflectEvent() 
{

}

void AccessorySystem::DeserializedReflectEvent() 
{

}

void AccessorySystem::Reset()
{
    _singletonComponent.SetSingleTon();
}

void AccessorySystem::Awake() 
{
    if (_singletonComponent.TrySingleTon())
    {

    }
}

void AccessorySystem::ElementTableSerialized() 
{
    ReflectFields->ElementTableData.clear();
    for (auto& [key, element] : _elementTable)
    {
        ReflectFields->ElementTableData.emplace_back(key, element.SerializedReflectFields());
    }
}

void AccessorySystem::ElementTableDeserialized() 
{
    _elementTable.clear();
    for (auto& [key, data] : ReflectFields->ElementTableData)
    {
        AccessoryElement element;
        element.DeserializedReflectFields(data);
    }
}
