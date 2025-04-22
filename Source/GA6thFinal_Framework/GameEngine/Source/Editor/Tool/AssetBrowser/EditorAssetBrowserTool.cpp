#include "pch.h"
#include "EditorAssetBrowserTool.h"

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
    spForderContext spForcusForder;
    if (false == _focusForder.expired())
    {
        spForcusForder = _focusForder.lock();
    }
    std::string focuspath = EditorIcon::ICON_FORDER_OPEN + std::string(" ");
    if (nullptr != spForcusForder)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 230, 200, 255));
        focuspath += spForcusForder->GetPath().string();
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
        if (nullptr != spForcusForder)
        {
            // DragDropTransform::WriteGameObjectFile(data.pTransform, _focusForder.lock()->GetPath().string());
        }
    }

    if (true == ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Backspace, false))
    {
        if (false == _renameFlags[RENAME_IS_RENAME])
        {
            auto&      curPath       = spForcusForder->GetPath();
            File::Path parentPath    = curPath.parent_path();
            auto       parentContext = UmFileSystem.GetContext<File::ForderContext>(parentPath);
            if (false == parentContext.expired())
            {
                _focusForder = parentContext.lock();
            }
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
    const File::Path&  path = context->GetPath();
    const std::string  icon = context->IsDirectory() ? EditorIcon::ICON_FORDER : EditorIcon::ICON_FILE;
    const std::string& name = context->GetName();

    bool isMeta = path.extension() == UmFileSystem.GetMetaExt();
    bool isSelected = UmFileSystem.IsSameContext(_selectedContext->GetContext(), context);

    if (true == mAssetBrowserFlags[ShowMetaFile] || (false == mAssetBrowserFlags[ShowMetaFile] && false == isMeta))
    {
        ImGui::PushID(context.get());

        ImGui::Selectable("##Selectable", isSelected, 0);
        {
            ItemEventAction(context);
            ItemMouseAction(context);
            ItemKeyBoardAction(context);

            ImGui::SameLine(ImGui::GetCursorPosX());
            ImGui::Text((icon + " ").c_str());
            ImGui::SameLine();
        }

        if (true == isSelected && true == _renameFlags[RENAME_IS_RENAME])
        {
            ItemInputText(context);
        }
        else
        {
            ImGui::Text(name.c_str());
        }

        ItemPopupAction(context);

        ImGui::PopID();
    }
}

void EditorAssetBrowserTool::ShowItemToIcon(spContext context) {}

void EditorAssetBrowserTool::ItemInputText(spContext context)
{
    static char        buffer[128] = "";

    const std::string&  name = context->GetName();
    const File::Path&   path = context->GetPath();
    ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f)); // Y 패딩만 약간 줘서 커서 보이게
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));       // 투명 배경

    if (true == _renameFlags[RENAME_SET_FOCUS_ONCE])
    {
        ImGui::SetKeyboardFocusHere();
        strcpy_s(buffer, name.c_str());
        _renameFlags[RENAME_SET_FOCUS_ONCE] = false;
    }

    ImGui::InputText("##InputText", buffer, IM_ARRAYSIZE(buffer), flags);

    if (ImGui::IsItemDeactivated())
    {
        std::filesystem::path newPath = path;
        newPath.replace_filename(buffer);
        context->Move(newPath);
        _renameFlags[RENAME_IS_RENAME] = false;
    }

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}

void EditorAssetBrowserTool::ItemEventAction(spContext context) 
{
    if (true == context->IsRegularFile())
    {
        DragDropAsset::Data data;
        const char* eventID = DragDropAsset::key;
        data.context = std::static_pointer_cast<File::FileContext>(context);

        std::function<void()> func = [&context]() {
            File::Path path = context->GetPath();
            ImGui::Text(path.string().c_str());
        };

        if (ImGuiHelper::DragDrop::SendDragDropEvent(eventID, &data, func))
        {
            // Dragging
        }
    }
}

void EditorAssetBrowserTool::ItemMouseAction(spContext context)
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
                context->Open();
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

void EditorAssetBrowserTool::ItemKeyBoardAction(spContext context) 
{
    bool isSelected = UmFileSystem.IsSameContext(_selectedContext->GetContext(), context);
    bool isLocked   = IsLock();

    if (true == isSelected && false == isLocked)
    {
        if (true == _renameFlags[RENAME_IS_RENAME])
        {
            if (true == ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Escape, false))
            {
                _renameFlags[RENAME_IS_RENAME]      = false;
                _renameFlags[RENAME_SET_FOCUS_ONCE] = false;
            }
        }
        else if (false == _renameFlags[RENAME_IS_RENAME])
        {
            if (true == ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_F2, false))
            {
                _renameFlags[RENAME_IS_RENAME]      = true;
                _renameFlags[RENAME_SET_FOCUS_ONCE] = true;
            }

            if (true == ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Enter, false))
            {
                _focusForder = std::static_pointer_cast<File::ForderContext>(context);
                _selectedContext->SetContext(context);
                EditorInspectorTool::SetFocusObject(_selectedContext);
            }

            if (true == ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Delete, false))
            {
                Global::editorModule->OpenPopupBox("RemoveAsset", [&, context]() { ShowDeletePopupBox(context); });
            }
        }
    }
}

void EditorAssetBrowserTool::ItemPopupAction(spContext context) 
{
    if (ImGui::BeginPopupContextItem("ItemPopup"))
    {
        _selectedContext->SetContext(context);
        if (ImGui::MenuItem("Open##"))
        {
            context->Open();
            ImGui::CloseCurrentPopup(); // 팝업 닫기
        }
        if (ImGui::MenuItem("Delete##"))
        {
            Global::editorModule->OpenPopupBox("RemoveAsset", [&, context]() { ShowDeletePopupBox(context); });
            ImGui::CloseCurrentPopup(); // 팝업 닫기
        }
        if (ImGui::MenuItem("Rename##"))
        {
            _renameFlags[RENAME_IS_RENAME]      = true;
            _renameFlags[RENAME_SET_FOCUS_ONCE] = true;
            ImGui::CloseCurrentPopup(); // 팝업 닫기
        }
        ImGui::EndPopup();
    }
}

void EditorAssetBrowserTool::ShowDeletePopupBox(wpContext context)
{
    ImGui::Text("정말 삭제하시겠습니까?");

    if (false == context.expired())
    {
        auto spContext = context.lock();    
        File::Path path = spContext->GetPath();
        ImGui::Text("경로: ");
        ImGui::SameLine();
        ImGui::Text(path.string().c_str());

        ImGui::PushID(this);

        if (ImGui::Button("OK##") || ImGui::IsKeyReleased(ImGuiKey::ImGuiKey_Enter))
        {
            spContext->Remove();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("NO##"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::PopID();
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