#include "pch.h"

namespace fs = std::filesystem;
using namespace u8_literals;

EditorAssetBrowserTool::EditorAssetBrowserTool()
{
    SetLabel("AssetBrowser");
    SetDockLayout(ImGuiDir_Down);

    _selectedContext = std::make_shared<EditorAssetObject>();
    _selectedContext->SetThis(_selectedContext);

    _showType = List;
}

EditorAssetBrowserTool::~EditorAssetBrowserTool() 
{
}

void EditorAssetBrowserTool::OnStartGui()
{
    _currFocusFolderContext = UmFileSystem.GetContext<File::FolderContext>(File::Path(UmFileSystem.GetRootPath()));
}

void EditorAssetBrowserTool::OnPreFrameBegin()
{
    if (false == _nextFocusFolderContext.expired())
    {
        auto sp = _nextFocusFolderContext.lock();

        _currFocusFolderContext = sp;
        _currFocusFolderPath    = sp->GetPath();
        _nextFocusFolderContext.reset();
    }
}

void EditorAssetBrowserTool::OnFrameRender()
{
    ImGui::PushID(this);

    ImGuiChildFlags flags = ImGuiChildFlags_Border;
    
    ShowUpperFrame();

    // Left, Right 구분 창
    BeginColumn();
    {
        // 왼쪽: 폴더 트리
        ImGui::BeginChild("FolderHierarchyFrame", ImVec2(ReflectFields->ColumWidth, ReflectFields->ColumHeight), flags);
        {
            ShowFolderHierarchy();
        }
        ImGui::EndChild();

        ShowColumnPlitter();
        ImGui::SameLine();

        // 오른쪽: 선택한 폴더의 파일 목록
        ImGui::BeginChild("ContentsFrame", ImVec2(0, ReflectFields->ColumHeight), flags);
        {
            ShowFolderContents();
        }
        ImGui::EndChild();
    }
    EndColumn();

    ImGui::PopID();
}

void EditorAssetBrowserTool::OnFrameEnd()
{
    for (auto& func : _eventFunc)
    {
        if (nullptr != func)
            func();
    }
    _eventFunc.clear();
}

#define REFRESH_TEXT "Refresh"
void EditorAssetBrowserTool::ShowUpperFrame()
{
    ImVec2 textSize = ImGui::CalcTextSize(REFRESH_TEXT);
    float  upperHeight = textSize.y + 15.0f;

    ImGuiChildFlags flags = ImGuiChildFlags_Border;
    ImGui::BeginChild("UpperFrame", ImVec2(0, upperHeight), flags, ImGuiWindowFlags_NoScrollbar);
    {
        // 해당 프레임의 사용 가능 영역을 가져옴
        ImVec2 windowPos  = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 availableSize = ImGui::GetContentRegionAvail();

        ImVec2 buttonSize = ImVec2(textSize.x, availableSize.y); // 버튼 크기 설정
        //ImGui::SetCursorPos(ImVec2(0, 0));            // 윈도우 내부 좌표 기준

        if (ImGui::Selectable(REFRESH_TEXT, false, ImGuiSelectableFlags_None, buttonSize))
        {
            File::Path path = UmFileSystem.GetRootPath();
            UmFileSystem.ReadDirectory();
            _currFocusFolderContext = UmFileSystem.GetContext<File::FolderContext>(path);
            _directoryUndoStack.clear();
            _directoryRedoStack.clear();
        }
        ImGui::SameLine();
        if (false == _currFocusFolderContext.expired())
        {
            spFolderContext spForcusFolder = _currFocusFolderContext.lock();
            ShowFolderDirectoryPath(spForcusFolder);
        }
    }
    ImGui::EndChild();
}

void EditorAssetBrowserTool::ShowFolderDirectoryPath(spFolderContext context)
{
    if (nullptr != context)
    {
        const File::Path& path = context->GetPath();
        const File::Path& root = UmFileSystem.GetRootPath();

        bool canUndo = (false == _directoryUndoStack.empty());
        bool canRedo = (false == _directoryRedoStack.empty());

        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 pos        = ImGui::GetCursorPos();
        {
           
            const char* icon        = EditorIcon::ICON_CIRCLE_ARROW_LEFT;
            ImVec2      size        = ImGui::CalcTextSize(icon);
            int         flags       = canUndo ? ImGuiSelectableFlags_None : ImGuiSelectableFlags_Disabled;

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

        std::string icon = EditorIcon::ICON_FOLDER_OPEN;
        ImGui::Text(icon.c_str());
        ImGui::SameLine(0.0f, 5.0f);

        ListToDirectoryFileName(L".");

        // 상대경로로 부모 폴더 계산
        File::Path relativePath;
        relativePath = fs::relative(path, root);

        File::Path node;
        for (auto itr = relativePath.begin(); itr != relativePath.end(); ++itr)
        {
            if ((*itr) == L".")
                continue;

            ImGui::SameLine();
            ImGui::Text("/");
            ImGui::SameLine();

            File::Path folderName = fs::absolute((*itr)).filename();
            node /= folderName;
            ListToDirectoryFileName(node);
        }
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 100, 100, 255));
        File::Path curPath = _currFocusFolderPath;
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
}

void EditorAssetBrowserTool::ListToDirectoryFileName(const File::Path& relativePath)
{
    auto& root = UmFileSystem.GetRootPath();

    File::Path  absPath    = fs::absolute(relativePath);
    File::Path  folderName = absPath.filename();
    std::string nameStr    = folderName.string();

    ImVec2 textSize = ImGui::CalcTextSize(nameStr.c_str());
    float  startX   = ImGui::GetCursorPosX();

    auto wpFolderContext = UmFileSystem.GetContext<File::FolderContext>(absPath);
    auto spFolderContext = wpFolderContext.lock(); // 실패하면 개버그니까 그냥 과감하게 lock

    ImGui::PushID(spFolderContext.get());

    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 0)); // 기본 배경 투명
    if (ImGui::Selectable(nameStr.c_str(), false, 0, textSize))
    {
        SetFocusFolder(wpFolderContext);
    }

    const char* eventID = DragDropAsset::KEY;
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(eventID))
        {
            DragDropAsset::Data data = (*(DragDropAsset::Data*)payload->Data);
            _eventFunc.emplace_back([=]() { ProcessMoveAction(*data.pContext, spFolderContext); });
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::PopStyleColor();

    ImGui::PopID();
}

void EditorAssetBrowserTool::BeginColumn()
{
    ReflectFields->ColumHeight = ImGui::GetWindowContentRegionMax().y - ImGui::GetCursorPosY();
    ReflectFields->ColumHeight = ImMax(ReflectFields->ColumHeight, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);   // 라운딩 적용
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
    ImGui::InvisibleButton("##AssetBrowserPlitter", ImVec2(padding, ReflectFields->ColumHeight));
    if (true == ImGui::IsItemActive())
    {
        float center              = ImGui::GetIO().MousePos.x - rect.Min.x - (padding * 1.5f);
        ReflectFields->ColumWidth = center;
        ReflectFields->ColumWidth = ImClamp(ReflectFields->ColumWidth, 200.0f, columWidth - 200.0f);
    }
}

void EditorAssetBrowserTool::ShowFolderHierarchy()
{
    const File::Path& root = UmFileSystem.GetRootPath();
    std::weak_ptr<File::FolderContext> rootFolder = UmFileSystem.GetContext<File::FolderContext>(root);

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
    bool  isSelected = UmFileSystem.IsSameContext(_currFocusFolderContext, FolderContext);
    float startX     = ImGui::GetCursorPosX();
    float offsetX    = 30.0f;

    if (true == isSelected)
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    // ==== Tree 출력 ====
    bool isOpen                = ImGui::TreeNodeEx((NODE_SPACING + path.string()).c_str(), flags);
    bool isHovered             = ImGui::IsItemHovered();
    bool isMouseDeoubleClicked = ImGui::IsMouseDoubleClicked(0);

    if (true == isHovered && true == isMouseDeoubleClicked)
    {
        SetFocusFolder(FolderContext);
    }

    // ==== Text출력 ====
    std::string icon = isOpen ? EditorIcon::ICON_FOLDER_OPEN : EditorIcon::ICON_FOLDER;
    std::string name = icon + " " + path.filename().string();
    ImGui::SameLine();
    ImGui::SetCursorPosX(startX + offsetX);
    ImGui::Text(name.c_str());
    ImGui::SetCursorPosX(startX + offsetX);
    ImGui::Separator();

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
    if (false == _currFocusFolderContext.expired())
    {
        spFolderContext spForcusFolder = _currFocusFolderContext.lock();

        ContentsFrameEventAction(spForcusFolder);

        //ShowSearchBar(spForcusFolder);

        switch (_showType)
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
}

void EditorAssetBrowserTool::ShowSearchBar(spFolderContext context) 
{
    float roundFactor = 3.0f;

    // ---------------------------
    // InputText의 위치 및 크기 계산
    // ---------------------------
    ImVec2 inputPos   = ImGui::GetCursorScreenPos();
    ImVec2 inputSize = ImVec2(200.0f, ImGui::GetFrameHeight()); // 높이는 자동 조절됨

    // ---------------------------
    // 버튼 크기 계산
    // ---------------------------
    ImVec2 textSize   = ImGui::CalcTextSize("x");
    ImVec2 buttonSize = textSize;
    buttonSize.x += ImGui::GetStyle().FramePadding.x * 2;
    buttonSize.y += ImGui::GetStyle().FramePadding.y * 2;


    // ---------------------------
    // 사각형 Draw
    // ---------------------------
    ImVec2 rectSize = ImVec2(inputSize.x + buttonSize.x, inputSize.y);
    ImU32 rectColor = ImGui::GetColorU32(ImGuiCol_FrameBg);
    ImGuiHelper::DrawFillRect(
        inputPos,
        inputPos + rectSize, rectColor, 
        roundFactor,
        ImDrawFlags_RoundCornersAll
    );
    
    // ---------------------------
    // 버튼 먼저 그림 (겹치게)
    // ---------------------------
    ImVec2 buttonPos = ImVec2(inputPos.x + inputSize.x, inputPos.y);
    ImGui::SetCursorScreenPos(buttonPos);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));        // 기본 배경
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0)); // 호버 시 배경
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));  // 클릭 시 배경
    if (ImGui::Button("x", buttonSize))
    {
        _searchBuffer[0] = '\0';
    }
    ImGui::PopStyleColor(3);

    // ---------------------------
    // InputText는 나중에 그림
    // ---------------------------
    ImGui::SetCursorScreenPos(inputPos);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0)); // 완전 투명
    ImGui::PushItemWidth(inputSize.x);
    ImGui::InputText("##SearchBarFrontFrame", _searchBuffer, IM_ARRAYSIZE(_searchBuffer));
    ImGui::PopItemWidth();
    ImGui::PopStyleColor();

    ImGuiHelper::Separator();
}

void EditorAssetBrowserTool::ContentsFrameEventAction(spFolderContext context)
{
    if (nullptr == context)
        return;

    const File::Path& curPath = context->GetPath();

    bool isSelected   = UmFileSystem.IsSameContext(_selectedContext->GetContext(), context);
    bool isRename     = browserFlags[FLAG_IS_RENAME];
    bool isItemActive = ImGui::IsItemActive();  // 셀렉터블이 눌렸는지
    bool isHovered    = ImGui::IsItemHovered(); // 셀렉터블이 호버링 되었는지

    bool isClickedLeft  = isHovered && ImGui::IsMouseClicked(0); // 마우스 왼 클릭
    bool isClickedRight = isHovered && ImGui::IsMouseClicked(1); // 마우스 오른 클릭

    bool isMouseDouble  = ImGui::IsMouseDoubleClicked(0);                 // 마우스 더블 클릭
    bool iskeyEnter     = ImGui::IsKeyPressed(ImGuiKey_Enter, false);     // 엔터키 눌림
    bool isKeyBackSpace = ImGui::IsKeyPressed(ImGuiKey_Backspace, false); // 백스페이스 눌림
    bool isKeyEscape    = ImGui::IsKeyPressed(ImGuiKey_Escape, false);    // ESC키 눌림
    bool isKeyDelete    = ImGui::IsKeyPressed(ImGuiKey_Delete, false);    // DEL키 눌림

    bool ctrl       = ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl);
    bool c          = ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_C, false);
    bool v          = ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_V, false);
    bool isKeyCopy  = ctrl && c; // Ctrl + C
    bool isKeyPaste = ctrl && v; // Ctrl + V

    if (nullptr != context)
    {
        DragDropTransform::Data data;
        if (ImGuiHelper::DragDrop::RecieveFrameDragDropEvent(DragDropTransform::KEY, &data))
        {
            std::filesystem::path path = context->GetPath();
            path = std::filesystem::relative(path, UmFileSystem.GetAssetPath());
            UmGameObjectFactory.WriteGameObjectFile(data.pTransform, path.string());
        }

        if (false == isRename)
        {
            if (true == isKeyBackSpace)
            {
                SetFocusParentFolder(context);
            }
            if (true == isKeyPaste)
            {
                if (true == fs::exists(_copyPath))
                {
                    File::Path from = _copyPath;
                    File::Path to   = (curPath / from.filename());

                    File::CopyFileFromTo(from, to);
                }
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
                    File::CreateFolderEx(curPath / "New Folder", true);
                }
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Copy Path"))
            {
                File::CopyPathToClipBoard(curPath);
            }
            ImGui::EndPopup();
        }

        ImGuiHelper::PopStyleCompact();
    }
}

void EditorAssetBrowserTool::ShowContentsToList()
{
    File::Path parentPath     = _currFocusFolderPath.parent_path().generic_wstring();
    auto       wpParentFolder = UmFileSystem.GetContext<File::FolderContext>(parentPath);
    if (false == wpParentFolder.expired())
    {
        auto spParentContext = wpParentFolder.lock();
        ShowItemToList(spParentContext, "parent");
    }

    // 참조 포인터가 살아있을 때
    if (false == _currFocusFolderContext.expired())
    {
        auto spFocusCtx = _currFocusFolderContext.lock();

        for (auto itr = spFocusCtx->begin(); itr != spFocusCtx->end();)
        {
            auto& [name, wpFileCtx] = *itr++;
            if (false == wpFileCtx.expired())
            {
                auto spFileCtx = wpFileCtx.lock();

                File::Path  path = spFileCtx->GetPath();
                std::string id   = name.string() + "##" + path.string();

                bool isMeta = path.extension() == UmFileSystem.GetMetaExt();

                ShowItemToList(spFileCtx);
            }
        }
    }
}

void EditorAssetBrowserTool::ShowContentsToIcon() {}

void EditorAssetBrowserTool::ShowItemToList(spContext context, const char* mode)
{
    if (nullptr == context)
        return;

    bool isSelected = UmFileSystem.IsSameContext(_selectedContext->GetContext(), context);
    bool isRename   = browserFlags[FLAG_IS_RENAME];
    bool isParent   = (0 == strcmp(mode, "parent"));

    float startX   = ImGui::GetCursorPosX();
    float fontSize = ImGui::GetFontSize();

    const File::Path&  path = context->GetPath();
    const std::string  icon = context->IsDirectory() ? EditorIcon::ICON_FOLDER : EditorIcon::ICON_FILE;
    const std::string& name = isParent ? "../" : context->GetName();

    ImGui::PushID(context.get());

    ImGui::Selectable("##Selectable", isSelected, 0);
    {
        ItemEventAction(context, mode);
        ItemInputAction(context, mode);

        ImGui::SameLine(ImGui::GetCursorPosX());
        ImGui::Text(icon.c_str());
        ImGui::SameLine(startX + fontSize);
    }

    if (true == isSelected && true == isRename && false == isParent)
    {
        ItemInputText(context);
    }
    else
    {
        ImGui::Text(name.c_str());
        ItemPopupAction(context, mode);
    }

    ImGui::Separator();

    ImGui::PopID();
}

void EditorAssetBrowserTool::ShowItemToIcon(spContext context, const char* mode) {}

void EditorAssetBrowserTool::ItemInputText(spContext context)
{
    static char buffer[128] = "";
    static bool init        = false;

    const std::string&  name  = context->GetName();
    const File::Path&   path  = context->GetPath();
    ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f)); // Y 패딩만 약간 줘서 커서 보이게
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));       // 투명 배경

    if (false == init)
    {
        ImGui::SetKeyboardFocusHere();
        strcpy_s(buffer, name.c_str());
        init = true;
    }

    if (ImGui::InputText("##InputText", buffer, IM_ARRAYSIZE(buffer), flags))
    {
        fs::path newPath = path;
        newPath.replace_filename(buffer);
        context->Move(newPath);
        browserFlags[FLAG_IS_RENAME] = false;
    }

    if (ImGui::IsItemDeactivated())
    {
        browserFlags[FLAG_IS_RENAME] = false;
        init                         = false;
    }

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}

void EditorAssetBrowserTool::ItemEventAction(spContext context, const char* mode)
{
    bool isParent = (0 == strcmp(mode, "parent"));

    if (true == context->IsDirectory())
    {
        auto spFolderContext = std::static_pointer_cast<File::FolderContext>(context);

        const char* eventID = DragDropAsset::KEY;
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(eventID))
            {
                DragDropAsset::Data data = (*(DragDropAsset::Data*)payload->Data);
                _eventFunc.emplace_back([=]() { ProcessMoveAction(*data.pContext, spFolderContext); });
            }
            ImGui::EndDragDropTarget();
        }
    }
    if (false == isParent)
    {
        const char* eventID = DragDropAsset::KEY;
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
        {
            DragDropAsset::Data                 data;
            static std::weak_ptr<File::Context> dataContext;
            dataContext   = context;
            data.pContext = &dataContext;
            ImGui::SetDragDropPayload(eventID, &data, sizeof(DragDropAsset::Data));
            File::Path path = context->GetPath();
            ImGui::Text(path.string().c_str());
            ImGui::EndDragDropSource();
        }
    }
}

void EditorAssetBrowserTool::ItemInputAction(spContext context, const char* mode)
{
    auto& io = ImGui::GetIO();

    bool isParent       = (0 == strcmp(mode, "parent"));
    bool isSelected     = UmFileSystem.IsSameContext(_selectedContext->GetContext(), context);
    bool isRename       = browserFlags[FLAG_IS_RENAME];
    bool isFrameFocused = ImGui::IsWindowFocused();               // 윈도우 포커스 여부
    bool isItemActive   = ImGui::IsItemActive();                  // 셀렉터블이 눌렸는지
    bool isItemHovered  = ImGui::IsItemHovered();                 // 셀렉터블이 호버링 되었는지
    bool isItemFocused  = ImGui::IsItemFocused() && io.NavActive; // 셀렉터블이 포커스 되었는지

    bool isClickedLeft  = isItemHovered && ImGui::IsMouseReleased(0); // 마우스 왼 클릭
    bool isClickedRight = isItemHovered && ImGui::IsMouseReleased(1); // 마우스 오른 클릭

    bool isMouseDouble  = isItemFocused && ImGui::IsMouseDoubleClicked(0);              // 마우스 더블 클릭
    bool iskeyEnter     = isItemFocused && ImGui::IsKeyPressed(ImGuiKey_Enter, false);  // 엔터키 눌림
    bool isKeyEscape    = isItemFocused && ImGui::IsKeyPressed(ImGuiKey_Escape, false); // ESC키 눌림
    bool isKeyDelete    = isItemFocused && ImGui::IsKeyPressed(ImGuiKey_Delete, false); // DEL키 눌림
    bool isKeyF2        = isItemFocused && ImGui::IsKeyPressed(ImGuiKey_F2, false);     // 백스페이스 눌림
    bool isKeyArrowDown = isItemFocused && ImGui::IsKeyDown(ImGuiKey_DownArrow);
    bool isKeyArrowUp   = isItemFocused && ImGui::IsKeyDown(ImGuiKey_UpArrow);
    bool isKeyArrow     = isKeyArrowDown || isKeyArrowUp;

    bool ctrl       = ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl);
    bool c          = ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_C, false);
    bool v          = ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_V, false);
    bool isKeyCopy  = ctrl && c; // Ctrl + C
    bool isKeyPaste = ctrl && v; // Ctrl + V

    if (true == isItemFocused)
    {
        if (true == isMouseDouble || true == iskeyEnter)
        {
            ProcessEnterAction(context);
        }
        if (true == isClickedLeft || true == isClickedRight || true == isKeyArrow)
        {
            SetFocusInspector(context);
        }
        if (false == isParent)
        {
            if (true == isRename)
            {
                if (true == isKeyEscape)
                {
                    browserFlags[FLAG_IS_RENAME] = false;
                }
            }
            else if (false == isRename)
            {
                if (true == isKeyF2)
                {
                    browserFlags[FLAG_IS_RENAME] = true;
                }

                if (true == isKeyDelete)
                {
                    Global::editorModule->OpenPopupBox("RemoveAsset", [&, context]() { ShowDeletePopupBox(context); });
                }

                if (true == isKeyCopy)
                {
                    _copyPath = context->GetPath();
                    UmFileSystem.RequestCopyFile(_copyPath);
                }
            }
        }
    }
}

void EditorAssetBrowserTool::ItemPopupAction(spContext context, const char* mode)
{
    bool isParent = (0 == strcmp(mode, "parent"));

    if (false == isParent)
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
                browserFlags[FLAG_IS_RENAME] = true;
                ImGui::CloseCurrentPopup(); // 팝업 닫기
            }
            ImGui::EndPopup();
        }
    }
}

void EditorAssetBrowserTool::ShowDeletePopupBox(wpContext context)
{
    bool isSelected = UmFileSystem.IsSameContext(_selectedContext->GetContext(), context);

    ImGui::Text(u8"정말 삭제하시겠습니까?"_c_str);

    if (false == context.expired())
    {
        auto       spContext = context.lock();
        File::Path path      = spContext->GetPath();
        ImGui::Text(u8"경로: "_c_str);
        ImGui::SameLine();
        ImGui::Text(path.string().c_str());

        ImGui::PushID(this);

        if (ImGui::Button("OK##") || ImGui::IsKeyReleased(ImGuiKey::ImGuiKey_Enter))
        {
            SetFocusParentFolder(spContext);
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

void EditorAssetBrowserTool::ShowSameFilePopupBox()
{
    ImGui::Text(u8"해당 경로에 중복된 파일 이름이 있습니다."_c_str);

    if (ImGui::Button("OK##") || ImGui::IsKeyReleased(ImGuiKey::ImGuiKey_Enter))
    {
        ImGui::CloseCurrentPopup();
    }
}

void EditorAssetBrowserTool::ProcessEnterAction(spContext context)
{
    if (nullptr != context)
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
            UmFileSystem.RequestOpenFile(context->GetPath());
        }
    }
}

void EditorAssetBrowserTool::ProcessMoveAction(wpContext srcContext, wpFolderContext dstContext)
{
    bool isSrcExpired = srcContext.expired();
    bool isDstExpired = dstContext.expired();

    if (false == isSrcExpired && false == isDstExpired)
    {
        auto spSrcContext = srcContext.lock();
        auto spDstContext = dstContext.lock();

        auto& from = spSrcContext->GetPath();
        auto& to   = spDstContext->GetPath();

        // 해당 폴더에 파일 이름을 붙임
        File::Path newPath = to / from.filename();
        newPath            = newPath.generic_wstring();

        if (true == fs::exists(newPath))
        {
            Global::editorModule->OpenPopupBox("SameFile", [this]() { ShowSameFilePopupBox(); });
        }
        else
        {
            spSrcContext->Move(newPath);
        }
    }
}

void EditorAssetBrowserTool::SetFocusInspector(wpContext context)
{
    _selectedContext->SetContext(context);
}

bool EditorAssetBrowserTool::SetFocusFolder(wpFolderContext context)
{
    if (false == context.expired())
    {
        auto spContext = context.lock();
        if (false == _currFocusFolderContext.expired())
        {
            auto  spFolderContext = _currFocusFolderContext.lock();
            auto& path            = spFolderContext->GetPath();
            _directoryUndoStack.push_back(path);
            if (_directoryUndoStack.size() > _maxUndoStack)
            {
                _directoryUndoStack.pop_front();
            }
        }
        _directoryRedoStack.clear();
        _nextFocusFolderContext = context;
        return true;
    }
    else
    {
        const File::Path rootPath = UmFileSystem.GetRootPath();
        _nextFocusFolderContext   = UmFileSystem.GetContext<File::FolderContext>(rootPath);
        _directoryUndoStack.clear();
        _directoryRedoStack.clear();
        return false;
    }
}

void EditorAssetBrowserTool::SetFocusParentFolder(spContext context)
{
    if (nullptr != context)
    {
        const File::Path& curPath       = context->GetPath();
        const File::Path  parentPath    = curPath.parent_path();
        const auto        parentContext = UmFileSystem.GetContext<File::FolderContext>(parentPath);
        SetFocusFolder(parentContext);
    }
}

void EditorAssetBrowserTool::SetFocusFromUndoPath()
{
    if (false == _directoryUndoStack.empty())
    {
        const File::Path& undoPath    = _directoryUndoStack.back();
        auto              undoContext = UmFileSystem.GetContext<File::FolderContext>(undoPath);
        if (false == undoContext.expired())
        {
            if (false == _currFocusFolderContext.expired())
            {
                const File::Path& path = _currFocusFolderContext.lock()->GetPath();
                _directoryRedoStack.push_back(path);
            }
            _nextFocusFolderContext = undoContext;
        }
        _directoryUndoStack.pop_back();
    }
}

void EditorAssetBrowserTool::SetFocusFromRedoPath()
{
    if (false == _directoryRedoStack.empty())
    {
        const File::Path& curPath     = _directoryRedoStack.back();
        auto              redoContext = UmFileSystem.GetContext<File::FolderContext>(curPath);
        if (false == redoContext.expired())
        {
            if (false == _currFocusFolderContext.expired())
            {
                const File::Path& path = _currFocusFolderContext.lock()->GetPath();
                _directoryUndoStack.push_back(path);
            }
            _nextFocusFolderContext = redoContext;
        }
        _directoryRedoStack.pop_back();
    }
}

void EditorAssetObject::OnInspectorStay()
{
    bool isDebug   = Global::editorModule->IsDebugMode();
    bool isExpired = _focusedInspector.expired();

    if (false == isExpired && true == isDebug)
    {
        auto  spContext = _focusedInspector.lock();
        auto& metaData  = spContext->GetMeta();

        ImGui::Text("Path: %s", spContext->GetPath().string().c_str());
        ImGui::Text("Guid: %s", metaData.GetGuid().string().c_str());
        ImGui::Separator();

        auto& path = spContext->GetPath();
        UmFileSystem.RequestInspectFile(path);
    }
}

void EditorAssetObject::SetContext(std::weak_ptr<File::Context> context) 
{
    _selectedAsset = context;

    if (true == EditorInspectorTool::SetFocusObject(_this))
    {
        _focusedInspector = context;
    }
}
