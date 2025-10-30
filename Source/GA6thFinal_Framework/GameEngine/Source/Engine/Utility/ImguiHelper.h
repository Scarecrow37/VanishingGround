#pragma once
class Camera;
class EditorDynamicCamera;

namespace ImGuiHelper
{
    enum DirectionType {
        LEFT, LEFTUP, LEFTDOWN,
        RIGHT, RIGHTUP, RIGHTDOWN,
        UP,
        DOWN,
        CENTER,
    };

    /// <summary>
    /// Preview 렉트를 인풋 텍스트 기반으로 하는 콤보
    /// </summary>
    bool BeginComboInput(const char* label, const char* preview_value,
                         ImGuiInputTextFlags inputTextFlags = ImGuiInputTextFlags_ReadOnly,
                         ImGuiComboFlags     comboFlags     = ImGuiComboFlags_None);
    /// <summary>
    /// 인풋 텍스트에 입력을 할 수 있는 콤보
    /// </summary>
    bool BeginComboInput(const char* label, std::string* inputBuffer,
                         ImGuiInputTextFlags inputTextFlags = ImGuiInputTextFlags_ReadOnly,
                         ImGuiComboFlags     comboFlags     = ImGuiComboFlags_None);
    /*
    호버링시 둘팁을 여는 텍스트를 생성
    desc = 툴팁에 나타날 텍스트
    mark = 호버링용 텍스트 글씨
    */
    static void TooltipMarker(const char* desc, const char* mark = "(?)")
    {
        ImGui::TextDisabled(mark);
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    /*
    정렬 텍스트
    *주의: scale값은 기존 폰트를 확장시키므로 텍스트에 계단현상이 발생할 수 있음.
    */
    static void AlignedText(const char* text, DirectionType dir = LEFT, float scale = 1.0f)
    {
        if (scale > 0)
        {
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            float old = window->FontWindowScale;
            ImGui::SetWindowFontScale(scale);

            float windowWidth = ImGui::GetWindowSize().x;
            float textWidth = ImGui::CalcTextSize(text).x;
            float weight = 0.0f;
            float offset = 0.0f;

            switch (dir)
            {
            case ImGuiHelper::LEFT:
                weight = 0.0f;
                offset = ImGui::GetStyle().FramePadding.x;
                break;
            case ImGuiHelper::RIGHT:
                weight = 1.0f;
                offset = -ImGui::GetStyle().FramePadding.x;
                break;
            case ImGuiHelper::CENTER:
                weight = 0.5f;
                offset = 0.0f;
                break;
            default:
                break;
            }

            // 정렬에 맞게 X 위치를 조정
            ImGui::SetCursorPosX((windowWidth - textWidth) * weight + offset);
            ImGui::Text("%s", text);

            ImGui::SetWindowFontScale(old);
        }
    }

    /// <summary>
    /// 윈도우의 탭바 영역을 반환합니다.
    /// </summary>
    /// <returns></returns>
    ImRect GetWindowTabBarRect();

    /// <summary>
    /// 여백있는 구분선
    /// </summary>
    /// <param name="upPadding">위 여백</param>
    /// <param name="downPadding">아래 여백</param>
    void Separator(float upPadding, float downPadding);
    void Separator(float spacing = 5.0f);

    /// <summary>
    /// <para>텍스트에 수직 구분선을 적용하여 출력합니다. startX는 구분선의 위치를 조정하는데 사용됩니다.</para>
    /// <para>기본 값으론 텍스트 크기에 따라 자동으로 계산됩니다.</para>
    /// </summary>
    /// <param name="text">출력할 텍스트</param>
    /// <param name="startX">구분선 위치</param>
    void TextWithVerticalSeparator(const char* text, float startX = FLT_MAX);

    void TextWithVerticalSeparatorEx(const char* text, float startX = FLT_MAX);

    /*
    토글이 가능한 버튼 (false->true / true->false)
    return: 버튼을 눌렀다 뗄 때
    */
    static bool ToggleButton(const char* label, bool* v, const ImVec4& trueColor, const ImVec4& falseColor)
    {
        if (v)
        {
            if (*v)
                ImGui::PushStyleColor(ImGuiCol_Button, trueColor); // 활성화 색상
            else
                ImGui::PushStyleColor(ImGuiCol_Button, falseColor); // 비활성화 색상

            bool clicked = ImGui::Button(label);
            if (clicked)
                *v = !*v;

            ImGui::PopStyleColor();
            return clicked;
        }
        return false;
    }

    /*
    로딩
    */
    static void LoadingSpinner(float radius = 10.0f, ImVec4 color = ImVec4(1, 1, 1, 1))
    {
        static int segments = 20;
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 center = ImGui::GetCursorScreenPos();
        center.x += radius;
        center.y += radius;

        float start = (float)ImGui::GetTime() * 3.0f;
        float angle_offset = 2.0f * IM_PI / segments;

        for (int i = 0; i < segments - 4; i++)
        {
            float angle = start + (i * angle_offset);
            ImVec2 p1 = ImVec2(center.x + cosf(angle) * radius, center.y + sinf(angle) * radius);
            ImVec2 p2 = ImVec2(center.x + cosf(angle + angle_offset) * radius, center.y + sinf(angle + angle_offset) * radius);
            draw_list->AddLine(p1, p2, ImGui::GetColorU32(color), 2.0f);
        }
    }

    // Make the UI compact because there are so many fields
    static void PushStyleCompactToFrame(float _val = 0.60f)
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x * _val, style.FramePadding.y * _val));
    }
    static void PushStyleCompactToItem(float _val = 0.60f)
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style.FramePadding.x * _val, style.FramePadding.y * _val));
    }
    static void PopStyleCompact()
    {
        ImGui::PopStyleVar();
    }
    
    /*
    색상 선택 버튼
    */
    static bool ColorPickerButton(const char* label, ImVec4* color)
    {
        bool colorChanged = false;
        // 버튼에 현재 색상을 표시
        if (ImGui::ColorButton(label, *color))
        {
            ImGui::OpenPopup(label);
        }
        if (ImGui::BeginPopup(label))
        {
            // 기본 색상 편집기를 팝업 내에 표시
            colorChanged = ImGui::ColorPicker4("##picker", (float*)color);
            ImGui::EndPopup();
        }
        return colorChanged;
    }

    static void DrawFillRect(const ImVec2& leftTop, const ImVec2& rightBottom, const ImU32& color, float round = 0.0f,
                             ImDrawFlags flag = 0)
    {
        ImDrawList* drawlist = ImGui::GetWindowDrawList();
        ImVec2      offset   = ImGui::GetCursorScreenPos();
        ImVec2      size     = rightBottom - leftTop;        // 크기 계산
        ImVec2      a        = offset;                       // 왼쪽 위
        ImVec2      b        = offset + size;                // 오른쪽 아래
        ImU32       col      = color;                        // 노란색
        float       rounding = round;                        // 모서리 라운딩 반경
        ImDrawFlags flags    = flag;                         // 모든 모서리 라운딩

        drawlist->AddRectFilled(a, b, col, rounding, flags);
    }

    static bool IsWindowDrawable(ImGuiWindow* window = nullptr)
    {
        if (!window)
            window = ImGui::GetCurrentWindowRead();
        if (!window)
            return false;

        return !window->SkipItems;
    }

    class StyleBuilder
    {
    public:
        StyleBuilder() = default;
        ~StyleBuilder() { PopStyle(); }

    public:
        template <typename T>
        void PushStyleVar(int idx, const T& val)
        {
            ImGui::PushStyleVar(idx, val);
            ++_pushStyleVarCount;
        }
        template <typename T>
        void PushStyleColor(int idx, const T& color)
        {
            ImGui::PushStyleColor(idx, color);
            ++_pushStyleColCount;
        }
        void PopStyle()
        {
            if (_pushStyleVarCount > 0)
            {
                ImGui::PopStyleVar(_pushStyleVarCount);
                _pushStyleVarCount = 0;
            }
            if (_pushStyleColCount > 0)
            {
                ImGui::PopStyleColor(_pushStyleColCount);
                _pushStyleColCount = 0;
            }
        }

    private:
        int _pushStyleVarCount = 0;
        int _pushStyleColCount = 0;
    };

    class DragDrop
    {
        using EventID = const char*;
    public:
        /* 드래그앤드롭을 시작합니다. */
        template <typename T>
        static bool SendDragDropEvent(EventID id, const T* sourceData, std::function<void()> func)
        {
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
            {
                ImGui::SetDragDropPayload(id, sourceData, sizeof(T));
                if (func)
                {
                    func();
                }
                ImGui::EndDragDropSource();
                return true;
            }
            return false;
        }

        /* 현재 아이템 기준으로 드래그앤드롭 데이터를 받습니다. */
        template <typename T>
        static bool RecieveItemDragDropEvent(EventID id, T* outData)
        {
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(id))
                {
                    if (nullptr != outData)
                    {
                        memcpy(outData, payload->Data, sizeof(T));
                    }
                    ImGui::EndDragDropTarget();
                    return true;
                }
                ImGui::EndDragDropTarget();
            }
            return false;
        }

        /* 현재 프레임 대상으로 드래그앤드롭을 데이터를 받습니다. */
        template <typename T>
        static bool RecieveFrameDragDropEvent(EventID id, T* outData)
        {
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            ImRect       rect   = window->Rect(); // 윈도우 전체 영역

            if (ImGui::BeginDragDropTargetCustom(rect, window->ID))
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(id))
                {
                    // 데이터 크기가 일치하는지 확인
                    int size = sizeof(T);
                    if (payload->DataSize == size)
                    {
                        if (nullptr != outData)
                        {
                            memcpy(outData, payload->Data, sizeof(T));
                        }
                        ImGui::EndDragDropTarget();
                        return true;
                    }
                }
                ImGui::EndDragDropTarget();
            }
            return false;
        }

        /* 드래그앤드롭 여부를 반환 */
        static bool IsDragDrop()
        {
            return ImGui::IsDragDropActive();
        }

        /* 드롭된 데이터 가져오기(드래그가 끝났을 때 사용) */ 
        template <typename T>
        static bool GetRecievedData(const ImGuiPayload* payload, T& outData)
        {
            if (payload && payload->DataSize == sizeof(T))
            {
                memcpy(&outData, payload->Data, sizeof(T));
                return true;
            }
            return false;
        }
    };
}


//by KimSiwoo
namespace ImGuiHelper
{
    std::array<float, 4> ImVec4ToArray(const ImVec4& vec4);
    ImVec4               ArrayToImVec4(const std::array<float, 4>& array);

    /// <summary>
    /// 이전 아이템에 마우스가 올라가면 툴팁을 출력합니다.
    /// </summary>
    /// <param name="toolTip :">출력할 내용</param>
    /// <returns>마우스 Hovered 여부</returns>
    bool HoveredToolTip(std::string_view toolTip, int flags = 0);
    bool HoveredToolTip(std::u8string_view toolTip, int flags = 0);

    /// <summary>
    /// ImVec4를 선형보간합니다.
    /// </summary>
    /// <param name="a"></param>
    /// <param name="b"></param>
    /// <param name="t"></param>
    /// <returns></returns>
    inline constexpr ImVec4 ImVec4Lerp(const ImVec4& a, const ImVec4& b, float t)
    {
        return ImVec4(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t);
    }

    //ViewManipulate용 데이터 구조체
    struct ViewManipulateDesc
    {
        // clientTop 좌표
        float ClientTop = 0.f;
        // clientRight 좌표
        float ClientRight = 0.f;
        // 크기
        ImVec2 Size = {0.f, 0.f};
        // 배경 색
        ImU32 BackgroundColor = 0x10101010;
    };

    // DrawManipulate용 데이터 구조체
    struct DrawManipulateDesc
    {
        ImGuizmo::OPERATION Operation = ImGuizmo::OPERATION::UNIVERSAL;

        ImGuizmo::MODE Mode = ImGuizmo::MODE::WORLD;
        //
        bool UseSnap = true;
        //snap 값
        std::array<float, 3> Snap{1.f, 1.f, 1.f};

        //View Manipulate용
        ViewManipulateDesc ViewDesc;
    };

    /// <summary>
    /// 전달받은 오브젝트의 Guizmo를 Draw 합니다. 반드시 ImGuizmo::SetRect() 설정 이후 호출해야합니다.
    /// </summary>
    /// <param name="pObject"></param>
    bool DrawManipulate(Camera* pCamera, Matrix* pObjectMatrix, DrawManipulateDesc& desc);

    /// <summary>
    /// 전달받은 오브젝트의 Guizmo를 Draw 합니다. 반드시 ImGuizmo::SetRect() 설정 이후 호출해야합니다.
    /// </summary>
    /// <param name="pObject"></param>
    bool DrawManipulate(EditorDynamicCamera* pDynamicCamera, Matrix* pObjectMatrix, DrawManipulateDesc& desc);

    /// <summary>
    /// 트리 스타일에 텍스트 버튼입니다.
    /// </summary>
    /// <param name="label :">사용할 라벨</param>
    /// <param name="width :">너비</param>
    /// <returns></returns>
    inline bool TreeStyleTextButton(const char* label, float width = 0.0f)
    {
        ImGuiStyle& style    = ImGui::GetStyle();
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        float frameHeight = ImGui::GetFontSize() + style.FramePadding.y * 2;
        if (width <= 0.0f)
            width = ImGui::GetContentRegionAvail().x;

        ImVec2 pos  = ImGui::GetCursorScreenPos();
        ImVec2 size = ImVec2(width, frameHeight);

        // 클릭 감지를 위한 InvisibleButton
        ImGui::InvisibleButton(label, size);

        // 상태 감지
        bool hovered = ImGui::IsItemHovered();
        bool held    = ImGui::IsItemActive();
        bool clicked = ImGui::IsItemClicked();

        // 상태별 색상 설정
        ImVec4 baseColor    = ImGui::GetStyleColorVec4(ImGuiCol_Header);
        ImVec4 hoveredColor = ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered);
        ImVec4 activeColor  = ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive);

        ImU32 bgCol = ImGui::ColorConvertFloat4ToU32(held ? activeColor : (hovered ? hoveredColor : baseColor));

        // 배경 그리기
        drawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), bgCol, style.FrameRounding);

        // 텍스트 위치 계산
        ImVec2 textSize = ImGui::CalcTextSize(label);
        ImVec2 textPos  = ImVec2(pos.x + style.FramePadding.x, pos.y + (frameHeight - textSize.y) * 0.5f);

        // 텍스트 그리기
        drawList->AddText(textPos, ImGui::GetColorU32(ImGuiCol_Text), label);

        return clicked;
    }

    template <typename Enum, size_t Count = rfl::get_enumerator_array<Enum>().size()>
    void EnumCombo(const char* label, Enum prev, const std::function<void(const std::pair<std::string_view, Enum>)>& callback, const std::array<std::u8string_view, Count>* toolTips = nullptr)
    {
        constexpr auto enumerator = rfl::get_enumerator_array<Enum>();
        if (ImGui::BeginCombo(label, rfl::enum_to_string(prev).c_str()))
        {
            int i = 0;
            for (auto& pair : enumerator)
            {
                auto& [key, value] = pair;
                ImGui::PushID(i);
                {
                    if (ImGui::Selectable(key.data(), value == prev))
                    {
                        if (callback)
                        {
                            callback(pair);
                        }
                    }

                    if (toolTips)
                    {
                        auto& toolTipArray = *toolTips;
                        if (i < toolTipArray.size())
                        {
                            ImGuiHelper::HoveredToolTip((const char*)toolTipArray[i].data());
                        }                        
                    }
                }
                ImGui::PopID();
                ++i;
            }
            ImGui::EndCombo();
        }
    }

    template <typename Enum, size_t Count = rfl::get_enumerator_array<Enum>().size()>
    void EnumCombo(const char* label, Enum prev, const std::function<void(const std::pair<std::string_view, Enum>)>& callback, const std::array<std::string_view, Count>* toolTips = nullptr)
    {
        constexpr auto enumerator = rfl::get_enumerator_array<Enum>();
        if (ImGui::BeginCombo(label, rfl::enum_to_string(prev).c_str()))
        {
            int i = 0;
            for (auto& pair : enumerator)
            {
                auto& [key, value] = pair;
                ImGui::PushID(i);
                {
                    if (ImGui::Selectable(key.data(), value == prev))
                    {
                        if (callback)
                        {
                            callback(pair);
                        }
                    }

                    if (toolTips)
                    {
                        auto& toolTipArray = *toolTips;
                        ImGuiHelper::HoveredToolTip(toolTipArray[i].data());
                    }
                }
                ImGui::PopID();
                ++i;
            }
            ImGui::EndCombo();
        }
    }
}

namespace ImGuiHelper
{
    void CenterText(const char* text);
}