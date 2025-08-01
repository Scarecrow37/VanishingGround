#include "pch.h"

namespace fs = std::filesystem;
using namespace u8_literals;


EditorAssetBrowserTool::EditorAssetBrowserTool()
{
    SetLabel("AssetBrowser");
    SetDockLayout(ImGuiDir_Down);

    //_selectedContext = std::make_shared<EditorAssetObject>();
    //_selectedContext->SetThis(_selectedContext);

    ReflectFields->ShowType = SHOW_TYPE_ICON;

    _staticInstance = this;
}

EditorAssetBrowserTool::~EditorAssetBrowserTool() 
{
    if (this == _staticInstance)
    {
        _staticInstance = nullptr;
    }
}

EditorAssetBrowserTool* EditorAssetBrowserTool::GetInstance()
{
    return _staticInstance;
}

void EditorAssetBrowserTool::OnTickGui() 
{
    _updateTime += UmTime.UnscaledDeltaTime();
}

void EditorAssetBrowserTool::OnStartGui()
{
    const MessageHandler msgHandler(WinProc, 0);
    UmApplication.AddMessageHandler(msgHandler);
}

void EditorAssetBrowserTool::OnPreFrameBegin()
{
    _windowRect = ImRect();
}

void EditorAssetBrowserTool::OnPostFrameBegin() 
{
    ImVec2 contentMin = ImGui::GetWindowContentRegionMin(); // 보통 (0, 0)
    ImVec2 contentMax = ImGui::GetWindowContentRegionMax(); // 보통 (width, height)
    ImVec2 pos        = ImGui::GetWindowPos();
    _windowRect       = ImRect(pos + contentMin, pos + contentMax);
}

void EditorAssetBrowserTool::OnFrameRender()
{
    RefreshState();
    if (_updateTime >= 0.5f || _needRefresh)
    {
        _updateTime     = 0.0f;
        _needRefresh    = false;
        RefreshFocusFolderEntries();
    }
    ImGui::PushID(this);

    ShowUpperFrame();

    // Left, Right 구분 창
    BeginColumn();
    {
        // 왼쪽: 폴더 트리
        ImGui::BeginChild("FolderHierarchyFrame", ImVec2(ReflectFields->ColumWidth, ReflectFields->ColumHeight), ImGuiChildFlags_Border);
        {
            ShowFolderHierarchy();
        }
        ImGui::EndChild();

        ShowColumnPlitter();
        ImGui::SameLine();

        // 오른쪽: 선택한 폴더의 파일 목록
        ImGui::BeginChild("ContentsFrame", ImVec2(0, ReflectFields->ColumHeight), ImGuiChildFlags_Border);
        {
            ShowFolderEntries();
        }
        ImGui::EndChild();
    }
    EndColumn();

    for (auto& func : _delayEvent)
    {
        if (nullptr != func)
            func();
    }
    _delayEvent.clear();

    ImGui::PopID();
}

void EditorAssetBrowserTool::OnFrameEnd()
{
}

void EditorAssetBrowserTool::OnFrameFocusEnter() 
{
}

void EditorAssetBrowserTool::OnFrameFocusStay() 
{
}

void EditorAssetBrowserTool::OnFrameFocusExit() 
{
}

void EditorAssetBrowserTool::ShowUpperFrame()
{
    ImGuiChildFlags flags = ImGuiChildFlags_Border;
    ImGui::BeginChild("UpperFrame", ImVec2(0, 30.0f), flags, ImGuiWindowFlags_NoScrollbar);
    {
        ImVec2 windowPos  = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 availableSize = ImGui::GetContentRegionAvail();

        bool isShowList = (ReflectFields->ShowType == SHOW_TYPE_LIST);
        if (ImGui::Checkbox("Show List", &isShowList))
        {
            ReflectFields->ShowType = isShowList ? SHOW_TYPE_LIST : SHOW_TYPE_ICON;
            _needRefresh = true; // 새로고침 필요
        }
    }
    ImGui::EndChild();
}

void EditorAssetBrowserTool::ShowFolderDirectoryPath(spFolderContext context)
{
    //if (nullptr != context)
    //{
    //    const File::Path& path = context->GetPath();
    //    const File::Path& root = UmFileSystem.GetRootPath();
    //
    //    bool canUndo = (false == _directoryUndoStack.empty());
    //    bool canRedo = (false == _directoryRedoStack.empty());
    //
    //    ImVec2 windowSize = ImGui::GetWindowSize();
    //    ImVec2 pos        = ImGui::GetCursorPos();
    //    {
    //       
    //        const char* icon  = EditorIcon::ICON_CIRCLE_ARROW_LEFT;
    //        ImVec2      size  = ImGui::CalcTextSize(icon);
    //        int         flags = canUndo ? ImGuiSelectableFlags_None : ImGuiSelectableFlags_Disabled;
    //
    //        if (ImGui::Selectable(icon, false, flags, size))
    //        {
    //            SetFocusFromUndoPath();
    //        }
    //        ImGui::SameLine(0.0f, 10.0f);
    //    }
    //    {
    //        const char* icon  = EditorIcon::ICON_CIRCLE_ARROW_RIGHT;
    //        ImVec2      size  = ImGui::CalcTextSize(icon);
    //        int         flags = canRedo ? ImGuiSelectableFlags_None : ImGuiSelectableFlags_Disabled;
    //
    //        if (ImGui::Selectable(icon, false, flags, size))
    //        {
    //            SetFocusFromRedoPath();
    //        }
    //        ImGui::SameLine(0.0f, 10.0f);
    //    }
    //
    //    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 230, 200, 255));
    //
    //    std::string icon = EditorIcon::ICON_FOLDER_OPEN;
    //    ImGui::Text(icon.c_str());
    //    ImGui::SameLine(0.0f, 5.0f);
    //
    //    ListToDirectoryFileName(L".");
    //
    //    // 상대경로로 부모 폴더 계산
    //    File::Path relativePath;
    //    relativePath = fs::relative(path, root);
    //
    //    File::Path node;
    //    for (auto itr = relativePath.begin(); itr != relativePath.end(); ++itr)
    //    {
    //        if ((*itr) == L".")
    //            continue;
    //
    //        ImGui::SameLine();
    //        ImGui::Text("/");
    //        ImGui::SameLine();
    //
    //        File::Path folderName = fs::absolute((*itr)).filename();
    //        node /= folderName;
    //        ListToDirectoryFileName(node);
    //    }
    //}
    //else
    //{
    //    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 100, 100, 255));
    //    File::Path curPath = _currFocusFolderPath;
    //    while (false == fs::exists(curPath))
    //    {
    //        curPath = curPath.parent_path();
    //
    //        auto context = UmFileSystem.GetContext<File::FolderContext>(curPath);
    //        if (false == context.expired())
    //        {
    //            SetFocusFolder(context);
    //        }
    //        else
    //        {
    //            if (true == curPath.empty())
    //            {
    //                break;
    //            }
    //            continue;
    //        }
    //    }
    //}
    //
    //ImGui::PopStyleColor();
}

void EditorAssetBrowserTool::ListToDirectoryFileName(const File::Path& relativePath)
{
    //auto& root = UmFileSystem.GetRootPath();
    //
    //File::Path  absPath    = fs::absolute(relativePath);
    //File::Path  folderName = absPath.filename();
    //std::string nameStr    = folderName.string();
    //
    //ImVec2 textSize = ImGui::CalcTextSize(nameStr.c_str());
    //float  startX   = ImGui::GetCursorPosX();
    //
    //auto wpFolderContext = UmFileSystem.GetContext<File::FolderContext>(absPath);
    //auto spFolderContext = wpFolderContext.lock(); // 실패하면 개버그니까 그냥 과감하게 lock
    //
    //ImGui::PushID(spFolderContext.get());
    //
    //ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 0)); // 기본 배경 투명
    //if (ImGui::Selectable(nameStr.c_str(), false, 0, textSize))
    //{
    //    SetFocusFolder(wpFolderContext);
    //}
    //
    //const char* eventID = DragDropAsset::KEY;
    //if (ImGui::BeginDragDropTarget())
    //{
    //    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(eventID))
    //    {
    //        DragDropAsset::Data data = (*(DragDropAsset::Data*)payload->Data);
    //        //_delayEvent.emplace_back([this, data]() { MoveEntry(data.Path, spFolderContext); });
    //    }
    //    ImGui::EndDragDropTarget();
    //}
    //
    //ImGui::PopStyleColor();
    //
    //ImGui::PopID();
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
    _folderCount = 0;
    ShowFolderHierarchyTree(root);
}

#define NODE_SPACING "                                                                ##"
void EditorAssetBrowserTool::ShowFolderHierarchyTree(const File::Path& directory)
{
    if (fs::exists(directory) && fs::is_directory(directory))
    {
        ++_folderCount;
        std::string filename    = directory.filename().string();
        float cursorX           = ImGui::GetCursorPosX();
        bool isFocusedFolder    = _focusFolderPath == directory;
        int treeFlags           = ImGuiTreeNodeFlags_OpenOnArrow;
        treeFlags |= isFocusedFolder ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None;

        // ==== Tree 출력 ====
        ImGui::PushID(_folderCount);
        bool isTreeOpen         = ImGui::TreeNodeEx(NODE_SPACING, treeFlags);
        bool isHovered          = ImGui::IsItemHovered();
        bool isDoubleClicked    = ImGui::IsMouseDoubleClicked(0);

        if (isHovered && isDoubleClicked)
        {
            SetFocusFolderPath(directory);
        }

        ImGui::SameLine();
        // ==== Text출력 ====
        const char* icon  = isTreeOpen ? EditorIcon::ICON_FOLDER_OPEN : EditorIcon::ICON_FOLDER;
        std::string label = std::format("{} {}", icon, filename);
        ImGui::SetCursorPosX(cursorX + 30.0f);
        ImGui::Text(label.c_str());

        if (isTreeOpen)
        {
            for (const auto& entry : fs::directory_iterator(directory))
            {
                ShowFolderHierarchyTree(entry.path());
            }
            ImGui::TreePop();
        }
        ImGui::PopID();
    }
}

// 오른쪽: 선택된 폴더의 파일 목록
void EditorAssetBrowserTool::ShowFolderEntries()
{
    if (fs::exists(_focusFolderPath) && fs::is_directory(_focusFolderPath))
    {
        UpdateFolderEntryInput();
        int showType = ReflectFields->ShowType;
        int pushStyleVar = 0;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(15.0f, 4.0f)); ++pushStyleVar;// 아이콘 간격 조정
        for (auto& asset : _focusFolderAssetDataList)
        {
            switch (showType)
            {
                case EditorAssetBrowserTool::SHOW_TYPE_LIST:
                    ShowFolderEntryToList(*asset);
                    break;
                case EditorAssetBrowserTool::SHOW_TYPE_ICON:
                    ShowFolderEntryToIcon(*asset);
                    break;
                default:
                    break;
            }
        }
        ImGui::PopStyleVar(pushStyleVar);
    }
}

void EditorAssetBrowserTool::ShowFolderEntryToList(AssetData& asset)
{
    auto& io = ImGui::GetIO();

    auto  window = ImGui::GetCurrentWindow();
    float oldFontScale = window->FontWindowScale;
    ImGui::SetWindowFontScale(_zoomScale);

    bool isItemClickedLeft  = false;
    bool isItemClickedRight = false;
    bool isItemFocusedNav   = false;
    bool isItemDoubleClicked = false;

    bool   isFocused  = (_focusEntryPath == asset.Entry.path());
    bool   isRenaming = _rename.IsActive() && isFocused;
    float  fontSize   = ImGui::GetFontSize();
    float  textHeight = ImGui::GetTextLineHeightWithSpacing();
    ImVec2 availSpace = ImGui::GetContentRegionAvail();
    ImVec2 widgetSize = ImVec2(availSpace.x, textHeight);
    ImVec2 cursorPos  = ImGui::GetCursorPos();
    ImVec4 bgCol      = isFocused ? ImGui::GetStyleColorVec4(ImGuiCol_Header) : ImVec4(0, 0, 0, 0);

    ImGui::PushID(&asset);

    int flags           = ImGuiSelectableFlags_AllowDoubleClick;
    isItemClickedLeft   |= ImGui::Selectable("##selectable", isFocused, flags, widgetSize);
    isItemClickedRight  |= ImGui::IsItemClicked(ImGuiMouseButton_Right) && !isRenaming;
    isItemFocusedNav    |= ImGui::IsItemFocused() && io.NavActive;
    isItemDoubleClicked |= ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && !isRenaming;
    if (isItemDoubleClicked)
    {
        if (asset.IsDirectory)
        {
            // 폴더 열기
            File::Path path = asset.Entry.path();
            _delayEvent.emplace_back([this, path]() {
                File::Path focusPath = path;
                SetFocusFolderPath(focusPath);
            });
        }
        else
        {
            // 파일 열기
            UmFileSystem.RequestOpenFile(asset.Entry.path());
        }
    }
    ProcessFolderEntryDragDrop(asset);

    ImGui::SameLine(ImGui::GetCursorPosX());
    const char* icon = asset.IsDirectory ? EditorIcon::ICON_FOLDER : EditorIcon::ICON_FILE;
    ImGui::Text(icon);
    ImGui::SameLine(cursorPos.x + fontSize);
    if (isRenaming)
    {
        ImGuiHelper::StyleBuilder textStyle;
        textStyle.PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f); // 테두리 두께 
        textStyle.PushStyleColor(ImGuiCol_Border, IM_COL32(100, 100, 255, 255));
        int flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll; 
        //ImGui::SetKeyboardFocusHere();
        if (ImGui::InputText("##name", &_rename.RenameBuffer, flags) && isRenaming)
        {
            const auto& path = _rename.ExecuteRename(_focusFolderPath);
            SetFocusEntryPath(path);
        }
        ImGuiHelper::HoveredToolTip(asset.FileName.c_str());
        textStyle.PopStyle();
    }
    else
    {
        ImGui::Text(asset.FileName.c_str());
    }
    if (isItemClickedLeft || isItemFocusedNav)
    {
        SetFocusEntryPath(asset.Entry.path());
    }
    if (isItemClickedRight)
    {
        ImGui::OpenPopup(POPUP_ID);
    }
    ShowFolderEntryPopup(asset);

    ImGui::PopID();

    ImGui::SetWindowFontScale(oldFontScale);
    ImGuiHelper::Separator(4.0f);
}

void EditorAssetBrowserTool::ShowFolderEntryToIcon(AssetData& asset)
{
    auto& io = ImGui::GetIO();
    
    bool isItemClickedLeft  = false;
    bool isItemClickedRight = false;
    bool isItemFocusedNav   = false;

    bool  isFocused         = (_focusEntryPath == asset.Entry.path());
    bool  isRenaming        = _rename.IsActive() && isFocused;
    float rounding          = 6.0f;

    float textWidgetHeight  = ImGui::GetTextLineHeightWithSpacing();
    ImVec2 iconWidgetSize   = ICON_WIDGET_SIZE * _zoomScale;
    ImVec2 widgetSize       = iconWidgetSize + ImVec2(0.0f, textWidgetHeight);

    ImGui::PushID(&asset);
    ImGui::BeginGroup();
    ImVec2 oldCursorPos = ImGui::GetCursorPos();
    {
        // 라운딩 스타일
        ImGuiHelper::StyleBuilder widgetStyle;
        widgetStyle.PushStyleVar(ImGuiStyleVar_FrameRounding, rounding); 
        ImVec4 bgCol = isFocused ? ImGui::GetStyleColorVec4(ImGuiCol_Header) : ImVec4(0, 0, 0, 0);

        // 아이콘
        D3D12_GPU_DESCRIPTOR_HANDLE icon = asset.PreviewIconTexture->GetGPUHandle();
        ImGuiHelper::StyleBuilder iconStyle;
        iconStyle.PushStyleColor(ImGuiCol_Button, bgCol);
        isItemClickedLeft   |= ImGui::ImageButton("##icon", (ImTextureID)icon.ptr, iconWidgetSize);
        isItemClickedRight  |= ImGui::IsItemClicked(ImGuiMouseButton_Right) && !isRenaming;
        isItemFocusedNav    |= ImGui::IsItemFocused() && io.NavActive;
        iconStyle.PopStyle();
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            if (asset.IsDirectory)
            {
                // 폴더 열기
                File::Path path = asset.Entry.path();
                _delayEvent.emplace_back([this, path]() {
                    File::Path focusPath = path;
                    SetFocusFolderPath(focusPath);
                });
            } 
            else
            {
                // 파일 열기
                UmFileSystem.RequestOpenFile(asset.Entry.path());
            }
        }
        ProcessFolderEntryDragDrop(asset);
        // 텍스트 
        ImGuiHelper::StyleBuilder textStyle;
        if (isRenaming)
        {
            // 평범하게 정렬
            textStyle.PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f); // 테두리 두께 
            textStyle.PushStyleColor(ImGuiCol_Border, IM_COL32(100, 100, 255, 255));
            ImGui::SetNextItemWidth(widgetSize.x);
        }
        else
        {
            // 가운데 정렬
            float textWidth  = ImGui::CalcTextSize(asset.ViewName.c_str()).x;
            float textOffset = std::max(0.0f, (iconWidgetSize.x - textWidth) * 0.5f);
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + textOffset);
            ImGui::SetNextItemWidth(textWidth + 8.0f);
            textStyle.PushStyleColor(ImGuiCol_FrameBg, bgCol);
        }
        int flags = isRenaming 
            ? ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll
            : ImGuiInputTextFlags_ReadOnly;
        std::string* buffer = isRenaming ? &_rename.RenameBuffer : &asset.ViewName;
        //ImGui::SetKeyboardFocusHere();
        if (ImGui::InputText("##name", buffer, flags) && isRenaming)
        {
            const auto& path = _rename.ExecuteRename(_focusFolderPath);
            SetFocusEntryPath(path);
        }
        isItemClickedLeft   |= ImGui::IsItemClicked(ImGuiMouseButton_Left) && !isRenaming;      // 좌 클릭 되었는지
        isItemClickedRight  |= ImGui::IsItemClicked(ImGuiMouseButton_Right) && !isRenaming;     // 우 클릭 되었는지
        isItemFocusedNav    |= ImGui::IsItemFocused() && io.NavActive;

        ImGuiHelper::HoveredToolTip(asset.FileName.c_str());
        textStyle.PopStyle();
        widgetStyle.PopStyle();
    }
    ImGui::EndGroup();

    if (isItemClickedLeft || isItemFocusedNav)
    {
        SetFocusEntryPath(asset.Entry.path());
    }
    if (isItemClickedRight)
    {
        ImGui::OpenPopup(POPUP_ID);
    }
    ShowFolderEntryPopup(asset);
    
    ImGui::PopID();
    // 충분한 공간이 있으면 같은 줄로 고정 
    ImVec2 availableSize = ImGui::GetContentRegionAvail();
    if (availableSize.x < oldCursorPos.x + widgetSize.x * 2.0f)
    {
        ImGui::NewLine();
    }
    else
    {
        ImGui::SameLine();
    }
}

void EditorAssetBrowserTool::ShowFolderEntryPopup(AssetData& asset) 
{
    if (ImGui::BeginPopup(POPUP_ID))
    {
        if (ImGui::MenuItem("Open"))
        {
            UmFileSystem.RequestOpenFile(asset.Entry.path());
        }
        if (ImGui::MenuItem("Copy"))
        {
            _copyBuffer = asset.Entry.path();
            UmFileSystem.RequestCopyFile(_copyBuffer);
        }
        if (ImGui::MenuItem("Rename"))
        {
            _rename.StartRename(asset.Entry.path());
        }
        if (ImGui::MenuItem("Delete"))
        {
            File::RemoveFile(asset.Entry.path());
            _needRefresh = true; // 삭제 후 새로고침 필요
        }
        if (ImGui::MenuItem("Copy Path"))
        {
            File::CopyPathToClipBoard(asset.Entry.path());
        }
        ImGui::EndPopup();
    }
}

void EditorAssetBrowserTool::ProcessFolderEntryDragDrop(AssetData& asset)
{
    if (true == asset.IsDirectory)
    {
        const char* eventID = DragDropAsset::KEY;
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(eventID))
            {
                DragDropAsset::Data data = (*(DragDropAsset::Data*)payload->Data);
                File::Path fromPath = data.GetPath();
                File::Path toPath   = asset.Entry.path() / fromPath.filename();
                _delayEvent.emplace_back([this, fromPath, toPath]()
                    {
                        if (fs::exists(fromPath))
                        {
                            fs::rename(fromPath, toPath);
                        }
                    });
            }
            ImGui::EndDragDropTarget();
        }
    }

    const char* eventID = DragDropAsset::KEY;
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
    {
        DragDropAsset::Data data;
        File::Path entryPath = asset.Entry.path();
        static std::weak_ptr<File::Context> context;
        static File::Path pPath;
        static File::Guid pGuid;
        context = UmFileSystem.GetContext<File::Context>(entryPath);
        pGuid = pPath = entryPath;
        data.pContext = &context;
        data.pPath    = &pPath;
        data.pGuid    = &pGuid;
        ImGui::SetDragDropPayload(eventID, &data, sizeof(DragDropAsset::Data));
        ImGui::Text(entryPath.string().c_str());
        ImGui::EndDragDropSource();
    }
}

void EditorAssetBrowserTool::UpdateFolderEntryInput() 
{
    bool isRootpath         = (_focusFolderPath == UmFileSystem.GetRootPath());

    bool isKeyDelete        = ImGui::IsKeyPressed(ImGuiKey_Delete, false); 
    bool isKeyDBackSpace    = ImGui::IsKeyPressed(ImGuiKey_Backspace, false);
    bool isKeyEsc           = ImGui::IsKeyPressed(ImGuiKey_Escape, false);
    bool isKeyCtrl          = ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl);
    bool isKeyF2            = ImGui::IsKeyPressed(ImGuiKey_F2, false);
    bool isKeyC             = ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_C, false);
    bool isKeyV             = ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_V, false);

    AssetData* focusAssetData = GetAssetData(_focusEntryPath);

    if (focusAssetData)
    {
        if (isKeyDBackSpace)
        {
            if (false == _rename.IsActive() && false == isRootpath)
            {
                File::Path parentPath = _focusFolderPath.parent_path();
                SetFocusFolderPath(parentPath);
            }
        }
        else if (isKeyF2)
        {
            if (false == _rename.IsActive())
            {
                _rename.StartRename(focusAssetData->Entry.path());
            }
        }
        else if (isKeyEsc)
        {
            if (true == _rename.IsActive())
            {
                _rename.CancelRename();
            }
        }
        else if (isKeyDelete)
        {
            _rename.CancelRename();
            File::RemoveFile(_focusEntryPath);
        }
    }
    if (isKeyCtrl)
    {
        float wheelY = ImGui::GetIO().MouseWheel;
        if (wheelY != 0.0f)
        {
            _zoomScale += wheelY * 0.1f;
            _zoomScale   = ImClamp(_zoomScale, 0.5f, 2.0f);
            _needRefresh = true;
        }
        if (isKeyC)
        {
            _copyBuffer = _focusEntryPath;
            UmFileSystem.RequestCopyFile(_copyBuffer);
        }
        if (isKeyV)
        {
            File::Path from = _copyBuffer;
            File::Path to   = (_focusFolderPath / from.filename());
            File::CopyFileFromTo(from, to);
            RefreshFocusFolderEntries();
            UmFileSystem.RequestPasteFile(to);
        }
    }
    if (ImGui::IsWindowHovered() && false == ImGui::IsAnyItemHovered())
    {
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        {
            ImGui::OpenPopup(POPUP_ID);
        }
        else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            _rename.CancelRename();
        }
    }
    if (ImGui::BeginPopup(POPUP_ID))
    {
        if (ImGui::MenuItem("Open"))
        {
            File::OpenFile(_focusFolderPath);
        }
        if (ImGui::BeginMenu("Create"))
        {
            if (ImGui::MenuItem("Folder"))
            {
                File::Path newFolderPath = _focusFolderPath / "New Folder";
                File::CreateFolderEx(newFolderPath, true);
                SetFocusEntryPath(newFolderPath);
            }
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Paste"))
        {
            File::Path from = _copyBuffer;
            File::Path to   = (_focusFolderPath / from.filename());
            File::CopyFileFromTo(from, to);
            RefreshFocusFolderEntries();
            UmFileSystem.RequestPasteFile(to);
        }
        if (ImGui::MenuItem("Copy Path"))
        {
            File::CopyPathToClipBoard(_focusFolderPath);
        }
        ImGui::EndPopup();
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

void EditorAssetBrowserTool::ShowDeletePopupBox(const File::Path& path)
{
    bool isFocused = (_focusEntryPath == path);

    ImGui::Text(u8"정말 삭제하시겠습니까?"_c_str);

    ImGui::Text(u8"경로: "_c_str);
    ImGui::SameLine();
    ImGui::Text(path.string().c_str());

    ImGui::PushID(this);

    if (ImGui::Button("OK##") || ImGui::IsKeyReleased(ImGuiKey::ImGuiKey_Enter))
    {
        SetFocusFolderPath(path.parent_path());
        ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();

    if (ImGui::Button("NO##") || ImGui::IsKeyReleased(ImGuiKey::ImGuiKey_Escape))
    {
        ImGui::CloseCurrentPopup();
    }

    ImGui::PopID();
}

void EditorAssetBrowserTool::ShowSameFilePopupBox()
{
    ImGui::Text(u8"해당 경로에 중복된 파일 이름이 있습니다."_c_str);

    if (ImGui::Button("OK##") || ImGui::IsKeyReleased(ImGuiKey::ImGuiKey_Enter))
    {
        ImGui::CloseCurrentPopup();
    }
}

void EditorAssetBrowserTool::ShowCopyFilePopupBox() 
{
    int idSeed = 0;
    int flags = ImGuiChildFlags_Border;
    float  height = ImGui::GetTextLineHeightWithSpacing() * _dragDropPaths.size() + 10.0f;
    ImGui::Text("Copy From: ");
    ImGui::BeginChild("##AssetBrowserCopyPopup", ImVec2(500.0f, height), flags);
    for (auto& [check, targetPath] : _dragDropPaths)
    {
        if (true == fs::exists(targetPath))
        {
            bool isDirectory = fs::is_directory(targetPath);
            if (isDirectory)
            {
                check = false;
            }
            std::string targetPathStr = targetPath.generic_string();
            ImGui::PushID(++idSeed);
            if (isDirectory)
            {   // 디렉터리는 허용하지 않음
                ImGui::BeginDisabled();
            }
            ImGui::Checkbox("##CheckCopy", &check);
            if (isDirectory)
            {
                ImGui::EndDisabled();
            }
            ImGui::SameLine();

            ImVec2 childAvail = ImGui::GetContentRegionAvail();
            ImGui::SetNextItemWidth(childAvail.x);
            int flags = ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_AutoSelectAll;
            if (true == check)
            {
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.2f, 0.1f, 1.0f));
            }
            else
            {
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.1f, 0.1f, 1.0f));
            }
            ImGui::InputText("##CopyFilePath", &targetPathStr, flags);
            ImGui::PopStyleColor();
            ImGuiHelper::HoveredToolTip(isDirectory ? (const char*)u8"폴더 복사는 허용하지 않습니다." : targetPathStr);
            ImGui::PopID();
        }
    }
    ImGui::EndChild();
    ImGui::Text("Copy To :");
    ImGui::BeginDisabled();
    std::string curPath = _destPath.string();
    ImGui::InputText("##import path", &curPath, ImGuiInputTextFlags_ReadOnly); // 임포트할 폰트 경로를 표시합니다.
    if (ImGui::BeginItemTooltip())                                          // 호버링 시 툴팁으로 경로를 표시합니다.
    {
        ImGui::Text(curPath.c_str());
        ImGui::EndTooltip();
    }
    ImGui::EndDisabled();
    ImGui::SameLine();
    if (ImGui::Button(EditorIcon::ICON_FOLDER_OPEN)) // 경로 선택 버튼
    {
        File::ShowOpenFolderDialog(NULL, L"경로 선택", _destPath.c_str(), _destPath);
    }
    ImGui::Separator();
    if (ImGui::Button("Copy"))
    {
        for (const auto& [check, targetPath] : _dragDropPaths)
        {
            if (check)
            {
                File::Path from = targetPath;
                File::Path to   = _destPath / from.filename();
                to   = File::GenerateUniquePath(to); // 중복된 파일 이름이 있을 경우, 고유한 이름으로 변경
                from = from.generic_wstring();
                to   = to.generic_wstring();
                File::CopyFileFromTo(from, to);
            }
        }
        ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel"))
    {
        ImGui::CloseCurrentPopup();
    }
}

void EditorAssetBrowserTool::SetFocusFromUndoPath()
{
    //if (false == _directoryUndoStack.empty())
    //{
    //    const File::Path& undoPath    = _directoryUndoStack.back();
    //    auto              undoContext = UmFileSystem.GetContext<File::FolderContext>(undoPath);
    //    if (false == undoContext.expired())
    //    {
    //        if (false == _currFocusFolderContext.expired())
    //        {
    //            const File::Path& path = _currFocusFolderContext.lock()->GetPath();
    //            _directoryRedoStack.push_back(path);
    //        }
    //        _nextFocusFolderContext = undoContext;
    //    }
    //    _directoryUndoStack.pop_back();
    //}
}

void EditorAssetBrowserTool::SetFocusFromRedoPath()
{
    //if (false == _directoryRedoStack.empty())
    //{
    //    const File::Path& curPath     = _directoryRedoStack.back();
    //    auto              redoContext = UmFileSystem.GetContext<File::FolderContext>(curPath);
    //    if (false == redoContext.expired())
    //    {
    //        if (false == _currFocusFolderContext.expired())
    //        {
    //            const File::Path& path = _currFocusFolderContext.lock()->GetPath();
    //            _directoryUndoStack.push_back(path);
    //        }
    //        _nextFocusFolderContext = redoContext;
    //    }
    //    _directoryRedoStack.pop_back();
    //}
}

void EditorAssetBrowserTool::ProcessDropFile(const HDROP hDrop) 
{
    if (_staticInstance)
    {
        // 드롭된 파일의 개수
        UINT fileCount = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
        _dragDropPaths.clear();
        for (UINT i = 0; i < fileCount; ++i)
        {
            // 각 파일의 절대경로를 얻음
            wchar_t targetPath[MAX_PATH];
            DragQueryFile(hDrop, i, targetPath, MAX_PATH);
            _dragDropPaths.push_back({true, targetPath});
        }
        if (Global::editorModule)
        {
            _destPath = GetCurrentFocusFolderPath();
            Global::editorModule->OpenPopupBox("CopyFile", [this]() { ShowCopyFilePopupBox(); });
        }
    }
    DragFinish(hDrop);
}

bool EditorAssetBrowserTool::WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (_staticInstance)
    {
        bool isProjectLoaded = UmFileSystem.IsLoadedProject();
        if (true == isProjectLoaded)
        {
            switch (msg)
            {
                case WM_DROPFILES: 
                {
                    HDROP hDrop = (HDROP)wParam;
                    if (true == _staticInstance->IsFocusFrame())
                    {
                        const File::Path& curPath = _staticInstance->GetCurrentFocusFolderPath();
                        if (true == fs::exists(curPath))
                        {
                            _staticInstance->ProcessDropFile(hDrop);
                            return true;
                        }
                    }
                    break;
                }
            }
        }
    }
    return false;
}

EditorAssetBrowserTool::AssetData* EditorAssetBrowserTool::GetAssetData(const File::Path& path)
{
    auto itr = _focusFolderAssetDataMap.find(path);
    if (itr != _focusFolderAssetDataMap.end())
    {
        return &itr->second; // 해당 경로의 AssetData 반환
    }
    return nullptr;
}

void EditorAssetBrowserTool::RefreshState() 
{
}

void EditorAssetBrowserTool::RefreshFocusFolderEntries()
{
    if (fs::exists(_focusFolderPath) && fs::is_directory(_focusFolderPath))
    {
        // 매번 Vector는 초기화하고 다시 생성
        _focusFolderAssetDataList.clear();
        // 파일이 없으면 제거, (벡터는 매번 초기화하므로 상관X)
        for (auto it = _focusFolderAssetDataMap.begin(); it != _focusFolderAssetDataMap.end();)
        {
            const auto& [path, data] = *it;
            if (fs::exists(path) == false)
            {
                it = _focusFolderAssetDataMap.erase(it);
            }
            else
            {
                ++it; // 파일이 있으면 다음으로 이동
            }
        }

        // 폴더 내의 모든 파일을 읽어옴
        for (const auto& entry : fs::directory_iterator(_focusFolderPath))
        {
            std::filesystem::path extension   = entry.path().extension();
            bool                  isMetaFile  = extension == File::META_EXTENSION;
            bool                  canShowMeta = _flags[FLAG_SHOW_META];
            if (true == isMetaFile && false == canShowMeta)
            {
                continue; // 메타 파일은 제외
            }

            // 이미 있는 데이터면 데이터만 갱신, 없으면 데이터를 추가하고 갱신
            auto itr = _focusFolderAssetDataMap.find(entry.path());
            if (itr == _focusFolderAssetDataMap.end())
            {
                _focusFolderAssetDataMap[entry.path()] = AssetData(entry);
            }

            AssetData& assetData = _focusFolderAssetDataMap[entry.path()];
            assetData.Refesh(entry);

            // 뷰 이름 설정
            ImVec2 iconWidgetSize = ICON_WIDGET_SIZE * _zoomScale;
            float  maxTextWidth   = iconWidgetSize.x - ImGui::CalcTextSize("...").x - 2.0f;
            float  fullWidth      = ImGui::CalcTextSize(assetData.ViewName.c_str()).x;
            // 너무 길면 ... 처리
            if (fullWidth > maxTextWidth)
            {
                while (!assetData.ViewName.empty() && ImGui::CalcTextSize((assetData.ViewName + "...").c_str()).x > maxTextWidth)
                {
                    assetData.ViewName.pop_back();
                }
                assetData.ViewName += "...";
            }
            _focusFolderAssetDataList.push_back(&assetData);
        }
        std::sort(_focusFolderAssetDataList.begin(), _focusFolderAssetDataList.end(), Compare(ReflectFields->SortFlags));
        _updateTime = 0.0f;
    }
}

void EditorAssetBrowserTool::SetFocusFolderPath(const File::Path& path) 
{
    File::Path focusPath = path.generic_string();
    if (fs::exists(focusPath) && fs::is_directory(focusPath))
    {
        if (_focusFolderPath != focusPath)
        {
            _focusFolderPath = focusPath;
            RefreshFocusFolderEntries();
            _rename.CancelRename();
            _directoryRedoStack.clear();
            _directoryUndoStack.push_back(focusPath);
            if (_directoryUndoStack.size() > _maxUndoStack)
            {
                _directoryUndoStack.pop_front();
            }
        }
    }
    else
    {
        _directoryUndoStack.clear();
        _directoryRedoStack.clear();
    }
}

void EditorAssetBrowserTool::SetFocusEntryPath(const File::Path& path) 
{
    File::Path focusPath = path.generic_string();
    if (fs::exists(path) && _focusEntryPath != focusPath)
    {
        File::Path parent = focusPath.parent_path();
        SetFocusFolderPath(parent.generic_string());
        _focusEntryPath = focusPath;
        _rename.CancelRename();
    }
}

void EditorAssetObject::OnInspectorEnter() 
{
}

void EditorAssetObject::OnInspectorStay()
{
    bool isDebug   = Global::editorModule->IsDebugMode();
    bool isExpired = _focusedInspector.expired();

    if (false == isExpired)
    {
        auto  spContext = _focusedInspector.lock();
        if (true == isDebug)
        {
            auto& metaData = spContext->GetMeta();
            ImGui::Text("Path: %s", spContext->GetPath().string().c_str());
            ImGui::Text("Guid: %s", metaData.GetGuid().string().c_str());
            ImGui::Separator();
        }
        auto& path = spContext->GetPath();
        UmFileSystem.RequestInspectFile(path);
    }
}

void EditorAssetObject::OnInspectorExit() 
{
}

void EditorAssetObject::SetContext(std::weak_ptr<File::Context> context)
{
    _selectedAsset = context;

    if (true == EditorInspectorTool::SetFocusObject(_this))
    {
        _focusedInspector = context;
    }
}

bool EditorAssetBrowserTool::Compare::operator()(const AssetData* a, const AssetData* b) const
{
    if (a->Order != b->Order)
    {   // Order가 다르면 Order 오름차순으로 정렬
        return a->Order < b->Order; 
    }
    if (flags & FLAGS_SORT_BY_TYPE)
    {
        return CompareByType(a, b); // 폴더 우선
    }
    if (flags & FLAGS_SORT_BY_NAME)
    {
        return CompareByName(a, b); // 이름순
    }
    if (flags & FLAGS_SORT_BY_DATE)
    {
        return CompareByDate(a, b); // 날짜순
    }
    return false;
}

bool EditorAssetBrowserTool::Compare::CompareByType(const AssetData* a, const AssetData* b) const
{
    bool aIsDir = a->IsDirectory;
    bool bIsDir = b->IsDirectory;
    return aIsDir > bIsDir;
}

bool EditorAssetBrowserTool::Compare::CompareByName(const AssetData* a, const AssetData* b) const
{
    std::string aName = a->FileName;
    std::string bName = b->FileName;
    return aName < bName;
}

bool EditorAssetBrowserTool::Compare::CompareByDate(const AssetData* a, const AssetData* b) const
{
    auto timeA = a->LastWriteTime;
    auto timeB = b->LastWriteTime;
    return timeA > timeB; // 최신순
}

EditorAssetBrowserTool::AssetData::AssetData(FileEntry entry) 
    : Entry(entry)
{
    IsDirectory = fs::is_directory(entry);
    FileName    = entry.path().filename().string();
    File::Path extension = entry.path().extension();

    static std::unordered_set<File::Path> imageFormat = {".jpg", ".png", ".dds", ".jpeg", ".bmp"};
    static std::unordered_set<File::Path> modelFormat = {".fbx", ".FBX",".obj"};

    if (IsDirectory)
    {
        PreviewIconTexture = UmResourceManager->LoadResource<Texture>(L"../GameEngine/Icon/Editor/Icon_Folder.png");
    }
    else if (imageFormat.find(extension) != imageFormat.end())
    {
        PreviewIconTexture = UmResourceManager->LoadResource<Texture>(entry.path());
    }
    else if (modelFormat.find(extension) != modelFormat.end())
    {
        PreviewIconTexture = UmResourceManager->LoadResource<Texture>(L"../GameEngine/Icon/Editor/Icon_File_FBX.png");
    }
    else
    {
        PreviewIconTexture = UmResourceManager->LoadResource<Texture>(L"../GameEngine/Icon/Editor/Icon_File_Default.png");
    }
}

void EditorAssetBrowserTool::AssetData::Refesh(FileEntry entry) 
{
    Entry         = entry;
    LastWriteTime = File::GetFileLastWriteTime(entry);
    ViewName      = FileName;
}

void EditorAssetBrowserTool::RenameController::StartRename(const File::Path& destFilePath)
{
    IsRenaming = true;
    DestPath     = destFilePath;
    DestName     = DestPath.filename().string();
    RenameBuffer = DestName;
}

void EditorAssetBrowserTool::RenameController::CancelRename() 
{
    IsRenaming   = false;
    DestName     = "";
    RenameBuffer = "";
}

const File::Path& EditorAssetBrowserTool::RenameController::ExecuteRename(const File::Path& targetFolder)
{
    Return = "";
    if (IsRenaming)
    {
        Return = targetFolder / RenameBuffer;
        if (false == Return.has_extension())
        {
            Return.replace_extension(DestPath.extension());
        }
        if (!fs::exists(Return))
        {
            fs::rename(DestPath, Return);
        }
        CancelRename();
    }
    return Return;
}
