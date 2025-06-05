#include "pch.h"
#include "NodeGraphHoudiniNode.h"

namespace NodeGraph
{
    HoudiniNode::HoudiniNode(EditorNodeGraph* owner, const char* name) 
        : Node(owner, name)
    {
        AddInputPin("Input1", PinType::Flow);
        AddOutputPin("Output1", PinType::Flow);
        AddOutputPin("Output2", PinType::Flow);
        
    }

    HoudiniNode::~HoudiniNode() {}

    void HoudiniNode::Draw()
    {
        DrawNode();
    }

    void HoudiniNode::DrawNode()
    {
        NodeEditorStyleColorBuilder styleColor;
        NodeEditorStyleVarBuilder   styleVar;

        styleColor.PushStyleColor(ed::StyleColor_NodeBg, ImColor(229, 229, 229, 200));
        styleColor.PushStyleColor(ed::StyleColor_NodeBorder, ImColor(125, 125, 125, 200));
        styleColor.PushStyleColor(ed::StyleColor_PinRect, ImColor(229, 229, 229, 60));
        styleColor.PushStyleColor(ed::StyleColor_PinRectBorder, ImColor(125, 125, 125, 60));

        styleVar.PushStyleVar(ed::StyleVar_NodePadding, ImVec4(0, 0, 0, 0));
        styleVar.PushStyleVar(ed::StyleVar_NodeRounding, _nodeRounding);
        styleVar.PushStyleVar(ed::StyleVar_SourceDirection, ImVec2(0.0f, 1.0f));
        styleVar.PushStyleVar(ed::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
        styleVar.PushStyleVar(ed::StyleVar_LinkStrength, 0.0f);
        styleVar.PushStyleVar(ed::StyleVar_PinBorderWidth, 1.0f);
        styleVar.PushStyleVar(ed::StyleVar_PinRadius, 6.0f);

        ed::BeginNode(_id);
        {
            ImGui::BeginVertical(_id.AsPointer());

            DrawInputPins();
            DrawLabel();
            DrawOutputPins();

            ImGui::EndVertical();
        }
        ed::EndNode();
    }

    void HoudiniNode::DrawInputPins()
    {
        ed::Style& style = ed::GetStyle();

        if (false == _inputPinList.empty())
        {
            ImGui::BeginHorizontal("InputPin");
            ImGui::Spring(1, 0);
            ImVec4 pinBgColor = style.Colors[ed::StyleColor_NodeBg];
            for (NodeGraph::Pin& pin : _inputPinList)
            {
                ImGui::Dummy(ImVec2(_pinPadding, _pinPadding));
                ImRect pinRect = ImGuiHelper::GetItemRect();
                pinRect.Min.y -= _pinPadding;
                pinRect.Max.y -= _pinPadding;

                ImGui::Spring(1, 0);

                DrawPin(&pin, pinBgColor, pinRect, ImDrawFlags_RoundCornersAll);

                const auto& state   = _owner->GetState();
                Pin*        self    = &pin;
                Pin*        linkPin = state.NewLinkPin;
                // 링크 시도 중일 때, 링크 가능한 핀을 제외하고 알파 낮추기
                if (nullptr != linkPin && false == CanLink(linkPin, self) && self != linkPin)
                {
                    ImGuiStyle& style = ImGui::GetStyle();
                    pinBgColor.w      = style.Alpha * 0.2f;
                }
            }
            ImGui::EndHorizontal();
        }
    }

    void HoudiniNode::DrawOutputPins() 
    {
        ed::Style& style = ed::GetStyle();

        if (false == _outputPinList.empty())
        {
            ImGui::BeginHorizontal("OutputPin");
            ImGui::Spring(1, 0);
            ImVec4 pinBgColor = style.Colors[ed::StyleColor_NodeBg];
            for (NodeGraph::Pin& pin : _outputPinList)
            {
                ImGui::Dummy(ImVec2(_pinPadding, _pinPadding));
                ImRect pinRect = ImGuiHelper::GetItemRect();
                pinRect.Min.y += _pinPadding;
                pinRect.Max.y += _pinPadding;

                ImGui::Spring(1, 0);

                DrawPin(&pin, pinBgColor, pinRect, ImDrawFlags_RoundCornersAll);

                const auto& state   = _owner->GetState();
                Pin*        self    = &pin;
                Pin*        linkPin = state.NewLinkPin;
                if (nullptr != linkPin && false == CanLink(linkPin, self) && self != linkPin)
                {
                    ImGuiStyle& style = ImGui::GetStyle();
                    pinBgColor.w      = style.Alpha * 0.2f;
                }
            }
            ImGui::EndHorizontal();
        }
    }

    void HoudiniNode::DrawLabel() 
    {
        ImGui::BeginHorizontal("content_frame");
        ImGui::Spring(1, _pinPadding);

        ImGui::BeginVertical("content", ImVec2(0.0f, 0.0f));
        ImGui::Dummy(ImVec2(160, 0));
        ImGui::Spring(1);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        ImGui::TextUnformatted(_name.c_str());
        ImGui::PopStyleColor();
        ImGui::Spring(1);
        ImGui::EndVertical();

        ImGui::Spring(1, _pinPadding);
        ImGui::EndHorizontal();
    }

    void HoudiniNode::DrawPin(Pin* pin, ImVec4 color, ImRect rect, int roundFlag)
    {
        float  pinRounding = 4.0f;
        ImVec2 center      = rect.GetCenter();
        ImVec2 topLeft     = rect.GetTL();
        ImVec2 bottomRight = rect.GetBR();
        ImU32  pinBgColor  = NodeGraph::GetColor4ToImU32(color);

        ed::PushStyleVar(ed::StyleVar_PinCorners, roundFlag);
        ed::BeginPin(pin->GetPinID(), pin->GetPinKind());
        {   // NodeEditorContext에 Pin정보 갱신
            ed::PinPivotRect(center, center);
            ed::PinRect(topLeft, bottomRight);
        }
        ed::EndPin();
        ed::PopStyleVar(1);

        // 핀 사각형 영역 (실질적인 Draw)
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(topLeft, bottomRight, pinBgColor, pinRounding, roundFlag);
        drawList->AddRect(topLeft, bottomRight, pinBgColor, pinRounding, roundFlag);
    }

} // namespace NodeGraph