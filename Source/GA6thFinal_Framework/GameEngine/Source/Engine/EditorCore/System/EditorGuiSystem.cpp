#include "pch.h"
#include "EditorGuiSystem.h"

EditorGuiSystem::EditorGuiSystem() 
{
}

EditorGuiSystem::~EditorGuiSystem() 
{
    for (auto& window : _dockWindowList)
    {
        if (nullptr != window)
        {
            delete window;
            window = nullptr;
        }
    }
    _dockWindowList.clear();
    _dockWindowTable.clear();
}

void EditorGuiSystem::OnTickGui() 
{
    for (auto& window : _dockWindowList)
    {
        if (nullptr != window)
        {
            window->OnTickGui();
        }
    }
}

void EditorGuiSystem::OnStartGui() 
{
    for (auto& window : _dockWindowList)
    {
        if (nullptr != window)
        {
            window->OnStartGui();
        }
    }
}

void EditorGuiSystem::OnDrawGui() 
{
    for (auto& window : _dockWindowList)
    {
        if (nullptr != window)
        {
            bool isOpen = window->IsVisible();
            if (true == isOpen)
            {
                window->OnDrawGui();
            }
        }
    }
}

void EditorGuiSystem::OnEndGui() 
{
    for (auto& window : _dockWindowList)
    {
        if (nullptr != window)
        {
            window->OnEndGui();
        }
    }
}

EditorDockWindow* EditorGuiSystem::RegisterDockWindow(const std::string& label, EditorDockWindow* parent)
{
    EditorDockWindow* instance;
    auto itr = _dockWindowTable.find(label);
    if (itr == _dockWindowTable.end())
    {
        instance = new EditorDockWindow;
        instance->SetLabel(label.c_str());
        _dockWindowTable[label] = instance;
        if (nullptr != parent)
        {
            parent->RegisterChildDockWindow(instance);
        }
        else
        {
            _dockWindowList.push_back(instance);
        }
    }
    else
    {
        instance = itr->second;
    }
    return instance;
}

EditorDockWindow* EditorGuiSystem::GetDockWindow(const std::string& label)
{
    auto itr = _dockWindowTable.find(label);
    if (_dockWindowTable.end() == itr)
        return nullptr;
    return itr->second;
}

EditorDockWindow* EditorGuiSystem::operator[](const std::string& label)
{
    return GetDockWindow(label);
}

YAML::Node EditorGuiSystem::SaveGuiSettingToMemory()
{
    YAML::Node rootNode;

    for (auto& [label, dock] : _dockWindowTable)
    {
        auto& table = dock->GetRefToolTable();

        YAML::Node dockNode;
        
        for (auto& [className, tool] : table)
        {
            dockNode[className] = tool->SerializedReflectFields();
        }
        dockNode["Fields"] = dock->SerializedReflectFields();

        rootNode[label] = dockNode;
    }

    return rootNode;
}

void EditorGuiSystem::LoadGuiSettingFromMemory(YAML::Node node)
{
    for (auto& [label, dock] : _dockWindowTable)
    {
        auto& table   = dock->GetRefToolTable();
        auto dockNode = node[label];

        if (dockNode["Fields"])
        {
            std::string serializeData = dockNode["Fields"].as<std::string>();
            dock->DeserializedReflectFields(serializeData);
        }
        for (auto& [className, tool] : table)
        {
            if (dockNode[className])
            {
                std::string serializeData = dockNode[className].as<std::string>();
                tool->DeserializedReflectFields(serializeData);
            }
        }
    }
}

const EditorGuiSystem::DockList& EditorGuiSystem::GetDockWindowList() const
{
    return _dockWindowList;
}

const EditorGuiSystem::DockTable& EditorGuiSystem::GetDockWindowTable() const
{
    return _dockWindowTable;
}
