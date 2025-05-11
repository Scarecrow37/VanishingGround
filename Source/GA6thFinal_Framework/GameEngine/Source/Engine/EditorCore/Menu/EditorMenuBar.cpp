#include "pch.h"
#include "EditorMenuBar.h"

EditorMenuBar::EditorMenuBar()
{
    _root = new EditorMenuNode();
    _root->SetPath("");
    _nodeTable[_root->GetPath()].reset(_root);
}

EditorMenuBar::~EditorMenuBar()
{
    _nodeTable.clear();
    _menuTable.clear();
}

void EditorMenuBar::OnTickGui()
{
     _root->OnTickGui();
}

void EditorMenuBar::OnStartGui()
{
    _root->OnStartGui();
}

void EditorMenuBar::OnDrawGui()
{
    ImGui::BeginMainMenuBar();

    _root->OnDrawGui();

    ImGui::EndMainMenuBar();
}

void EditorMenuBar::OnEndGui()
{
    _root->OnEndGui();
}

/*
인자로 받은 경로에 따라 메뉴Node를 만들고 최상위 부모Node를 반환합니다.
*/
EditorMenuNode* EditorMenuBar::BuildMenuNode(Path path)
{
    if (true == path.empty())
        return nullptr;

    Path curPath; // 현재 진행 중인 경로
    for (auto entry : path)
    {
        curPath /= entry;
        curPath = curPath.generic_string();

        auto itr = _nodeTable.find(curPath);
        if (itr == _nodeTable.end())
        {
            EditorMenuNode* instance = new EditorMenuNode();
            instance->SetPath(curPath.string());
            _nodeTable[curPath].reset(instance);
            //instance->SetLabel(curPath.filename().string());

            EditorMenuNode* parent = GetMenuFromPath(curPath.parent_path());
            parent->_menuList[""].push_back(instance);

            Sort(parent);
        }
    }

    return _nodeTable[path].get();
}

void EditorMenuBar::Sort(EditorMenuNode* root)
{
    //if (root->_menuList.empty())
    //    return;
    //
    //for (auto& [key, group] : root->_menuList)
    //{
    //    if (group.empty())
    //        continue;
    //
    //    std::sort(group.begin(), group.end(),
    //              [](EditorGui* a, EditorGui* b) { return a->GetCallOrder() < b->GetCallOrder(); });
    //
    //    for (auto& node : group)
    //    {
    //        auto* menuNode = dynamic_cast<EditorMenuNode*>(node);
    //        if (menuNode)
    //        {
    //            Sort(menuNode);
    //        }
    //    }
    //}
}

EditorMenuNode* EditorMenuBar::GetMenuFromPath(Path path)
{
    auto itr = _nodeTable.find(path);
    // 없으면 만들어 줌
    if (itr != _nodeTable.end())
    {
        return itr->second.get();
    }
    return nullptr;
}

EditorMenuBase::EditorMenuBase()
    : _isActive(true), _path("")
{

}

EditorMenuBase::~EditorMenuBase() 
{
}

void EditorMenuBase::DefaultDebugFrame()
{
    //static char tooltip[256];
    //
    //snprintf(tooltip, sizeof(tooltip), "GuiID: 0x%08X\nOrder: %d\nPath: %s\nLabel: %s",
    //    ImGui::GetID(""),
    //    GetPath().c_str(), 
    //);
    //
    //if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
    //{
    //    ImGui::BeginTooltip();
    //    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    //    ImGui::TextUnformatted(tooltip);
    //    ImGui::PopTextWrapPos();
    //    ImGui::EndTooltip();
    //}
}

EditorMenuNode::EditorMenuNode()
{
}

EditorMenuNode::~EditorMenuNode() 
{
}

void EditorMenuNode::OnTickGui() 
{
    for (auto itr = _menuList.begin(); itr != _menuList.end(); ++itr)
    {
        auto& [key, group] = *itr;

        for (auto& node : group)
        {
            node->OnTickGui();
        }
    }
}

void EditorMenuNode::OnStartGui() 
{
    for (auto itr = _menuList.begin(); itr != _menuList.end(); ++itr)
    {
        auto& [key, group] = *itr;

        for (auto& node : group)
        {
            node->OnStartGui();
        }
    }
}

void EditorMenuNode::OnDrawGui()
{
}

void EditorMenuNode::OnEndGui() 
{
    for (auto itr = _menuList.begin(); itr != _menuList.end(); ++itr)
    {
        auto& [key, group] = *itr;

        for (auto& node : group)
        {
            node->OnEndGui();
        }
    }
}

EditorMenu::EditorMenu() 
{
}

EditorMenu::~EditorMenu() 
{
}

void EditorMenu::OnDrawGui()
{
    if (true == IsVisible())
    {
        ImGui::PushID(this);

        OnMenu();

        if (true == Global::editorModule->IsDebugMode())
        {
            DefaultDebugFrame();
        }

        ImGui::PopID();
    }
}
