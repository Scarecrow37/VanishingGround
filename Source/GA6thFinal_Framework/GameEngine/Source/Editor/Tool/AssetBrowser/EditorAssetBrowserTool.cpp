#include "pch.h"

REFLECT_FUNCTION(EditorAssetBrowserTool)

namespace fs = std::filesystem;
using namespace u8_literals;


EditorAssetBrowserTool::EditorAssetBrowserTool()
{
    SetLabel("AssetBrowser");
    SetDockLayout(ImGuiDir_Down);

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
    _inspectorDrawer = std::make_unique<InspectorDrawer>();
    UmFileSystem.RegisterFileEventSubscriber(this);
}

void EditorAssetBrowserTool::OnPreFrameBegin()
{
}

void EditorAssetBrowserTool::OnPostFrameBegin() 
{
    ImVec2 contentMin = ImGui::GetWindowContentRegionMin(); // 보통 (0, 0)
    ImVec2 contentMax = ImGui::GetWindowContentRegionMax(); // 보통 (width, height)
    ImVec2 pos        = ImGui::GetWindowPos();
}

void EditorAssetBrowserTool::OnFrameRender()
{
    if (false == fs::exists(_focusFolderPath))
    {
        File::Path parentPath = _focusFolderPath.parent_path();
        if (fs::exists(parentPath) && fs::is_directory(parentPath))
        {
            SetFocusFolderPath(parentPath, false);
        }
        else
        {
            ResetState();
        }
    }
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
    if (fs::exists(_focusFolderPath) && fs::is_directory(_focusFolderPath))
    {
        ProcessInput();
    }
}

void EditorAssetBrowserTool::OnFrameFocusExit() 
{
}

void EditorAssetBrowserTool::OnPostRequestedLoad() 
{
    // 파일 시스템이 로드된 후에 호출되는 메서드
    // 여기서 초기 폴더 경로를 설정하거나 필요한 초기화 작업을 수행할 수 있습니다.
    ResetState();
}

void EditorAssetBrowserTool::ShowUpperFrame()
{
    auto&   style       = ImGui::GetStyle();
    float   fontSize    = ImGui::GetFontSize();
    int     childFlags  = ImGuiChildFlags_Border;
    int     windowFlags = ImGuiWindowFlags_NoScrollbar;
    float   height      = ImGui::GetTextLineHeightWithSpacing();
    ImVec2  spacing     = style.ItemSpacing;
    ImVec2  padding     = style.FramePadding;
    bool    isRootpath  = (_focusFolderPath == UmFileSystem.GetRootPath());
    bool    isExsists   = fs::exists(_focusFolderPath);

    ImGui::BeginChild("UpperFrame", ImVec2(0, fontSize + height), childFlags, windowFlags);
    {
        ImVec2 windowPos    = ImGui::GetWindowPos();
        ImVec2 windowSize   = ImGui::GetWindowSize();
        ImVec2 availSpace   = ImGui::GetContentRegionAvail();
        ImVec2 cursorpos    = ImGui::GetCursorPos();

        bool canUndo = (false == _directoryUndoStack.empty());
        bool canRedo = (false == _directoryRedoStack.empty());

        // 뒤로 가기, 앞으로 가기
        ImGuiHelper::StyleBuilder buttonStyle;
        buttonStyle.PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        buttonStyle.PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
        if (false == canUndo || false == isExsists) ImGui::BeginDisabled();
        if (ImGui::Button(EditorIcon::ICON_CIRCLE_ARROW_LEFT))
        {
            UndoPath();
        }
        if (false == canUndo || false == isExsists) ImGui::EndDisabled();
        ImGuiHelper::HoveredToolTip(u8"뒤로 가기"_c_str, ImGuiHoveredFlags_DelayNormal);
        ImGui::SameLine();
        if (false == canRedo || false == isExsists) ImGui::BeginDisabled();
        if (ImGui::Button(EditorIcon::ICON_CIRCLE_ARROW_RIGHT))
        {
            RedoPath();
        }
        if (false == canRedo || false == isExsists) ImGui::EndDisabled();
        ImGuiHelper::HoveredToolTip(u8"앞으로 가기"_c_str, ImGuiHoveredFlags_DelayNormal);
        ImGui::SameLine(0.0f, 10.0f);

        if (isRootpath || false == isExsists) ImGui::BeginDisabled();
        if (ImGui::Button(EditorIcon::ICON_CIRCLE_ARROW_UP))
        {
            _rename.CancelRename();
            File::Path parentPath = _focusFolderPath.parent_path();
            SetFocusFolderPath(parentPath);
        }
        if (isRootpath || false == isExsists) ImGui::EndDisabled();
        ImGuiHelper::HoveredToolTip(u8"상위 폴더"_c_str, ImGuiHoveredFlags_DelayNormal);
        ImGui::SameLine(0.0f, 10.0f);

        if (false == isExsists) ImGui::BeginDisabled();
        if (ImGui::Button(EditorIcon::ICON_CIRCLE))
        {
            _needRefresh = true;
        }
        if (false == isExsists) ImGui::EndDisabled();
        ImGuiHelper::HoveredToolTip(u8"새로고침"_c_str, ImGuiHoveredFlags_DelayNormal);
        ImGui::SetCursorPos(cursorpos + ImVec2(ReflectFields->ColumWidth, 0.0f));

        // 폴더 경로 표시
        if (false == _focusFolderPath.empty())
        {
            const File::Path& currPath  = _focusFolderPath;
            const File::Path& rootPath  = UmFileSystem.GetRootPath();
            File::Path relativePath     = fs::relative(currPath, rootPath);

            relativePath = relativePath == L"." ? 
                rootPath.filename() :
                rootPath.filename() / relativePath;
            
            File::Path node = rootPath.parent_path();
            for (auto itr = relativePath.begin(); itr != relativePath.end();)
            {
                const File::Path& segment = *itr;
                if (segment.empty())
                    break;
                if (segment == L".")
                    continue;

                node /= segment; // 누적 경로에 현재 세그먼트 추가

                // 세그먼트 버튼
                std::string label = segment.string();
                if (ImGui::Button(label.c_str()))
                {
                    SetFocusFolderPath(node); // 해당 폴더로 포커스 이동
                }

                // 마지막 세그먼트가 아니면 구분자 찍기
                ++itr;
                if (itr != relativePath.end())
                {
                    ImGui::SameLine();
                    ImGui::TextUnformatted(">");
                    ImGui::SameLine();
                }
            }
        }
        buttonStyle.PopStyle();
    }
    ImGui::EndChild();
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
        ReflectFields->ColumWidth = ImGui::GetIO().MousePos.x - rect.Min.x - (padding * 1.5f);
    }
    ReflectFields->ColumWidth = ImClamp(ReflectFields->ColumWidth, 200.0f, columWidth - 200.0f);
}

void EditorAssetBrowserTool::ShowFolderHierarchy()
{
    _folderCount = 0;
    const File::Path& root = UmFileSystem.GetRootPath();
    ImVec4 col       = ImColor(70, 70, 70, 255);
    ImVec4 colHover  = ImColor(90, 90, 90, 255);
    ImVec4 colActive = ImColor(50, 50, 50, 255);
    ImGuiHelper::StyleBuilder collapsingStyle;
    collapsingStyle.PushStyleColor(ImGuiCol_Header, col);
    collapsingStyle.PushStyleColor(ImGuiCol_HeaderHovered, colHover);
    collapsingStyle.PushStyleColor(ImGuiCol_HeaderActive, colActive);
    std::string folderLabel = std::format("{} {}", EditorIcon::ICON_FOLDER_OPEN, "Project Folders");
    if (ImGui::CollapsingHeader(folderLabel.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
    {
        collapsingStyle.PopStyle();
        ShowFolderHierarchyTree(root);
    }
    ImGui::Separator();
    // 즐겨찾기 폴더
    collapsingStyle.PushStyleColor(ImGuiCol_Header, col);
    collapsingStyle.PushStyleColor(ImGuiCol_HeaderHovered, colHover);
    collapsingStyle.PushStyleColor(ImGuiCol_HeaderActive, colActive);
    std::string favoriteLabel = std::format("{} {}", EditorIcon::ICON_STAR, "Favorite Folders");
    if (ImGui::CollapsingHeader(favoriteLabel.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
    {
        collapsingStyle.PopStyle();
        for (const auto& folder : ReflectFields->FavoriteFolders)
        {
            const File::Path& root         = UmFileSystem.GetRootPath();
            File::Path        favoritePath = root / folder;
            if (fs::exists(favoritePath) && fs::is_directory(favoritePath))
            {
                ImGui::PushID(&folder);
                ShowFolderHierarchyTree(favoritePath);
                ImGui::PopID();
            }
            else
            {
                _delayEvent.push_back([this, favoritePath]() {
                    RemoveFavoriteFolder(favoritePath); 
                    });
            }
        }
    }
    collapsingStyle.PopStyle();
}

#define NODE_SPACING "                                                                ##"
void EditorAssetBrowserTool::ShowFolderHierarchyTree(const File::Path& directory)
{
    if (fs::exists(directory) && fs::is_directory(directory))
    {
        ++_folderCount;
        std::string filePath    = directory.string();
        std::string fileName    = directory.filename().string();
        bool isFocusedFolder    = _focusFolderPath == directory;
        float cursorX           = ImGui::GetCursorPosX();

        // ==== Tree 출력 ====
        ImGui::PushID(_folderCount);
        int treeFlags           = ImGuiTreeNodeFlags_OpenOnArrow;
        treeFlags               |= isFocusedFolder ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None;
        bool isTreeOpen         = ImGui::TreeNodeEx(NODE_SPACING, treeFlags);
        bool isHovered          = ImGui::IsItemHovered();
        bool isLeftClicked      = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
        bool isRightClicked     = ImGui::IsMouseClicked(ImGuiMouseButton_Right);
        bool isDoubleClicked    = ImGui::IsMouseDoubleClicked(0);
        ImGuiHelper::HoveredToolTip(filePath.c_str(), ImGuiHoveredFlags_DelayNormal);

        if (isHovered)
        {
            if (isDoubleClicked)
            {
                SetFocusFolderPath(directory);
            }
            if (isRightClicked)
            {
                ImGui::OpenPopup("##folder_context_popup");
            }
        }
        if (ImGui::BeginPopup("##folder_context_popup"))
        {
            if (IsFavoriteFolder(directory))
            {
                if (ImGui::MenuItem("Remove Favorite"))
                {
                    _delayEvent.emplace_back([this, directory]() {
                        RemoveFavoriteFolder(directory);
                        });
                }
            }
            else
            {
                if (ImGui::MenuItem("Add Favorite"))
                {
                    _delayEvent.emplace_back([this, directory]() {
                        AddFavoriteFolder(directory); 
                        });
                }
            }
            ImGui::EndPopup();
        }

        ImGui::SameLine();
        // ==== Text출력 ====
        const char* icon  = isTreeOpen ? EditorIcon::ICON_FOLDER_OPEN : EditorIcon::ICON_FOLDER;
        std::string label = std::format("{} {}", icon, fileName);
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
        DragDropTransform::Data data;
        if (ImGuiHelper::DragDrop::RecieveFrameDragDropEvent(DragDropTransform::KEY, &data))
        {
            File::Path relativePath = std::filesystem::relative(_focusFolderPath, UmFileSystem.GetAssetPath());
            UmGameObjectFactory.WriteGameObjectFile(data.pTransform, relativePath.string());
            RefreshFocusFolderEntries();
        }
        ShowSearchBar();
        BeginFolderEntryFrame();
        UpdateFolderEntriesInput();
        int showType = ReflectFields->ShowType;
        int pushStyleVar = 0;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(15.0f, 4.0f)); ++pushStyleVar;// 아이콘 간격 조정
        // 엔트리 목록
        for (auto& asset : _focusFolderAssetDataList)
        {
            if (_search.PassFilter(asset->FileName.c_str()))
            {
                switch (showType)
                {
                case EditorAssetBrowserTool::SHOW_TYPE_LIST:
                    ShowFolderEntryToList(*asset);
                    break;
                case EditorAssetBrowserTool::SHOW_TYPE_ICON:
                    ShowFolderEntryToIcon(*asset);
                    break;
                case EditorAssetBrowserTool::SHOW_TYPE_EDIT:
                    ShowFolderEntryToEdit(*asset);
                    break;
                default:
                    break;
                }
            }
        }
        ImGui::PopStyleVar(pushStyleVar);
        ImGui::EndChild();
    }
}

void EditorAssetBrowserTool::ShowSearchBar()
{
    float  roundFactor = 3.0f;
    auto&  style       = ImGui::GetStyle();
    float  fontSize    = ImGui::GetFontSize();
    float  frameHeight = ImGui::GetFrameHeight();
    float  height      = ImGui::GetTextLineHeightWithSpacing();
    ImVec2 spacing     = style.ItemSpacing;
    ImVec2 padding     = style.FramePadding;

    ImGuiHelper::StyleBuilder styleBuilder;
    styleBuilder.PushStyleVar(ImGuiStyleVar_FrameRounding, roundFactor);
    ImGui::BeginChild("SearchBar", ImVec2(0, fontSize + height), ImGuiChildFlags_Border, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImVec2 windowPos  = ImGui::GetWindowPos();
    ImVec2 windowSize = ImGui::GetWindowSize();
    ImVec2 availSpace = ImGui::GetContentRegionAvail();
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    ImVec2 inputSize = ImVec2(availSpace.x * 0.5f + 50.0f, ImGui::GetFrameHeight()); // 높이는 자동 조절됨

    // ---------------------------
    // 버튼 크기 계산
    // ---------------------------
    const char* iconX = EditorIcon::ICON_CIRCLE_X;
    ImVec2 textSize   = ImGui::CalcTextSize(iconX);
    ImVec2 buttonSize = textSize + padding * 2;

    // ---------------------------
    // 사각형 Draw
    // ---------------------------
    ImVec2 rectSize  = ImVec2(inputSize.x + buttonSize.x, inputSize.y);
    ImU32  rectColor = ImGui::GetColorU32(ImGuiCol_FrameBg);
    ImGuiHelper::DrawFillRect(cursorPos, cursorPos + rectSize, rectColor, roundFactor, ImDrawFlags_RoundCornersAll);

    // ---------------------------
    // 버튼 먼저 그림 (겹치게)
    // ---------------------------
    ImVec2 buttonPos = ImVec2(cursorPos + ImVec2(inputSize.x, 0.0f));
    ImGui::SetCursorScreenPos(buttonPos);
    ImGuiHelper::StyleBuilder buttonStyle;
    buttonStyle.PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));        // 기본 배경
    buttonStyle.PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0)); // 호버 시 배경
    buttonStyle.PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));  // 클릭 시 배경
    if (ImGui::Button(iconX, buttonSize))
    {
        _search.ClearBuffer();
    }
    buttonStyle.PopStyle();

    // ---------------------------
    // InputText는 나중에 그림
    // ---------------------------
    ImGui::SetCursorScreenPos(cursorPos);
    ImGuiHelper::StyleBuilder inputStyle;
    inputStyle.PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0)); // 완전 투명
    ImGui::SetNextItemWidth(inputSize.x);
    if (ImGui::InputTextWithHint("##SearchBar", "Search...", &_search.SearchBuffer, ImGuiInputTextFlags_AutoSelectAll))
    {
        if (File::IsGuid(_search.SearchBuffer))
        {
            const File::Path& path = UmFileSystem.GetPathFromGuid(_search.SearchBuffer);
            if (path != File::NULL_PATH)
            {
                SetFocusEntryPath(path);
            }
            _search.ClearBuffer();
        }
        else
        {
            _search.UpdateBuffer();
        }
    }
    _search.IsSearching = ImGui::IsItemActive();
    inputStyle.PopStyle();
    styleBuilder.PopStyle();

    // ---------------------------
    // Open Option
    // ---------------------------
    ImVec2 optionCursorPos = cursorPos + ImVec2(availSpace.x - buttonSize.x - padding.x * 2.0f, 0.0f);
    ImGui::SetCursorScreenPos(optionCursorPos);
    const char* popupID = "##filter_compare_popup";
    buttonStyle.PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    if (ImGui::Button(EditorIcon::ICON_SUN))
    {
        ImGui::OpenPopup(popupID);
    }
    if (ImGui::BeginPopup(popupID))
    {
        ImGuiHelper::AlignedText(u8"정렬"_c_str, ImGuiHelper::CENTER, 0.8f);
        ImGuiHelper::Separator(3.0f);

        bool compareByType = ReflectFields->SortFlags.first == Compare::FLAGS_SORT_BY_TYPE;
        if (ImGui::MenuItem(u8"유형"_c_str, "", compareByType))
        {
            ReflectFields->SortFlags.first = Compare::FLAGS_SORT_BY_TYPE;
            _needRefresh = true;
        }
        bool compareByName = ReflectFields->SortFlags.first == Compare::FLAGS_SORT_BY_NAME;
        if (ImGui::MenuItem(u8"이름"_c_str, "", compareByName))
        {
            ReflectFields->SortFlags.first = Compare::FLAGS_SORT_BY_NAME;
            _needRefresh = true;
        }
        bool compareByDate = ReflectFields->SortFlags.first == Compare::FLAGS_SORT_BY_DATE;
        if (ImGui::MenuItem(u8"수정된 날짜"_c_str, "", compareByDate))
        {
            ReflectFields->SortFlags.first = Compare::FLAGS_SORT_BY_DATE;
            _needRefresh = true;
        }
        bool compareByAssetID = ReflectFields->SortFlags.first == Compare::FLAGS_SORT_BY_ASSET_ID;
        if (ImGui::MenuItem(u8"에셋 ID"_c_str, "", compareByAssetID))
        {
            ReflectFields->SortFlags.first = Compare::FLAGS_SORT_BY_ASSET_ID;
            _needRefresh                   = true;
        }
        ImGuiHelper::Separator(3.0f);
        bool compareAscending = ReflectFields->SortFlags.second;
        if (ImGui::MenuItem(u8"오름차순"_c_str, "", compareAscending))
        {
            ReflectFields->SortFlags.second = true;
            _needRefresh = true; 
        }
        if (ImGui::MenuItem(u8"내림차순"_c_str, "", !compareAscending))
        {
            ReflectFields->SortFlags.second = false;
            _needRefresh = true;
        }

        ImGui::EndPopup();
    }
    buttonStyle.PopStyle();

    // ---------------------------
    // Change View Type Button
    // ---------------------------
    bool isShowList = (ReflectFields->ShowType == SHOW_TYPE_LIST);
    bool isShowIcon = (ReflectFields->ShowType == SHOW_TYPE_ICON);
    bool isShowEdit = (ReflectFields->ShowType == SHOW_TYPE_EDIT);
    ImVec2 iconCusorPos = optionCursorPos - ImVec2(frameHeight, 0.0f);
    ImVec2 listCusorPos = iconCusorPos - ImVec2(frameHeight, 0.0f);
    ImVec2 editCusorPos = listCusorPos - ImVec2(frameHeight, 0.0f);

    {
        ImVec4 selectedColor = isShowIcon ? ImGui::GetStyleColorVec4(ImGuiCol_Header) : ImVec4(0, 0, 0, 0);
        buttonStyle.PushStyleColor(ImGuiCol_Button, selectedColor);
        buttonStyle.PushStyleColor(ImGuiCol_ButtonHovered, selectedColor * ImVec4(1.2f, 1.2f, 1.2f, 1.0f));
        buttonStyle.PushStyleColor(ImGuiCol_ButtonActive, selectedColor * ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
        ImGui::SetCursorScreenPos(iconCusorPos);
        if (ImGui::Button(EditorIcon::ICON_IMAGES))
        {
            ReflectFields->ShowType = SHOW_TYPE_ICON;
            _needRefresh = true; // 새로고침 필요
        }
        buttonStyle.PopStyle();
    }
    {
        ImVec4 selectedColor = isShowList ? ImGui::GetStyleColorVec4(ImGuiCol_Header) : ImVec4(0, 0, 0, 0);
        buttonStyle.PushStyleColor(ImGuiCol_Button, selectedColor);
        buttonStyle.PushStyleColor(ImGuiCol_ButtonHovered, selectedColor * ImVec4(1.2f, 1.2f, 1.2f, 1.0f));
        buttonStyle.PushStyleColor(ImGuiCol_ButtonActive, selectedColor * ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
        ImGui::SetCursorScreenPos(listCusorPos);
        if (ImGui::Button(EditorIcon::ICON_LIST))
        {
            ReflectFields->ShowType = SHOW_TYPE_LIST;
            _needRefresh            = true; // 새로고침 필요
        }
        buttonStyle.PopStyle();
    }
    {
        ImVec4 selectedColor = isShowEdit ? ImGui::GetStyleColorVec4(ImGuiCol_Header) : ImVec4(0, 0, 0, 0);
        buttonStyle.PushStyleColor(ImGuiCol_Button, selectedColor);
        buttonStyle.PushStyleColor(ImGuiCol_ButtonHovered, selectedColor * ImVec4(1.2f, 1.2f, 1.2f, 1.0f));
        buttonStyle.PushStyleColor(ImGuiCol_ButtonActive, selectedColor * ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
        ImGui::SetCursorScreenPos(editCusorPos);
        if (ImGui::Button(EditorIcon::ICON_EDIT))
        {
            ReflectFields->ShowType = SHOW_TYPE_EDIT;
            _needRefresh            = true; // 새로고침 필요
        }
        buttonStyle.PopStyle();
    }

    ImGui::EndChild();
}


void EditorAssetBrowserTool::ShowFolderEntryToList(AssetData& asset)
{
    auto& io = ImGui::GetIO();
    auto& style = ImGui::GetStyle();

    auto  window = ImGui::GetCurrentWindow();
    float oldFontScale = window->FontWindowScale;
    ImGui::SetWindowFontScale(_zoomScale);

    bool isItemReleasedLeft     = false;
    bool isItemClickedLeft      = false;
    bool isItemClickedRight     = false;
    bool isItemFocusedNav       = false;
    bool isItemDoubleClicked    = false;

    bool   isFocused  = (_focusEntryPath == asset.Entry.path());
    bool   isRenaming = _rename.IsActive() && isFocused;
    float  fontSize   = ImGui::GetFontSize();
    float  textHeight = ImGui::GetTextLineHeightWithSpacing();
    ImVec2 availSpace = ImGui::GetContentRegionAvail();
    ImVec2 widgetSize = ImVec2(availSpace.x, textHeight);
    ImVec2 cursorPos  = ImGui::GetCursorPos();
    ImVec4 bgCol      = isFocused ? ImGui::GetStyleColorVec4(ImGuiCol_Header) : ImVec4(0, 0, 0, 0);

    ImGui::PushID(&asset);
    ImGuiHelper::StyleBuilder buttonStyle;
    int buttonFlags = ImGuiButtonFlags_None;
    buttonStyle.PushStyleColor(ImGuiCol_Button, bgCol);
    isItemClickedLeft |= ImGui::ButtonEx(" ", widgetSize, buttonFlags);
    

    bool isHovered      = ImGui::IsItemHovered();
    isItemFocusedNav    |= ImGui::IsItemFocused() && io.NavActive;
    isItemClickedRight  |= isHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right) && !isRenaming;
    isItemReleasedLeft  |= isHovered && ImGui::IsMouseReleased(ImGuiMouseButton_Left) && !isRenaming;
    isItemDoubleClicked |= isHovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && !isRenaming;
    buttonStyle.PopStyle();

    ProcessFolderEntryDragDrop(asset);

    bool isNavDirty = isItemFocusedNav 
                        && (ImGui::IsKeyDown(ImGuiKey_DownArrow) 
                        || ImGui::IsKeyDown(ImGuiKey_UpArrow) 
                        || ImGui::IsKeyDown(ImGuiKey_Tab));

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
        {   // 파일 열기
            UmFileSystem.RequestOpenFile(asset.Entry.path());
        }
    }
    
    const char* icon = asset.IsDirectory ? EditorIcon::ICON_FOLDER : EditorIcon::ICON_FILE;
    ImGui::SetCursorPos(cursorPos);
    ImGui::TextUnformatted(icon);
    ImGui::SameLine(cursorPos.x + fontSize + style.FramePadding.x);
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
        ImGui::TextUnformatted(asset.FileName.c_str());
        ImGui::SameLine();

        std::ostringstream oss;
        std::tm            tm;
        localtime_s(&tm, &asset.LastWriteTime);
        oss << std::put_time(&tm, "%F %T"); // "YYYY-MM-DD HH:MM:SS" 형식
        ImGui::SetCursorPosX(availSpace.x * ReflectFields->ListColumnWidth[0] + style.FramePadding.x);
        ImGui::TextUnformatted(oss.str().c_str());
        ImGui::SameLine();

        ImGui::SetCursorPosX(availSpace.x * ReflectFields->ListColumnWidth[1] + style.FramePadding.x);
        if (asset.IsDirectory)
        {
            ImGui::TextUnformatted("-");
        }
        else
        {
            int         assetID = UmFileSystem.GetAssetIDFromPath(asset.Entry.path());
            std::string idStr   = std::to_string(assetID);
            ImGui::TextUnformatted(idStr.c_str());
        }
        ImGui::SameLine();

        ImGui::SetCursorPosX(availSpace.x * ReflectFields->ListColumnWidth[2] + style.FramePadding.x);
        if (asset.IsDirectory)
        {
            ImGui::TextUnformatted(u8"폴더"_c_str);
        }
        else
        {
            std::string formatLabel = std::format("{} {}", asset.Extension.c_str(), u8"파일"_c_str);
            ImGui::TextUnformatted(formatLabel.c_str());
        }
    }
    if (isItemReleasedLeft || isNavDirty)
    {
        SetFocusEntryPath(asset.Entry.path());
        _inspectorDrawer->SetAsset(&asset);
        EditorInspectorTool::SetFocusObject(_inspectorDrawer);
    }
    if (isItemClickedRight)
    {
        ImGui::OpenPopup(POPUP_ID);
    }
    ShowFolderEntryPopup(asset);

    ImGui::PopID();

    ImGui::SetWindowFontScale(oldFontScale);
    ImGui::Separator();
}

void EditorAssetBrowserTool::ShowFolderEntryToIcon(AssetData& asset)
{
    auto& io = ImGui::GetIO();
    
    bool isNavDirty         = false;
    bool isItemReleasedLeft = false;
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
        int buttonFlags = ImGuiButtonFlags_None;
        ImVec4 iconColor = _copyBuffer.first == 1 && asset.IsSamePath(_copyBuffer.second) ? ImVec4(1.0f, 1.0f, 1.0f, 0.5f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        D3D12_GPU_DESCRIPTOR_HANDLE icon = asset.PreviewIconTexture->GetGPUHandle();
        ImGuiHelper::StyleBuilder iconStyle;
        iconStyle.PushStyleColor(ImGuiCol_Button, bgCol);
        ImGuiID id = ImGui::GetID("##icon");
        isItemClickedLeft |=
            ImGui::ImageButtonEx(id, (ImTextureID)icon.ptr, iconWidgetSize, ImVec2(0, 0),
            ImVec2(1, 1), ImVec4(0,0,0,0), iconColor, buttonFlags);
        
        bool isHovered      = ImGui::IsItemHovered();
        isItemFocusedNav |= ImGui::IsItemFocused() && io.NavActive;
        isItemClickedRight |= isHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right) && !isRenaming;
        isItemReleasedLeft |= isHovered && ImGui::IsMouseReleased(ImGuiMouseButton_Left) && !isRenaming;
        iconStyle.PopStyle();

        ProcessFolderEntryDragDrop(asset);

        isNavDirty = isItemFocusedNav
                        && (ImGui::IsKeyDown(ImGuiKey_LeftArrow)
                        || ImGui::IsKeyDown(ImGuiKey_RightArrow)
                        || ImGui::IsKeyDown(ImGuiKey_Tab));

        if (isHovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
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
        //isItemFocusedNav    |= ImGui::IsItemFocused() && io.NavActive;

        ImGuiHelper::HoveredToolTip(asset.FileName.c_str());
        textStyle.PopStyle();
        widgetStyle.PopStyle();
        
    }
    ImGui::EndGroup();

    if (isItemReleasedLeft || isNavDirty)
    {
        SetFocusEntryPath(asset.Entry.path());
        _inspectorDrawer->SetAsset(&asset);
        EditorInspectorTool::SetFocusObject(_inspectorDrawer);
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

void EditorAssetBrowserTool::ShowFolderEntryToEdit(AssetData& asset) 
{
    auto& io    = ImGui::GetIO();
    auto& style = ImGui::GetStyle();

    auto  window       = ImGui::GetCurrentWindow();
    float oldFontScale = window->FontWindowScale;
    ImGui::SetWindowFontScale(_zoomScale);

    bool isItemReleasedLeft  = false;
    bool isItemClickedLeft   = false;
    bool isItemClickedRight  = false;
    bool isItemFocusedNav    = false;
    bool isItemDoubleClicked = false;

    bool   isFocused  = (_focusEntryPath == asset.Entry.path());
    float  fontSize   = ImGui::GetFontSize();
    float  textHeight = ImGui::GetTextLineHeightWithSpacing();
    ImVec2 availSpace = ImGui::GetContentRegionAvail();
    ImVec2 widgetSize = ImVec2(availSpace.x, textHeight);
    ImVec2 cursorPos  = ImGui::GetCursorPos();
    ImVec4 bgCol      = isFocused ? ImGui::GetStyleColorVec4(ImGuiCol_Header) : ImVec4(0, 0, 0, 0);

    ImGui::PushID(&asset);

    const char* icon = asset.IsDirectory ? EditorIcon::ICON_FOLDER : EditorIcon::ICON_FILE;
    ImGui::SetCursorPos(cursorPos);
    ImGui::TextUnformatted(icon);
    ImGui::SameLine(cursorPos.x + fontSize + style.FramePadding.x);
    
    ImGui::TextUnformatted(asset.FileName.c_str());
    ImGui::SameLine();

    std::ostringstream oss;
    std::tm            tm;
    localtime_s(&tm, &asset.LastWriteTime);
    oss << std::put_time(&tm, "%F %T"); // "YYYY-MM-DD HH:MM:SS" 형식
    ImGui::SetCursorPosX(availSpace.x * ReflectFields->ListColumnWidth[0] + style.FramePadding.x);
    ImGui::TextUnformatted(oss.str().c_str());
    ImGui::SameLine();

    ImGui::SetCursorPosX(availSpace.x * ReflectFields->ListColumnWidth[1] + style.FramePadding.x);
    if (asset.IsDirectory)
    {
        ImGui::TextUnformatted("-");
    }
    else
    {
        int assetID = UmFileSystem.GetAssetIDFromPath(asset.Entry.path());
        ImGui::SetNextItemWidth(availSpace.x * (ReflectFields->ListColumnWidth[2] - ReflectFields->ListColumnWidth[1]) - (style.FramePadding.x * 2.0f));
        if (ImGui::InputInt("##asset_id", &assetID, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
        {
            EditorAssetBrowserTool::ChangeAssetID(asset.Entry.path(), assetID);
        }
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Give Empty Asset ID"))
            {
                EditorAssetBrowserTool::ChangeAssetID(asset.Entry.path(), 0);
            }
            if (ImGui::MenuItem("Give Automatic Asset ID"))
            {
                EditorAssetBrowserTool::ChangeAutomaticAssetID(asset);
            }
            ImGui::EndPopup();
        }
    }
    ImGui::SameLine();

    ImGui::SetCursorPosX(availSpace.x * ReflectFields->ListColumnWidth[2] + style.FramePadding.x);
    if (asset.IsDirectory)
    {
        ImGui::TextUnformatted(u8"폴더"_c_str);
    }
    else
    {
        std::string formatLabel = std::format("{} {}", asset.Extension.c_str(), u8"파일"_c_str);
        ImGui::TextUnformatted(formatLabel.c_str());
    }

    ImGui::PopID();

    ImGui::SetWindowFontScale(oldFontScale);
    ImGui::Separator();
}

void EditorAssetBrowserTool::ShowFolderEntryPopup(AssetData& asset) 
{
    if (ImGui::BeginPopup(POPUP_ID))
    {
        if (ImGui::MenuItem("Open"))
        {
            File::OpenFile(asset.Entry.path());
        }
        if (asset.IsDirectory) ImGui::BeginDisabled();
        if (ImGui::MenuItem("Copy"))
        {
            SetCopyFileFromPath(asset.Entry.path());
        }
        if (ImGui::MenuItem("Cut"))
        {
            SetCutFileFromPath(asset.Entry.path());
        }
        if (asset.IsDirectory) ImGui::EndDisabled();
        if (ImGui::MenuItem("Rename"))
        {
            _rename.StartRename(asset.Entry.path());
        }
        if (ImGui::MenuItem("Delete"))
        {
            Global::editorModule->OpenPopupBox("Delete File", [this, asset]()
                {
                    _rename.CancelRename();
                    if (ShowDeletePopupBox(asset.Entry.path()))
                    {
                        DeleteFileFromPath(asset.Entry.path());
                    }
                });
        }
        ImGuiHelper::Separator(3.0f);
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
        if (asset.AbleToDragTarget)
        {
            const char* eventID = DragDropAsset::KEY;
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(eventID))
                {
                    DragDropAsset::Data data     = (*(DragDropAsset::Data*)payload->Data);
                    File::Path          fromPath = data.GetPath();
                    File::Path          toPath   = asset.Entry.path() / fromPath.filename();
                    _delayEvent.emplace_back([this, fromPath, toPath]() {
                        if (fs::exists(fromPath))
                        {
                            fs::rename(fromPath, toPath);
                        }
                    });
                }
                ImGui::EndDragDropTarget();
            }
        }
    }
    if (asset.AbleToDragSource)
    {
        const char* eventID = DragDropAsset::KEY;
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
        {
            DragDropAsset::Data                 data;
            File::Path                          entryPath = asset.Entry.path();
            static std::weak_ptr<File::Context> context;
            static File::Path                   path;
            static File::Guid                   guid;
            context       = UmFileSystem.GetContext<File::Context>(entryPath);
            path          = entryPath;
            guid          = path.ToGuid();
            data.pContext = &context;
            data.pPath    = &path;
            data.pGuid    = &guid;
            ImGui::SetDragDropPayload(eventID, &data, sizeof(DragDropAsset::Data));
            if (asset.PreviewIconTexture)
            {
                ImGui::Image((ImTextureID)asset.PreviewIconTexture->GetGPUHandle().ptr, ICON_WIDGET_SIZE);
            }
            ImGui::Text(asset.FileName.c_str());
            ImGui::EndDragDropSource();
        }
    }
}

void EditorAssetBrowserTool::UpdateFolderEntriesInput()
{
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
        int copyState= _copyBuffer.first;
        if (-1 == copyState) ImGui::BeginDisabled();
        if (ImGui::MenuItem("Paste"))
        {
            PasteFile();
        }
        if (-1 == copyState) ImGui::EndDisabled();

        if (ImGui::MenuItem("Copy Path"))
        {
            File::CopyPathToClipBoard(_focusFolderPath);
        }

        ImGuiHelper::Separator(3.0f);
        if (ImGui::MenuItem("Reset AssetID All File"))
        {
            for (auto& asset : _focusFolderAssetDataList)
            {
                if (asset && false == asset->IsDirectory)
                {
                    EditorAssetBrowserTool::ChangeAssetID(asset->Entry.path(), 0);
                }
            }
        }
        if (ImGui::BeginMenu("Give Automatic Asset ID"))
        {
            if (ImGui::MenuItem("All File"))
            {   
                for (auto& asset : _focusFolderAssetDataList)
                {
                    if (asset && false == asset->IsDirectory)
                    {
                        EditorAssetBrowserTool::ChangeAutomaticAssetID(*asset);
                    }
                }
            }
            if (ImGui::MenuItem("Only '0'"))
            {
                for (auto& asset : _focusFolderAssetDataList)
                {
                    if (asset && false == asset->IsDirectory)
                    {
                        int assetID = UmFileSystem.GetAssetIDFromPath(asset->Entry.path());
                        if (0 == assetID)
                        {
                            EditorAssetBrowserTool::ChangeAutomaticAssetID(*asset);
                        }
                    }
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }
}

void EditorAssetBrowserTool::BeginFolderEntryFrame()
{
    ImGui::BeginChild("ListFrame", ImVec2(0, 0));
    int showType = ReflectFields->ShowType;
    // Column
    ImVec2 availSpace = ImGui::GetContentRegionAvail();
    float  frameHeight = ImGui::GetFrameHeight();
    switch (showType)
    {
        case EditorAssetBrowserTool::SHOW_TYPE_ICON:
        {
            break;
        }
        case EditorAssetBrowserTool::SHOW_TYPE_LIST:
        {
            // SHOW_TYPE_EDIT과 동일
        }
        case EditorAssetBrowserTool::SHOW_TYPE_EDIT: 
        {
            ImGui::BeginChild("Column", ImVec2(availSpace.x, frameHeight));
            ImGuiHelper::TextWithVerticalSeparator(u8" 이름"_c_str, availSpace.x * ReflectFields->ListColumnWidth[0]);
            ImGuiHelper::TextWithVerticalSeparator(u8" 수정한 날짜"_c_str, availSpace.x * ReflectFields->ListColumnWidth[1]);
            ImGuiHelper::TextWithVerticalSeparator(u8" 에셋 ID"_c_str, availSpace.x * ReflectFields->ListColumnWidth[2]);
            ImGui::TextUnformatted(u8" 파일 유형"_c_str);
            ImGui::EndChild();
            break;
        }
    default:
        break;
    }
}

bool EditorAssetBrowserTool::ShowDeletePopupBox(const File::Path& path)
{
    ImGui::Text(u8"정말 삭제하시겠습니까?"_c_str);

    ImGui::Text(u8"경로: "_c_str);
    ImGui::SameLine();
    ImGui::Text(path.string().c_str());

    ImGui::PushID(&path);

    if (ImGui::Button("OK##") || ImGui::IsKeyReleased(ImGuiKey::ImGuiKey_Enter))
    {
        ImGui::CloseCurrentPopup();
        ImGui::PopID();
        return true;
    }

    ImGui::SameLine();

    if (ImGui::Button("NO##") || ImGui::IsKeyReleased(ImGuiKey::ImGuiKey_Escape))
    {
        ImGui::CloseCurrentPopup();
    }

    ImGui::PopID();

    return false;
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
    std::string curPath = _dragDropPath.string();
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
        File::ShowOpenFolderDialog(NULL, L"경로 선택", _dragDropPath.c_str(), _dragDropPath);
    }
    ImGui::Separator();
    if (ImGui::Button("Copy"))
    {
        for (const auto& [check, targetPath] : _dragDropPaths)
        {
            if (check)
            {
                File::Path from = targetPath;
                File::Path to   = _dragDropPath / from.filename();
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

void EditorAssetBrowserTool::ShowAlreadyAssetIDPopupBox(const File::Path& path, int changeID)
{
    const File::Path& dstPath = UmFileSystem.GetPathFromAssetID(changeID);
    std::string dstPathStr = dstPath.string();
    std::string text = std::format("{} {}", changeID, (const char*)u8"ID는 이미 사용중인 ID입니다. 아래 경로의 Asset을 확인해주세요.");
    ImGui::Text(text.c_str());
    //ImGui::BeginDisabled();
    ImGui::SetNextItemWidth(ImGui::CalcTextSize(text.c_str()).x);
    ImGui::InputText("##AssetID", &dstPathStr, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_AutoSelectAll);
    //ImGui::EndDisabled();
    ImGuiHelper::HoveredToolTip(dstPathStr.c_str());

    if (ImGui::Button("OK##"))
    {
        ImGui::CloseCurrentPopup();
    }
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
            _dragDropPath = GetCurrentFocusFolderPath();
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

void EditorAssetBrowserTool::ResetState() 
{
    SetFocusEntryPath("");
    SetFocusFolderPath(UmFileSystem.GetRootPath(), false);
    _search.ClearBuffer();
    _rename.CancelRename();
    ClearUndoRedoStack();
}

void EditorAssetBrowserTool::ProcessInput()
{
    bool isRootpath    = (_focusFolderPath == UmFileSystem.GetRootPath());
    bool isFileCopying = (_copyBuffer.first == 0);
    bool isFileCutting = (_copyBuffer.first == 1);

    bool isKeyCtrlDown   = ImGui::IsKeyDown(ImGuiKey_LeftCtrl);
    bool isMouseXbutton1 = ImGui::IsMouseClicked(ImGuiMouseButton_XButton1, false);
    bool isMouseXbutton2 = ImGui::IsMouseClicked(ImGuiMouseButton_XButton2, false);
    bool isKeyEnter      = ImGui::IsKeyPressed(ImGuiKey_Enter, false);
    bool isKeyDelete     = ImGui::IsKeyPressed(ImGuiKey_Delete, false);
    bool isKeyBackSpace  = ImGui::IsKeyPressed(ImGuiKey_Backspace, false);
    bool isKeyEsc        = ImGui::IsKeyPressed(ImGuiKey_Escape, false);
    bool isKeyF2         = ImGui::IsKeyPressed(ImGuiKey_F2, false);
    bool isKeyC          = ImGui::IsKeyPressed(ImGuiKey_C, false);
    bool isKeyX          = ImGui::IsKeyPressed(ImGuiKey_X, false);
    bool isKeyV          = ImGui::IsKeyPressed(ImGuiKey_V, false);

    AssetData* focusAssetData = GetAssetData(_focusEntryPath);
    File::Path parentPath     = _focusFolderPath.parent_path();

    if (focusAssetData)
    {
        if (isKeyF2)
        {
            if (false == _rename.IsActive() || parentPath != _focusEntryPath)
            {
                _rename.StartRename(focusAssetData->Entry.path());
            }
        }
        else if (isKeyEsc)
        {
            if (isFileCutting)
            {
                _copyBuffer.first = -1; // 클립보드 초기화
            }
            else if (_rename.IsActive())
            {
                _rename.CancelRename();
            }
        }
        else if (isKeyDelete)
        {
            File::Path focusPath = _focusEntryPath;
            Global::editorModule->OpenPopupBox("Delete File", [this, focusPath]() {
                _rename.CancelRename();
                if (ShowDeletePopupBox(focusPath))
                {
                    DeleteFileFromPath(focusPath);
                }
            });
        }
        else if (isKeyEnter && false == _rename.IsActive())
        {
            if (focusAssetData->IsDirectory)
            {
                SetFocusFolderPath(focusAssetData->Entry.path());
            }
            else
            {
                UmFileSystem.RequestOpenFile(focusAssetData->Entry.path());
            }
        }
    }
    if (isMouseXbutton1)
    {
        UndoPath();
    }
    else if (isMouseXbutton2)
    {
        RedoPath();
    }
    if (isKeyCtrlDown)
    {
        float wheelY = ImGui::GetIO().MouseWheel;
        if (wheelY != 0.0f)
        {
            _zoomScale += wheelY * 0.1f;
            _zoomScale   = ImClamp(_zoomScale, 0.5f, 2.0f);
            _needRefresh = true;
        }
        if (isKeyC && fs::exists(_focusEntryPath))
        {
            SetCopyFileFromPath(_focusEntryPath);
        }
        if (isKeyX && fs::exists(_focusEntryPath))
        {
            SetCutFileFromPath(_focusEntryPath);
        }
        if (isKeyV && false == _search.IsActive())
        {
            PasteFile();
        }
    }
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
            assetData.Refresh(entry);

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

            UmFileSystem.CheckFileContextIntegrity(assetData.Entry.path());
        }
        std::sort(_focusFolderAssetDataList.begin(), _focusFolderAssetDataList.end(), Compare(ReflectFields->SortFlags));
        _updateTime = 0.0f;
    }
}

void EditorAssetBrowserTool::SetFocusFolderPath(const File::Path& path, bool pushStack)
{
    File::Path focusPath = path.generic_string();
    if (false == focusPath.empty() && fs::exists(focusPath) && fs::is_directory(focusPath))
    {
        if (_focusFolderPath != focusPath)
        {
            if (pushStack)
            {
                _directoryRedoStack.clear();
                _directoryUndoStack.push_back(_focusFolderPath);
                if (_directoryUndoStack.size() > _maxUndoStack)
                {
                    _directoryUndoStack.pop_front();
                }
            }
            _focusFolderPath = focusPath;
            RefreshFocusFolderEntries();
            _rename.CancelRename();
        }
    }
    else
    {
        if (pushStack)
        {
            _directoryUndoStack.clear();
            _directoryRedoStack.clear();
        }
    }
}

void EditorAssetBrowserTool::SetFocusEntryPath(const File::Path& path) 
{
    File::Path focusPath = path.generic_string();
    if (fs::exists(focusPath) && _focusEntryPath != focusPath)
    {
        File::Path parent = focusPath.parent_path();
        _focusEntryPath = focusPath;
        SetFocusFolderPath(parent.generic_string());
        _rename.CancelRename();
    }
}

void EditorAssetBrowserTool::ClearUndoRedoStack() 
{
    _directoryUndoStack.clear();
    _directoryRedoStack.clear();
}

void EditorAssetBrowserTool::UndoPath()
{
    if (false == _directoryUndoStack.empty())
    {
        const File::Path undoPath = _directoryUndoStack.back();
        _directoryRedoStack.push_back(_focusFolderPath);
        _directoryUndoStack.pop_back();
        SetFocusFolderPath(undoPath, false);
    }
}

void EditorAssetBrowserTool::RedoPath()
{
    if (false == _directoryRedoStack.empty())
    {
        const File::Path redoPath = _directoryRedoStack.back();
        _directoryUndoStack.push_back(_focusFolderPath);
        _directoryRedoStack.pop_back();
        SetFocusFolderPath(redoPath, false);
    }
}

void EditorAssetBrowserTool::SetCopyFileFromPath(const File::Path& path)
{
    _copyBuffer.first  = 0;
    _copyBuffer.second = path;
    UmFileSystem.RequestCopyFile(_copyBuffer.second);
}

void EditorAssetBrowserTool::SetCutFileFromPath(const File::Path& path)
{
    _copyBuffer.first  = 1;
    _copyBuffer.second = path;
}

void EditorAssetBrowserTool::PasteFile() 
{
    if (-1 != _copyBuffer.first)
    {
        File::Path from = _copyBuffer.second;
        File::Path to   = (_focusFolderPath / from.filename());
        bool isFromExists = fs::exists(from);
        if (isFromExists)
        {
            if (0 == _copyBuffer.first)
            {
                File::CopyFileFromTo(from, to);
                UmFileSystem.RequestPasteFile(to);
            }
            if (1 == _copyBuffer.first)
            {
                if (from != to)
                {
                    fs::rename(from, to);
                }
                _copyBuffer.first = -1;
            }
            _needRefresh = true;
        }
    }
}

bool EditorAssetBrowserTool::DeleteFileFromPath(const File::Path& path)
{
    if (fs::exists(path))
    {
        if (_focusEntryPath == path)
        {
            SetFocusEntryPath("");
            _inspectorDrawer->SetAsset(nullptr); // 인스펙터 비우기
        }
        File::RemoveFile(path);
        _needRefresh = true; // 삭제 후 새로고침 필요
        return true;
    }
    return false;
}

bool EditorAssetBrowserTool::IsFavoriteFolder(const File::Path& path) const
{
    const File::Path& root = UmFileSystem.GetRootPath();
    std::string pathStr = fs::relative(path, root).generic_string();
    auto itr = ReflectFields->FavoriteFolders.find(pathStr);
    if (itr != ReflectFields->FavoriteFolders.end())
    {
        return true;
    }
    return false;
}

void EditorAssetBrowserTool::AddFavoriteFolder(const File::Path& path)
{
    const File::Path& root = UmFileSystem.GetRootPath();
    std::string pathStr = fs::relative(path, root).generic_string();
    auto itr = ReflectFields->FavoriteFolders.find(pathStr);
    if (itr == ReflectFields->FavoriteFolders.end())
    {
        // 폴더가 없으면 추가
        ReflectFields->FavoriteFolders.insert(pathStr);
    }
}

void EditorAssetBrowserTool::RemoveFavoriteFolder(const File::Path& path) 
{
    const File::Path& root = UmFileSystem.GetRootPath();
    std::string pathStr = fs::relative(path, root).generic_string();
    ReflectFields->FavoriteFolders.erase(pathStr);
}

bool EditorAssetBrowserTool::ChangeAssetID(const File::Path& path, int changeID)
{
    if (UmFileSystem.IsExistsAssetID(changeID))
    {
        Global::editorModule->OpenPopupBox(u8"이미 존재하는 Asset ID입니다."_c_str, [path, changeID]() {
            EditorAssetBrowserTool::ShowAlreadyAssetIDPopupBox(path, changeID);
        });
        return false; // 중복된 AssetID가 있으면 처리 중지
    }
    else
    {
        auto weakContext = UmFileSystem.GetContext(path);
        UmFileSystem.ChangeAssetID(weakContext, changeID);
        return true;
    }
}

bool EditorAssetBrowserTool::ChangeAutomaticAssetID(const AssetData& asset)
{
    size_t pos = 0;
    int    id  = 0;
    try
    {
        id = std::stoi(asset.FileName, &pos); // 앞 숫자만 추출
    }
    catch (const std::exception&)
    {
        id = 0;
    }
    return EditorAssetBrowserTool::ChangeAssetID(asset.Entry.path(), id);
}

bool EditorAssetBrowserTool::Compare::operator()(const AssetData* a, const AssetData* b) const
{
    if (a->Order != b->Order)
    {   // Order가 다르면 Order 오름차순으로 정렬
        return a->Order < b->Order; 
    }
    bool aIsDir = a->IsDirectory;
    bool bIsDir = b->IsDirectory;
    if (aIsDir != bIsDir)
    {
        return aIsDir > bIsDir;
    }
    else
    {
        if (flags == FLAGS_SORT_BY_TYPE)
        {
            return CompareByType(a, b); // 이름순
        }
        if (flags == FLAGS_SORT_BY_NAME)
        {
            return CompareByName(a, b); // 이름순
        }
        if (flags == FLAGS_SORT_BY_DATE)
        {
            return CompareByDate(a, b); // 날짜순
        }
        if (flags == FLAGS_SORT_BY_ASSET_ID)
        {
            return CompareByAssetID(a, b); // 날짜순
        }
    }
   
    return false;
}

bool EditorAssetBrowserTool::Compare::CompareByType(const AssetData* a, const AssetData* b) const
{
    const std::string& aIsExt = a->Extension;
    const std::string& bIsExt = b->Extension;
    return isAscending ? a->Extension > b->Extension : a->Extension < b->Extension;
}

bool EditorAssetBrowserTool::Compare::CompareByName(const AssetData* a, const AssetData* b) const
{
    const std::string& aName = a->FileName;
    const std::string& bName = b->FileName;
    return isAscending ? aName > bName : aName < bName;
}

bool EditorAssetBrowserTool::Compare::CompareByDate(const AssetData* a, const AssetData* b) const
{
    const auto& timeA = a->LastWriteTime;
    const auto& timeB = b->LastWriteTime;
    return isAscending ? timeA > timeB : timeA < timeB;
}

bool EditorAssetBrowserTool::Compare::CompareByAssetID(const AssetData* a, const AssetData* b) const
{
    const int idA = UmFileSystem.GetAssetIDFromPath(a->Entry.path());
    const int idB = UmFileSystem.GetAssetIDFromPath(b->Entry.path());
    return isAscending ? idA > idB : idA < idB;
}

EditorAssetBrowserTool::AssetData::AssetData(FileEntry entry) 
    : Entry(entry)
{
    IsDirectory = fs::is_directory(entry);
    FileName    = entry.path().filename().string();
    Extension   = entry.path().extension().string().c_str() + 1;
    File::Path extension = entry.path().extension();

    static std::unordered_set<File::Path> imageFormat = {".jpg", ".png", ".dds", ".jpeg", ".bmp"};
    static std::unordered_set<File::Path> modelFormat = {".fbx", ".FBX",".obj"};
    static std::unordered_set<File::Path> soundFormat = {".wav"};
    static std::unordered_set<File::Path> prefabFormat = {EGameObjectFactory::PREFAB_EXTENSION};
    static std::unordered_set<File::Path> vfxFormat = {".vfx"};
    static std::unordered_set<File::Path> animFormat = {AnimationEventTrack::EXTENSION};
    static std::unordered_set<File::Path> settingFormat = {File::SETTING_EXTENSION};

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
    else if (soundFormat.find(extension) != soundFormat.end())
    {
        PreviewIconTexture = UmResourceManager->LoadResource<Texture>(L"../GameEngine/Icon/Editor/Icon_File_Sound.png");
    }
    else if (prefabFormat.find(extension) != prefabFormat.end())
    {
        PreviewIconTexture = UmResourceManager->LoadResource<Texture>(L"../GameEngine/Icon/Editor/Icon_File_Prefab.png");
    }
    else if (vfxFormat.find(extension) != vfxFormat.end())
    {
        PreviewIconTexture = UmResourceManager->LoadResource<Texture>(L"../GameEngine/Icon/Editor/Icon_File_VFX.png");
    }
    else if (animFormat.find(extension) != animFormat.end())
    {
        PreviewIconTexture = UmResourceManager->LoadResource<Texture>(L"../GameEngine/Icon/Editor/Icon_File_Animation.png");
    }
    else if (settingFormat.find(extension) != settingFormat.end())
    {
        PreviewIconTexture = UmResourceManager->LoadResource<Texture>(L"../GameEngine/Icon/Editor/Icon_File_Setting.png");
    }
    else
    {
        PreviewIconTexture = UmResourceManager->LoadResource<Texture>(L"../GameEngine/Icon/Editor/Icon_File_Default.png");
    }
}

void EditorAssetBrowserTool::AssetData::Refresh(FileEntry entry) 
{
    Entry         = entry;
    LastWriteTime = File::GetFileLastWriteTime(entry);
    ViewName      = FileName;
}

bool EditorAssetBrowserTool::AssetData::IsSamePath(const std::filesystem::path& path) const
{
    return Entry.path() == path;
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

void EditorAssetBrowserTool::InspectorDrawer::OnInspectorEnter() 
{
    if (_assetData && fs::exists(_assetPath))
    {
        _selectedAsset = UmFileSystem.GetContext(_assetData->Entry.path());
    }
    else
    {
        _assetData = nullptr;
        _assetPath = "";
        _selectedAsset.reset(); 
    }
}

void EditorAssetBrowserTool::InspectorDrawer::OnInspectorStay()
{
    if (nullptr == _assetData || false == fs::exists(_assetPath))
    {
        return;
    }

    ImGuiStyle& style  = ImGui::GetStyle();

    bool   isDebug      = Global::editorModule->IsDebugMode();
    bool   isExpired    = _selectedAsset.expired();
    float  offsetX      = 150.0f;
    ImVec2 availSpace   = ImGui::GetContentRegionAvail();
    ImVec2 cursorPos    = ImGui::GetCursorPos();
    ImVec2 iconSize     = ImVec2(64, 64);
    ImVec2 padding      = style.ItemSpacing;

    if (_assetData->PreviewIconTexture)
    {
        D3D12_GPU_DESCRIPTOR_HANDLE iconHandle = _assetData->PreviewIconTexture->GetGPUHandle();
        ImGui::Image((ImTextureID)iconHandle.ptr, iconSize);
        ImGui::SetCursorPos(cursorPos + ImVec2(iconSize.x + padding.x, 0.0f));
    }

    ImGui::Text(_assetData->FileName.c_str());
    

    const char* buttonLabel = "Open File";
    ImVec2 buttonSize = ImGui::CalcTextSize(buttonLabel) + padding;
    ImGui::SetCursorPos(cursorPos + ImVec2(availSpace.x - buttonSize.x, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
    if (ImGui::Button(buttonLabel, buttonSize))
    {
        File::OpenFile(_assetData->Entry.path());
    }
    ImGui::PopStyleVar();

    ImGui::SetCursorPos(cursorPos + ImVec2(0.0f, iconSize.y + padding.y));
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Asset Info", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (false == _assetData->IsDirectory)
        {
            std::shared_ptr<File::Context> context;
            if (false == isExpired)
            {
                context = _selectedAsset.lock();
            }

            ImGuiHelper::TextWithVerticalSeparator("Asset Guid", offsetX);
            static std::string guidStr;
            if (context)
            {
                File::MetaData& meta = context->GetMeta();
                guidStr = meta.GetGuid().string();
                ImGui::InputText("##guid", &guidStr, ImGuiInputTextFlags_ReadOnly);
            }
            else
            {
                guidStr = "NULL_GUID";
                ImGui::InputText("##guid", &guidStr, ImGuiInputTextFlags_ReadOnly);
            }

            if (context)
            {
                ImGuiHelper::TextWithVerticalSeparator("AssetID", offsetX);
                ImGuiHelper::StyleBuilder assetIDStyle;
                File::MetaData& meta = context->GetMeta();
                int assetID = meta.GetAssetID();
                bool isUnique = _assetPath == UmFileSystem.GetPathFromAssetID(assetID);
                ImU32 borderColor = isUnique ? IM_COL32(100, 255, 100, 255) : IM_COL32(255, 100, 100, 255);
                assetIDStyle.PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f); // 테두리 두께 
                assetIDStyle.PushStyleColor(ImGuiCol_Border, borderColor);
                if (ImGui::InputInt("##asset_id", &assetID, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    EditorAssetBrowserTool::ChangeAssetID(_assetData->Entry.path(), assetID);
                }
                assetIDStyle.PopStyle();
            }
            
            ImGuiHelper::TextWithVerticalSeparator("File Last Write", offsetX);
            std::ostringstream oss;
            std::tm tm;
            localtime_s(&tm, &_assetData->LastWriteTime);
            oss << std::put_time(&tm, "%F %T"); // "YYYY-MM-DD HH:MM:SS" 형식
            ImGui::Text(oss.str().c_str());

            {
                ImGuiHelper::TextWithVerticalSeparator("File Size", offsetX);

                const char* units[]   = {"byte", "kb", "mb", "gb", "tb"};
                double      size      = static_cast<double>(_assetData->Entry.file_size());
                int         unitIndex = 0;

                while (size >= 1024.0 && unitIndex < 4) // 1024씩 나누면서 단위 변경
                {
                    size /= 1024.0;
                    ++unitIndex;
                }

                // 소수점 자릿수는 크기에 따라 다르게
                if (size < 10.0 && unitIndex > 0)
                    ImGui::Text("%.2f %s", size, units[unitIndex]);
                else if (size < 100.0 && unitIndex > 0)
                    ImGui::Text("%.1f %s", size, units[unitIndex]);
                else
                    ImGui::Text("%.0f %s", size, units[unitIndex]);
            }
        }
    }
    ImGui::Separator();
    UmFileSystem.RequestInspectFile(_assetData->Entry.path());
}

void EditorAssetBrowserTool::InspectorDrawer::OnInspectorExit() 
{
}

void EditorAssetBrowserTool::InspectorDrawer::SetAsset(AssetData* assetData)
{
    if (assetData)
    {
        if (false == EditorInspectorTool::IsLockFocus())
        {
            _assetData     = assetData;
            _assetPath     = assetData->Entry.path().generic_string();
            _selectedAsset = UmFileSystem.GetContext(_assetData->Entry.path());
        }
    }
    else
    {
        _assetData = nullptr;
        _assetPath = "";
        _selectedAsset.reset(); 
    }
}

void EditorAssetBrowserTool::SearchController::UpdateBuffer()
{
    strncpy_s(SearchFilter.InputBuf, SearchBuffer.c_str(), SearchBuffer.length());
    SearchFilter.InputBuf[SearchBuffer.length()] = '\0'; // Ensure null-termination
    SearchFilter.Build();
}

void EditorAssetBrowserTool::SearchController::ClearBuffer() 
{
    SearchBuffer.clear();
    SearchFilter.InputBuf[0] = '\0'; // Ensure null-termination
    SearchFilter.Build();
    IsSearching = false;
}

bool EditorAssetBrowserTool::SearchController::PassFilter(const char* text)
{
    return SearchFilter.PassFilter(text);
}
