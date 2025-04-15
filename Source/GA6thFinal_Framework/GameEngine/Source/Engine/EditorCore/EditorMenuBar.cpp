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

    for (auto& node : _root->_menuList)
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
            parent->_menuList.push_back(instance);

            Sort(parent);
        }
    }

    return _nodeTable[path].get();
}

void EditorMenuBar::Sort(EditorMenuNode* root)
{
    if (root->_menuList.empty())
        return;

    std::sort(root->_menuList.begin(), root->_menuList.end(),
        [](EditorBase* a, EditorBase* b) {
            return a->GetCallOrder() > b->GetCallOrder();
        });

    for (auto& instance : root->_menuList)
    {
        auto* node = dynamic_cast<EditorMenuNode*>(instance);
        if (node)
        {
            Sort(node);
        }
    }
}

EditorMenuBase::EditorMenuBase(std::string_view path) 
    : _path(path)
{

}

EditorMenuBase::~EditorMenuBase() 
{
}

void EditorMenuBase::DefaultDebugFrame()
{
    static char tooltip[256];

    snprintf(tooltip, sizeof(tooltip), "GuiID: 0x%08X\nOrder: %d",
             ImGui::GetID(""), _callOrder);

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(tooltip);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

EditorMenuNode::EditorMenuNode(std::string_view path) : EditorMenuBase(path) {}

EditorMenuNode::~EditorMenuNode() 
{
}

void EditorMenuNode::OnDrawGui()
{
    if (GetVisible())
    {
        ImGui::PushID(this);

        if (ImGui::BeginMenu(GetPath().c_str(), GetActive()))
        {
            if (true == Global::editorManager->IsDebugMode())
            {
                DefaultDebugFrame();
            }
            for (auto& node : _menuList)
            {
                node->OnDrawGui();
            }
            ImGui::EndMenu();
        }
        ImGui::PopID();
    }
}

EditorMenu::EditorMenu() : EditorMenuBase("") {}

EditorMenu::~EditorMenu() {}

void EditorMenu::OnDrawGui()
{
    OnTickGui();

    if (true == GetVisible())
    {
        ImGui::PushID(this);

        OnMenu();
        if (true == Global::editorManager->IsDebugMode())
        {
            DefaultDebugFrame();
        }

        ImGui::PopID();
    }
}
