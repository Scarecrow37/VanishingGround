#include "pch.h"
#include "EditorGuiSystem.h"

EditorGuiSystem::EditorGuiSystem() 
{
}

EditorGuiSystem::~EditorGuiSystem() 
{
    Clear();
}

void EditorGuiSystem::OnTickGui() 
{
    for (auto& window : _dockWindowList)
    {
        if (window)
        {
            window->OnTickGui();
        }
    }
}

void EditorGuiSystem::OnStartGui() 
{
    for (auto& window : _dockWindowList)
    {
        if (window)
        {
            window->OnStartGui();
        }
    }
}

void EditorGuiSystem::OnDrawGui() 
{
    for (auto& window : _dockWindowList)
    {
        if (window)
        {
            if (window->IsVisible())
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
        if (window)
        {
            window->OnEndGui();
        }
    }
}

EditorDockWindow* EditorGuiSystem::CreateDockWindow(const char* label, const char* parentLabel)
{
    DockID id       = ImHashStr(label);
    bool   contains = _dockWindowTable.contains(id);
    if (false == contains)
    {
        EditorDockWindow* dock = new EditorDockWindow();
        dock->SetLabel(label);
        _dockWindowTable[id] = dock;
        if (parentLabel)
        {
            if (EditorDockWindow* parent = GetDockWindow(parentLabel))
            {
                parent->RegisterChildDockWindow(dock);
            }
        }
        else
        {
            _dockWindowList.push_back(dock);
        }
        return dock;
    }
    return nullptr;
}

EditorDockWindow* EditorGuiSystem::GetDockWindow(const char* label) const
{
    DockID id = ImHashStr(label);
    auto   it = _dockWindowTable.find(id);
    if (_dockWindowTable.end() == it)
    {
        return nullptr;
    }
    else
    {
        return it->second;
    }
}

EditorDockWindow* EditorGuiSystem::operator[](const char* label) const
{
    return GetDockWindow(label);
}

void EditorGuiSystem::Clear()
{
    for (auto& gui : _dockWindowList)
    {
        if (gui)
        {
            delete gui;
            gui = nullptr;
        }
    }
    _dockWindowList.clear();
    _dockWindowTable.clear();
}

void EditorGuiSystem::ResetLayout()
{
    for (auto& [label, window] : _dockWindowTable)
    {
        if (nullptr != window)
        {
            window->RequestBuildDockLayout();
        }
    }
}

YAML::Node EditorGuiSystem::SaveGuiSettingToMemory()
{
    YAML::Node rootNode;

    for (auto& [label, dock] : _dockWindowTable)
    {
        if (dock)
        {
            auto& table = dock->GetRefToolTable();

            YAML::Node dockNode;
            
            for (auto& [className, tool] : table)
            {
                if (tool)
                {
                    dockNode[className] = tool->SerializedReflectFields();
                }
            }
            dockNode["Fields"] = dock->SerializedReflectFields();

            rootNode[label] = dockNode;
        }
    }

    return rootNode;
}

void EditorGuiSystem::LoadGuiSettingFromMemory(YAML::Node node)
{
    for (auto& [label, dock] : _dockWindowTable)
    {
        if (dock)
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
                if (tool && dockNode[className])
                {
                    std::string serializeData = dockNode[className].as<std::string>();
                    tool->DeserializedReflectFields(serializeData);
                }
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
