#include "EditorAssetBrowserTool.h"
#include "pch.h"

namespace fs = std::filesystem;

EditorAssetBrowserTool::EditorAssetBrowserTool()
{
    SetLabel("AssetBrowser");
    SetDockLayout(DockLayout::DOWN);
    SetWindowFlag(ImGuiWindowFlags_MenuBar);    // 메뉴바 사용

    _selectedContext = std::make_shared<EditorFileObject>();

    mShowType = List;
}

EditorAssetBrowserTool::~EditorAssetBrowserTool() 
{
}

void EditorAssetBrowserTool::OnStartGui()
{
    _focusForder = UmFileSystem.GetContext<File::ForderContext>(File::Path(UmFileSystem.GetRootPath()));
}

void EditorAssetBrowserTool::OnPreFrame() {}

void EditorAssetBrowserTool::OnFrame()
{
    ImGui::PushID(this);

    // 메뉴 창
    ShowBrowserMenu();

    ImGui::BeginChild("UpperFrame", ImVec2(0, _upperHeight), true);
    {
        ShowUpperFrame();
    }
    ImGui::EndChild();

    // Left, Right 구분 창
    BeginColumn();
    {
        // 왼쪽: 폴더 트리
        ImGui::BeginChild("FolderHierarchyFrame", ImVec2(_columWidth, _columHeight), true);
        {
            ShowFolderHierarchy();
        }
        ImGui::EndChild();

        ShowColumnPlitter();
        ImGui::SameLine();

        // 오른쪽: 선택한 폴더의 파일 목록
        ImGui::BeginChild("ContentsFrame", ImVec2(0, _columHeight), true);
        {
            ShowFolderContents();
        }
        ImGui::EndChild();
    }
    EndColumn();

    ImGui::PopID();
}

void EditorAssetBrowserTool::OnPostFrame() {}

void EditorAssetBrowserTool::ShowBrowserMenu() 
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Temp"))
        {
            ImGui::MenuItem("New");
            ImGui::MenuItem("Open");
            ImGui::MenuItem("Save");
            ImGui::EndMenu();
        }
        ImGui::Button("List");
        ImGui::EndMenuBar();
    }
}

void EditorAssetBrowserTool::ShowUpperFrame() {}

void EditorAssetBrowserTool::BeginColumn()
{
    ImGuiWindow* window      = GImGui->CurrentWindow;
    ImRect       rect        = window->Rect();
    ImDrawList*  draw_list   = ImGui::GetWindowDrawList();
    float        columWidth  = rect.Max.x - rect.Min.x;
    float        columHeight = rect.Max.y - rect.Min.y;

    _columHeight = ImGui::GetWindowContentRegionMax().y - ImGui::GetCursorPosY();

    _columWidth  = ImClamp(_columWidth, 200.0f, columWidth - 100.0f);
    _columHeight = ImMax(_columHeight, 1.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);  // 라운딩 적용
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f); // 경계 두께 설정
}

void EditorAssetBrowserTool::EndColumn() 
{
    ImGui::PopStyleVar(3);
}

void EditorAssetBrowserTool::ShowColumnPlitter()
{
    ImGuiWindow* window      = GImGui->CurrentWindow;
    ImRect       rect        = window->Rect();
    float        columWidth  = rect.Max.x - rect.Min.x;
    float        columHeight = rect.Max.y - rect.Min.y;
    static float padding     = 8.0f;

    ImGui::SameLine();
    ImGui::InvisibleButton("##AssetBrowserPlitter", ImVec2(padding, _columHeight));
    if (true == ImGui::IsItemActive())
    {
        float center = ImGui::GetIO().MousePos.x - rect.Min.x - (padding * 1.5f);
        _columWidth  = center;
    }
}

void EditorAssetBrowserTool::ShowFolderHierarchy()
{
    std::weak_ptr<File::ForderContext> rootForder =
        UmFileSystem.GetContext<File::ForderContext>(File::Path(UmFileSystem.GetRootPath()));

    if (false == rootForder.expired())
    {
        ShowFolderHierarchy(rootForder.lock());
    }
}

#define NODE_SPACING "                                                                ##"

// 왼쪽: 폴더 트리 표시 (재귀)
void EditorAssetBrowserTool::ShowFolderHierarchy(spForderContext forderContext)
{
    auto& path       = forderContext->GetPath();
    int   flags      = ImGuiTreeNodeFlags_OpenOnArrow;
    bool  isSelected = UmFileSystem.IsSameContext(_focusForder, forderContext);
    float startX     = ImGui::GetCursorPosX();
    float offsetX    = 30.0f;

    if (true == isSelected)
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    // ==== Tree 출력 ====
    bool  isOpen = ImGui::TreeNodeEx((NODE_SPACING + path.string()).c_str(), flags);
    bool  isHovered = ImGui::IsItemHovered();
    bool  isMouseDeoubleClicked = ImGui::IsMouseDoubleClicked(0);

    if (true == isHovered && true == isMouseDeoubleClicked)
    {
        _focusForder = forderContext;
    }

    // ==== Text출력 ====
    std::string icon = isOpen ? EditorIcon::ICON_FORDER_OPEN : EditorIcon::ICON_FORDER;
    std::string name = icon + " " + path.filename().string();
    ImGui::SameLine();
    ImGui::SetCursorPosX(startX + offsetX);
    ImGui::Text(name.c_str());

    
    if (isOpen)
    {
        for (auto itr = forderContext->begin(); itr != forderContext->end(); ++itr)
        {
            auto& [name, wpCtx] = *itr;
            if (false == wpCtx.expired())
            {
                auto spCtx = wpCtx.lock();
                // 폴더일때만
                if (spCtx->IsDirectory())
                {
                    auto spForderCtx = std::static_pointer_cast<File::ForderContext>(spCtx);
                    ShowFolderHierarchy(spForderCtx);
                }
            }
        }
        ImGui::TreePop();
    }
}

// 오른쪽: 선택된 폴더의 파일 목록
void EditorAssetBrowserTool::ShowFolderContents()
{
    std::string focuspath = EditorIcon::ICON_FORDER_OPEN + std::string(" ");
    if (false == _focusForder.expired())
    {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 230, 200, 255));
        focuspath += _focusForder.lock()->GetPath().string();
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 100, 100, 255));
        focuspath += "Null";
    }
    ImGui::Text(focuspath.c_str());
    ImGui::PopStyleColor();
    ImGuiHelper::Separator();

    DragDropTransform::Data data;
    if (ImGuiHelper::DragDrop::RecieveFrameDragDropEvent(DragDropTransform::key, &data))
    {
        if (false == _focusForder.expired())
        {
            // DragDropTransform::WriteGameObjectFile(data.pTransform, _focusForder.lock()->GetPath().string());
        }
    }

    if (ImGui::BeginPopupContextItem("ContentsPopup"))
    {
        ImGui::MenuItem("Show in explorer");
        ImGui::MenuItem("Refresh");
        // 팝업 내용
        ImGui::EndPopup();
    }
    switch (mShowType)
    {
    case EditorAssetBrowserTool::List:
        ShowContentsToList();
        break;
    case EditorAssetBrowserTool::Icon:
        ShowContentsToIcon();
        break;
    default:
        break;
    }
}

void EditorAssetBrowserTool::ShowContentsToList()
{
    // 참조 포인터가 살아있을 때
    if (false == _focusForder.expired())
    {
        auto spFocusCtx = _focusForder.lock();

        for (auto itr = spFocusCtx->begin(); itr != spFocusCtx->end(); ++itr)
        {
            auto& [name, wpFileCtx] = *itr;
            if (false == wpFileCtx.expired())
            {
                auto spFileCtx = wpFileCtx.lock();

                File::Path  path = spFileCtx->GetPath();
                std::string id   = name.string() + "##" + path.string();

                bool isMeta = path.extension() == UmFileSystem.GetMetaExt();

                if (true == mAssetBrowserFlags[ShowMetaFile] ||
                    (false == mAssetBrowserFlags[ShowMetaFile] && false == isMeta))
                {
                    ShowItemToList(spFileCtx);
                }
            }
        }
    }
}

void EditorAssetBrowserTool::ShowContentsToIcon() {}

void EditorAssetBrowserTool::ShowItemToList(spContext context)
{
    const File::Path& path = context->GetPath();
    const std::string icon = context->IsDirectory() ? EditorIcon::ICON_FORDER : EditorIcon::ICON_FILE;
    const std::string name = icon + " " + context->GetName();

    bool isMeta = path.extension() == UmFileSystem.GetMetaExt();

    if (true == mAssetBrowserFlags[ShowMetaFile] || (false == mAssetBrowserFlags[ShowMetaFile] && false == isMeta))
    {
        ImGui::PushID(context.get());

        bool isSelected = UmFileSystem.IsSameContext(_selectedContext->GetContext(), context);

        ImGui::Selectable(name.c_str(), isSelected);

        if (ImGui::BeginPopupContextItem())
        {
            ImGui::MenuItem("Open##1");
            ImGui::MenuItem("Delete##2");
            // 팝업 내용
            ImGui::EndPopup();
        }

        ItemClickedAction(context);

        ImGui::PopID();
    }
}

void EditorAssetBrowserTool::ShowItemToIcon(spContext context) {}

void EditorAssetBrowserTool::ItemClickedAction(spContext context)
{
    if (ImGui::IsItemHovered())
    {
        // 더블 클릭 시 (이거 먼저 확인해야함!)
        if (ImGui::IsMouseDoubleClicked(0))
        {
            // 폴더일 시
            if (context->IsDirectory())
            {
                _focusForder = std::static_pointer_cast<File::ForderContext>(context);
            }
            // 파일일 시
            else
            {
            }
        }
        // 일반 클릭 시
        else if (ImGui::IsItemClicked(0))
        {
            _selectedContext->SetContext(context);
            EditorInspectorTool::SetFocusObject(_selectedContext);
        }
        else if (ImGui::IsItemClicked(1))
        {
            // ImGui::OpenPopup("RightClickPopup");
        }
    }
}

void EditorFileObject::OnInspectorStay()
{
    if (false == _context.expired())
    {
        auto  spContext = _context.lock();
        auto& metaData  = spContext->GetMeta();

        ImGui::Text("Path: %s", spContext->GetPath().string().c_str());
        ImGui::Text("Guid: %s", metaData.GetFileGuid().string().c_str());
        ImGui::Separator();
    }
}