#pragma once

namespace ImGuiHelper
{
    enum DirectionType {
        LEFT, LEFTUP, LEFTDOWN,
        RIGHT, RIGHTUP, RIGHTDOWN,
        UP,
        DOWN,
        CENTER,
    };

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

            switch (dir)
            {
            case ImGuiHelper::LEFT:
                weight = 0.0f;
                break;
            case ImGuiHelper::RIGHT:
                weight = 1.0f;
                break;
            case ImGuiHelper::CENTER:
                weight = 0.5f;
                break;
            default:
                break;
            }

            // 정렬에 맞게 X 위치를 조정
            ImGui::SetCursorPosX((windowWidth - textWidth) * weight);
            ImGui::Text("%s", text);

            ImGui::SetWindowFontScale(old);
        }
    }

    /*
    여백있는 구분선
    */
    static void Separator(float upPadding, float downPadding)
    {
        ImGui::Dummy(ImVec2(0.0f, upPadding));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, downPadding));
    }
    static void Separator(float spacing = 5.0f)
    {
        ImGui::Dummy(ImVec2(0.0f, spacing));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, spacing));
    }

    /*
    토글이 가능한 버튼 (false->true / true->false)
    return: 버튼을 눌렀다 뗄 때
    */
    static bool ToggleButton(const char* label, bool* v, ImVec4 trueColor, ImVec4 falseColor)
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
        static bool RecieveDragDropEvent(EventID id, T* targetData)
        {
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(id))
                {
                    // 데이터 크기가 일치하는지 확인
                    if (payload->DataSize == sizeof(T))
                    {
                        memcpy(targetData, payload->Data, sizeof(T));
                        ImGui::EndDragDropTarget();
                        return true;
                    }
                }
                ImGui::EndDragDropTarget();
            }
            return false;
        }

        /* 현재 프레임 대상으로 드래그앤드롭을 데이터를 받습니다. */
        template <typename T>
        static bool RecieveFrameDragDropEvent(EventID id, T* targetData)
        {
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            ImRect       rect   = window->Rect(); // 윈도우 전체 영역

            if (ImGui::BeginDragDropTargetCustom(rect, window->ID))
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(id))
                {
                    // 데이터 크기가 일치하는지 확인
                    if (payload->DataSize == sizeof(T))
                    {
                        memcpy(targetData, payload->Data, sizeof(T));
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


//김시우가 만듬
namespace ImGuiHelper
{
    std::array<float, 4> ImVec4ToArray(const ImVec4& vec4);
    ImVec4               ArrayToImVec4(const std::array<float, 4>& array);
}