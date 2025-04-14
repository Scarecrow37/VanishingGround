#include "pch.h"
#include "EditorMenuBar.h"

EditorMenuBar::EditorMenuBar()
{
    _root = new EditorMenuNode;
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
        if (itr->second->IsNode())
        {
            if (true == itr->second->IsNode())
                return static_cast<EditorMenuNode*>(itr->second.get());
        }
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
            _nodeTable[curPath].reset(new EditorMenuNode);
            auto& instance = _nodeTable[curPath];
            instance->SetLabel(curPath.string());

            EditorMenuNode* parent = GetMenuFromPath(curPath.parent_path());
            parent->_MenuNodeVec.push_back(instance.get());

            Sort(parent);
        }
    }

    return _nodeTable[path].get();
}

void EditorMenuBar::Sort(EditorMenuNode* root)
{
    // root->_MenuNodeVec의 컨테이너에서 EditorMenu::GetCallOrder값을 통해 내림차순으로 정렬한다.
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
        if (true == instance->IsNode())
        {
            Sort(static_cast<EditorMenuNode*>(instance));
        }
    }
}

void EditorMenuNode::OnDrawGui()
{
    if (GetVisible())
    {
        ImGui::PushID(this);

        std::string label = GetLabel();
        if (ImGui::BeginMenu(label.c_str(), GetActive()))
        {
            for (auto& node : _MenuNodeVec)
            {
                if(nullptr != node)
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

