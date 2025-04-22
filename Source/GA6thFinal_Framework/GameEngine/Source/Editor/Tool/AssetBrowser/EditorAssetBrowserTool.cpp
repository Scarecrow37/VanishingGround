#include "pch.h"
#include "EditorAssetBrowserTool.h"

namespace fs = std::filesystem;
using namespace u8_literals;

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
    _focusFolder = UmFileSystem.GetContext<File::FolderContext>(File::Path(UmFileSystem.GetRootPath()));
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
    std::weak_ptr<File::FolderContext> rootFolder =
        UmFileSystem.GetContext<File::FolderContext>(File::Path(UmFileSystem.GetRootPath()));

    if (false == rootFolder.expired())
    {
        ShowFolderHierarchy(rootFolder.lock());
    }
}

#define NODE_SPACING "                                                                ##"

// 왼쪽: 폴더 트리 표시 (재귀)
void EditorAssetBrowserTool::ShowFolderHierarchy(spFolderContext FolderContext)
{
    auto& path       = FolderContext->GetPath();
    int   flags      = ImGuiTreeNodeFlags_OpenOnArrow;
    bool  isSelected = UmFileSystem.IsSameContext(_focusFolder, FolderContext);
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
        SetFocusFolder(FolderContext);
    }

    // ==== Text출력 ====
    std::string icon = isOpen ? EditorIcon::ICON_Folder_OPEN : EditorIcon::ICON_Folder;
    std::string name = icon + " " + path.filename().string();
    ImGui::SameLine();
    ImGui::SetCursorPosX(startX + offsetX);
    ImGui::Text(name.c_str());

    
    if (isOpen)
    {
        for (auto itr = FolderContext->begin(); itr != FolderContext->end(); ++itr)
        {
            auto& [name, wpCtx] = *itr;
            if (false == wpCtx.expired())
            {
                auto spCtx = wpCtx.lock();
                // 폴더일때만
                if (spCtx->IsDirectory())
                {
                    auto spFolderCtx = std::static_pointer_cast<File::FolderContext>(spCtx);
                    ShowFolderHierarchy(spFolderCtx);
                }
            }
        }
        ImGui::TreePop();
    }
}

// 오른쪽: 선택된 폴더의 파일 목록
void EditorAssetBrowserTool::ShowFolderContents()
{
    spFolderContext spForcusFolder;
    if (false == _focusFolder.expired())
    {
        spForcusFolder = _focusFolder.lock();
    }

    ShowFolderDirectoryPath(spForcusFolder);

    ContentsFrameEventAction(spForcusFolder);

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

void EditorAssetBrowserTool::ShowFolderDirectoryPath(spFolderContext context)
{
    if (nullptr != context)
    {
        const File::Path& path = context->GetPath();

        bool canUndo = (false == _directoryUndoStack.empty());
        bool canRedo = (false == _directoryRedoStack.empty());
        {
            const char* icon  = EditorIcon::ICON_CIRCLE_ARROW_LEFT;
            ImVec2      size  = ImGui::CalcTextSize(icon);
            int         flags = canUndo ? ImGuiSelectableFlags_None : ImGuiSelectableFlags_Disabled;
            if (ImGui::Selectable(icon, false, flags, size))
            {
                SetFocusFromUndoPath();
            }
            ImGui::SameLine(0.0f, 10.0f);
        }
        {
            const char* icon  = EditorIcon::ICON_CIRCLE_ARROW_RIGHT;
            ImVec2      size  = ImGui::CalcTextSize(icon);
            int         flags = canRedo ? ImGuiSelectableFlags_None : ImGuiSelectableFlags_Disabled;
            if (ImGui::Selectable(icon, false, flags, size))
            {
                SetFocusFromRedoPath();
            }
            ImGui::SameLine(0.0f, 10.0f);
        }

        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 230, 200, 255));

        std::string icon = EditorIcon::ICON_Folder_OPEN;
        ImGui::Text(icon.c_str());
        ImGui::SameLine(0.0f, 5.0f);

        File::Path curPath;

        for (auto itr = path.begin(); itr != path.end();)
        {
            fs::path name = (*itr);
            std::string nameStr = name.string();
            curPath /= name;

            ImVec2 textSize = ImGui::CalcTextSize(nameStr.c_str());
            float  startX   = ImGui::GetCursorPosX();

            ImGui::PushID(nameStr.c_str());

            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 0)); // 기본 배경 투명
            if (ImGui::Selectable(nameStr.c_str(), false, 0, textSize))
            {
                auto selectedContext = UmFileSystem.GetContext<File::FolderContext>(curPath);
                if (false == selectedContext.expired())
                {
                    SetFocusFolder(selectedContext);
                }
            }
            ImGui::PopStyleColor();

            ImGui::PopID();

            if (path.end() != ++itr)
            {
                ImGui::SameLine();
                ImGui::Text("/");
                ImGui::SameLine();
            }
        }
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 100, 100, 255));
        File::Path curPath = _focusFolderPath;
        while (false == fs::exists(curPath))
        {
            curPath = curPath.parent_path();

            auto context = UmFileSystem.GetContext<File::FolderContext>(curPath);
            if (false == context.expired())
            {
                SetFocusFolder(context);
            }
            else
            {
                if (true == curPath.empty())
                {
                    break;
                }
                continue;
            }
        }
    }

    ImGui::PopStyleColor();
    ImGuiHelper::Separator();
}

void EditorAssetBrowserTool::ContentsFrameEventAction(spFolderContext context)
{
    DragDropTransform::Data data;
    if (ImGuiHelper::DragDrop::RecieveFrameDragDropEvent(DragDropTransform::KEY, &data))
    {
        if (nullptr != context)
        {
            DragDropTransform::WriteGameObjectFile(data.pTransform, context->GetPath().string());
        }
    }

    if (true == ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Backspace, false))
    {
        if (false == browserFlags[RENAME_IS_RENAME])
        {
            SetFocusParentFolder(context);
        }
    }

    if (true == ImGui::IsWindowHovered() && false == ImGui::IsAnyItemHovered())
    {
        if (true == ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        {
            ImGui::OpenPopup("ContentsFramePopup");
        }
    }

    float compactFactor = 0.1f;
    ImGuiHelper::PushStyleCompactToItem(compactFactor);

    if (ImGui::BeginPopup("ContentsFramePopup"))
    {
        if (ImGui::BeginMenu("Create"))
        {
            if (ImGui::MenuItem("Folder"))
            {
                File::CreateFolderEx(context->GetPath() / "New Folder", true);
            }
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Copy Path"))
        {
            File::CopyPathToClipBoard(context->GetPath());
        }
        ImGui::EndPopup();
    }

    ImGuiHelper::PopStyleCompact();
}

void EditorAssetBrowserTool::ShowContentsToList()
{
    // 참조 포인터가 살아있을 때
    if (false == _focusFolder.expired())
    {
        auto spFocusCtx = _focusFolder.lock();

        for (auto itr = spFocusCtx->begin(); itr != spFocusCtx->end(); ++itr)
        {
            auto& [name, wpFileCtx] = *itr;
            if (false == wpFileCtx.expired())
            {
                auto spFileCtx = wpFileCtx.lock();

                File::Path  path = spFileCtx->GetPath();
                std::string id   = name.string() + "##" + path.string();

                bool isMeta = path.extension() == UmFileSystem.GetMetaExt();

                if (true == browserFlags[META_IS_SHOW] || (false == browserFlags[META_IS_SHOW] && false == isMeta))
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
    const std::string  icon = context->IsDirectory() ? EditorIcon::ICON_Folder : EditorIcon::ICON_FILE;
    const std::string& name = context->GetName();

    bool isSelected = UmFileSystem.IsSameContext(_selectedContext->GetContext(), context);

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

    if (true == isSelected && true == browserFlags[RENAME_IS_RENAME])
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

void EditorAssetBrowserTool::ShowItemToIcon(spContext context) {}

void EditorAssetBrowserTool::ItemInputText(spContext context)
{
    static char        buffer[128] = "";

    const std::string&  name = context->GetName();
    const File::Path&   path = context->GetPath();
    ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f)); // Y 패딩만 약간 줘서 커서 보이게
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));       // 투명 배경

    if (true == browserFlags[RENAME_SET_FOCUS_ONCE])
    {
        ImGui::SetKeyboardFocusHere();
        strcpy_s(buffer, name.c_str());
        browserFlags[RENAME_SET_FOCUS_ONCE] = false;
    }

    ImGui::InputText("##InputText", buffer, IM_ARRAYSIZE(buffer), flags);

    if (ImGui::IsItemDeactivated())
    {
        std::filesystem::path newPath = path;
        newPath.replace_filename(buffer);
        context->Move(newPath);
        browserFlags[RENAME_IS_RENAME] = false;
    }

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}

void EditorAssetBrowserTool::ItemEventAction(spContext context) 
{
    if (true == context->IsRegularFile())
    {
        DragDropAsset::Data data;
        const char* eventID = DragDropAsset::KEY;
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
    if (true ==ImGui::IsItemHovered())
    {
        // 더블 클릭 시 (이거 먼저 확인해야함!)
        if (true == ImGui::IsMouseDoubleClicked(0))
        {
            // 폴더일 시
            if (true == context->IsDirectory())
            {
                auto spFolderContext = std::static_pointer_cast<File::FolderContext>(context);
                SetFocusFolder(spFolderContext);
            }
            // 파일일 시
            else
            {
                context->Open();
            }
        }
        // 일반 클릭 시
        else if (true == ImGui::IsItemClicked(0))
        {
            _selectedContext->SetContext(context);
            EditorInspectorTool::SetFocusObject(_selectedContext);
        }
        else if (true == ImGui::IsItemClicked(1))
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
        if (true == browserFlags[RENAME_IS_RENAME])
        {
            if (true == ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Escape, false))
            {
                browserFlags[RENAME_IS_RENAME]      = false;
                browserFlags[RENAME_SET_FOCUS_ONCE] = false;
            }
        }
        else if (false == browserFlags[RENAME_IS_RENAME])
        {
            if (true == ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_F2, false))
            {
                browserFlags[RENAME_IS_RENAME]      = true;
                browserFlags[RENAME_SET_FOCUS_ONCE] = true;
            }

            if (true == ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Enter, false))
            {
                if (true == context->IsRegularFile())
                {
                    context->Open();
                }
                else if (true == context->IsDirectory())
                {
                    auto spFolderContext = std::static_pointer_cast<File::FolderContext>(context);
                    SetFocusFolder(spFolderContext);
                }
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
            // 지금 포커싱 되어있는 폴더인 경우 혹시나 하는 예외 처리
            bool isSelected = UmFileSystem.IsSameContext(_selectedContext->GetContext(), context);
            if (true == isSelected)
            {
                const File::Path rootPath = UmFileSystem.GetRootPath();
                wpFolderContext  rootContext = UmFileSystem.GetContext<File::FolderContext>(rootPath);
                _selectedContext->SetContext(rootContext);
                EditorInspectorTool::SetFocusObject(_selectedContext);
            }
            ImGui::CloseCurrentPopup(); // 팝업 닫기
        }
        if (ImGui::MenuItem("Rename##"))
        {
            browserFlags[RENAME_IS_RENAME]      = true;
            browserFlags[RENAME_SET_FOCUS_ONCE] = true;
            ImGui::CloseCurrentPopup(); // 팝업 닫기
        }
        ImGui::EndPopup();
    }
}

void EditorAssetBrowserTool::ShowDeletePopupBox(wpContext context)
{
    ImGui::Text(u8"정말 삭제하시겠습니까?"_c_str);

    if (false == context.expired())
    {
        auto spContext = context.lock();    
        File::Path path = spContext->GetPath();
        ImGui::Text(u8"경로: "_c_str);
        ImGui::SameLine();
        ImGui::Text(path.string().c_str());

        ImGui::PushID(this);

        if (ImGui::Button("OK##") || ImGui::IsKeyReleased(ImGuiKey::ImGuiKey_Enter))
        {
            spContext->Remove();
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("NO##") || ImGui::IsKeyReleased(ImGuiKey::ImGuiKey_Escape))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::PopID();
    }
}

bool EditorAssetBrowserTool::SetFocusFolder(wpFolderContext context)
{
    if (false == context.expired())
    {
        auto spContext = context.lock();
        if (false == _focusFolder.expired())
        {
            const File::Path& path = _focusFolder.lock()->GetPath();
            _directoryUndoStack.push_back(path);
            if (_directoryUndoStack.size() > _maxUndoStack)
            {
                _directoryUndoStack.pop_front();
            }
        }
        _directoryRedoStack.clear();
        _focusFolder = spContext;
        _focusFolderPath = spContext->GetPath();
        return true;
    }
    else
    {
        const File::Path rootPath = UmFileSystem.GetRootPath();
        _focusFolder = UmFileSystem.GetContext<File::FolderContext>(rootPath);
        _directoryUndoStack.clear();
        _directoryRedoStack.clear();
        return false;
    }
}

void EditorAssetBrowserTool::SetFocusParentFolder(spFolderContext context)
{
    const File::Path& curPath       = context->GetPath();
    const File::Path  parentPath    = curPath.parent_path();
    const auto parentContext = UmFileSystem.GetContext<File::FolderContext>(parentPath);
    SetFocusFolder(parentContext);
}

void EditorAssetBrowserTool::SetFocusFromUndoPath() 
{
    if (false == _directoryUndoStack.empty())
    {
        const File::Path& undoPath = _directoryUndoStack.back();
        auto undoContext = UmFileSystem.GetContext<File::FolderContext>(undoPath);
        if (false == undoContext.expired())
        {
            if (false == _focusFolder.expired())
            {
                const File::Path& path = _focusFolder.lock()->GetPath();
                _directoryRedoStack.push_back(path);
            }
            _focusFolder     = undoContext;
            _focusFolderPath = undoContext.lock()->GetPath();
        }
        _directoryUndoStack.pop_back();
    }
}

void EditorAssetBrowserTool::SetFocusFromRedoPath() 
{
    if (false == _directoryRedoStack.empty())
    {
        const File::Path& curPath = _directoryRedoStack.back();
        auto redoContext = UmFileSystem.GetContext<File::FolderContext>(curPath);
        if (false == redoContext.expired())
        {
            if (false == _focusFolder.expired())
            {
                const File::Path& path = _focusFolder.lock()->GetPath();
                _directoryUndoStack.push_back(path);
            }
            _focusFolder     = redoContext;
            _focusFolderPath = redoContext.lock()->GetPath();
        }
        _directoryRedoStack.pop_back();
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