#include "pch.h"
#include "EditorDockWindowSystem.h"

EditorDockWindowSystem::EditorDockWindowSystem() 
{
}

EditorDockWindowSystem::~EditorDockWindowSystem() 
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

void EditorDockWindowSystem::OnTickGui() 
{
    for (auto& window : _dockWindowList)
    {
        if (nullptr != window)
        {
            window->OnTickGui();
        }
    }
}

void EditorDockWindowSystem::OnStartGui() 
{
    for (auto& window : _dockWindowList)
    {
        if (nullptr != window)
        {
            window->OnStartGui();
        }
    }
}

void EditorDockWindowSystem::OnDrawGui() 
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

void EditorDockWindowSystem::OnEndGui() 
{
    for (auto& window : _dockWindowList)
    {
        if (nullptr != window)
        {
            window->OnEndGui();
        }
    }
}

EditorDockWindow* EditorDockWindowSystem::RegisterDockWindow(const std::string& label, EditorDockWindow* parent)
{
    EditorDockWindow* instance;
    auto itr = _dockWindowTable.find(label);
    if (itr == _dockWindowTable.end())
    {
        instance = new EditorDockWindow;
        instance->SetLabel(label);
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

EditorDockWindow* EditorDockWindowSystem::GetDockWindow(const std::string& label)
{
    auto itr = _dockWindowTable.find(label);
    if (_dockWindowTable.end() == itr)
        return nullptr;
    return itr->second;
}


EditorDockWindow* EditorDockWindowSystem::operator[](const std::string& label)
{
    return GetDockWindow(label);
}

const EditorDockWindowSystem::DockList& EditorDockWindowSystem::GetDockWindowList() const
{
    return _dockWindowList;
}
