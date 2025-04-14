#include "pch.h"
#include "EditorMenuBar.h"

EditorMenuBar::EditorMenuBar()
{
    _root = new EditorMenuNode("");
    _nodeTable[""].reset(_root);
}

EditorMenuBar::~EditorMenuBar()
{
    _nodeTable.clear();
    _menuTable.clear();
}

void EditorMenuBar::OnTickGui()
{
}

void EditorMenuBar::OnStartGui()
{
    for (auto& [key, menu] : _menuTable)
    {
        if (nullptr != menu)
            menu->OnStartGui();
    }
}

void EditorMenuBar::OnDrawGui()
{
    ImGui::BeginMainMenuBar();

    for (auto& [key, menu] : _menuTable)
    {
        if (nullptr != menu)
            menu->OnTickGui();
    }

    for (auto& node : _root->_MenuNodeVec)
    {
        if(nullptr != node)
            node->OnDrawGui();
    }

    ImGui::EndMainMenuBar();
}

void EditorMenuBar::OnEndGui()
{
    for (auto& [key, menu] : _menuTable)
    {
        if (nullptr != menu)
            menu->OnEndGui();
    }
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
            EditorMenuNode* instance = new EditorMenuNode(curPath.filename().string());
            _nodeTable[curPath].reset(instance);
            instance->SetLabel(curPath.string());

            EditorMenuNode* parent = GetMenuFromPath(curPath.parent_path());
            parent->_MenuNodeVec.push_back(instance);

            Sort(parent);
        }
    }

    return _nodeTable[path].get();
}

void EditorMenuBar::Sort(EditorMenuNode* root)
{
    if (root->_MenuNodeVec.empty())
        return;

    std::sort(
        root->_MenuNodeVec.begin(),
        root->_MenuNodeVec.end(),
              [](EditorMenuBase* a, EditorMenuBase* b) {
            return a->GetCallOrder() > b->GetCallOrder();
        });

    for (auto& instance : root->_MenuNodeVec)
    {
        auto* node = dynamic_cast<EditorMenuNode*>(instance);
        if (node)
        {
            Sort(node);
        }
    }
}

EditorMenuNode::EditorMenuNode(std::string_view name)
    : _name(name)
{
}

EditorMenuNode::~EditorMenuNode() 
{
}

void EditorMenuNode::OnDrawGui()
{
    if (GetVisible())
    {
        ImGui::PushID(this);

        if (ImGui::BeginMenu(_name.c_str(), GetActive()))
        {
            for (auto& node : _MenuNodeVec)
            {
                node->OnDrawGui();
            }
            ImGui::EndMenu();
        }
        ImGui::PopID();
    }
}
void EditorMenu::OnDrawGui()
{
    OnTickGui();

    if (true == GetVisible())
    {
        ImGui::PushID(this);

        OnMenu();

        ImGui::PopID();
    }
}

