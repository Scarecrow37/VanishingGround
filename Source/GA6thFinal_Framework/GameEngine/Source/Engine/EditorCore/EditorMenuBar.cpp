#include "pch.h"
#include "EditorMenuBar.h"

EditorMenuBar::EditorMenuBar()
{
    _root = new EditorMenuNode;
    _pathTable[""] = _root;
}

EditorMenuBar::~EditorMenuBar()
{
}

void EditorMenuBar::OnTickGui()
{
}

void EditorMenuBar::OnStartGui()
{
    for (auto& node : _root->_MenuNodeVec)
    {
        if (nullptr != node)
            node->OnStartGui();
    }
}

void EditorMenuBar::OnDrawGui()
{
    ImGui::BeginMainMenuBar();

    for (auto& [key, menu] : _nameTable)
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
    for (auto& node : _root->_MenuNodeVec)
    {
        if (nullptr != node)
            node->OnEndGui();
    }
}

EditorMenuNode* EditorMenuBar::GetMenuFromPath(Path path)
{
    auto itr = _pathTable.find(path);
    // 없으면 만들어 줌
    if (itr != _pathTable.end())
    {
        return itr->second;
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

        auto itr = _pathTable.find(curPath);
        if (itr == _pathTable.end())
        {
            EditorMenuNode* parent = GetMenuFromPath(curPath.parent_path());
            EditorMenuNode* instance = new EditorMenuNode;
            instance->SetMenuPath(curPath);

            parent->_MenuNodeVec.push_back(instance);
            _pathTable[curPath] = instance;

            Sort(parent);
        }
    }

    return _pathTable[path];
}

void EditorMenuBar::Sort(EditorMenuNode* root)
{
    // root->_MenuNodeVec의 컨테이너에서 EditorMenu::GetCallOrder값을 통해 내림차순으로 정렬한다.
    if (root->_MenuNodeVec.empty())
        return;

    std::sort(
        root->_MenuNodeVec.begin(),
        root->_MenuNodeVec.end(),
        [](EditorMenu* a, EditorMenu* b) {
            return a->GetCallOrder() > b->GetCallOrder();
        });

    for (auto& node : root->_MenuNodeVec)
    {
        auto* instance = dynamic_cast<EditorMenuNode*>(node);
        if(instance)
            Sort(instance);
    }
}

void EditorMenuNode::OnDrawGui()
{
    if (GetVisible())
    {
        OnPreMenu();

        std::string label = GetMenuPath().filename().string();
        if (ImGui::BeginMenu(label.c_str(), GetActive()))
        {
            OnMenu();

            for (auto& node : _MenuNodeVec)
            {
                if(nullptr != node)
                node->OnDrawGui();
            }
            ImGui::EndMenu();
        }

        OnPostMenu();
    }
}

void EditorMenuNode::Sort()
{
    std::sort(_MenuNodeVec.begin(), _MenuNodeVec.end(),
        [](EditorMenu* a, EditorMenu* b) {
            return a->GetCallOrder() > b->GetCallOrder();
        });
}

void EditorMenuLeaf::OnDrawGui()
{
    if (true == GetVisible())
    {
        if (ImGui::MenuItem(GetLabel().c_str(), GetShortcut().c_str(), GetToggleValue(), GetActive()))
        {
            OnSelected();
        }
        OnMenu();
    }
}

