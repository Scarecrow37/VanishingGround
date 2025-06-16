#include "pch.h"
#include "BehaviorTreeNode.h"

namespace BehaviorTree
{
    void BTNode::Draw()
    {
        DrawNode();
    }

    void BTNode::OnCreate() {}

    void BTNode::OnNodePopup()
    {
        if (ImGui::MenuItem("Select"))
        {
            SetSeletion(true);
        }
        if (ImGui::MenuItem("Remove Node"))
        {
            NodeGraphContext* context = NodeGraph::GetCurrentNodeGraphContext();
            if (nullptr != context)
            {
                context->RemoveNodeFromNodeID(GetNodeID());
            }
        }
        if (ImGui::MenuItem("ChangeBg"))
        {
            _nodeRectFillColor   = ImColor(rand() % 256, rand() % 256, rand() % 256, 200);
            _nodeRectBorderColor = ImColor(rand() % 256, rand() % 256, rand() % 256, 0);
        }
    }

    void BTNode::OnPinPopup(UINT64 _pinID)
    {
        NodeGraph::Pin* pin = FindPin(_pinID);
        if (nullptr == pin)
            return;
        if (ImGui::MenuItem("Remove Pin"))
        {
            RemovePin(pin->GetPinID());
        }
        if (ImGui::MenuItem("Set Flow"))
        {
            bool isFlowForward = ed::PinKind::Input == pin->GetPinKind() ? true : false;
            for (int i = 0; i < pin->GetLinkCount(); ++i)
            {
                NodeGraph::Link* link = pin->GetLinkToIndex(i);
                if (nullptr != link)
                {
                    link->SetFlow(isFlowForward, 5.0f);
                }
            }
        }
        // Clear Link
        if (ImGui::MenuItem("Clear Links"))
        {
            pin->ClearLinks();
        }
    }

    void BTNode::OnQueryNewLink(NodeGraph::Pin* from, NodeGraph::Pin* to, bool isReject) 
    {
    }

    void BTNode::DrawNode()
    {
        NodeGraph::NodeEditorStyleColorBuilder styleColor;
        NodeGraph::NodeEditorStyleVarBuilder   styleVar;

        styleColor.PushStyleColor(ed::StyleColor_NodeBg, _nodeRectFillColor);
        styleColor.PushStyleColor(ed::StyleColor_NodeBorder, _nodeRectBorderColor);
        styleColor.PushStyleColor(ed::StyleColor_PinRect, ImColor(60, 180, 255, 150));
        styleColor.PushStyleColor(ed::StyleColor_PinRectBorder, ImColor(60, 180, 255, 150));

        styleVar.PushStyleVar(ed::StyleVar_NodeBorderWidth, 1.0f);
        styleVar.PushStyleVar(ed::StyleVar_NodePadding, ImVec4(0, 0, 0, 0));
        styleVar.PushStyleVar(ed::StyleVar_NodeRounding, _nodeRounding);
        styleVar.PushStyleVar(ed::StyleVar_SelectedNodeBorderWidth, 1.0f);
        styleVar.PushStyleVar(ed::StyleVar_SourceDirection, ImVec2(0.0f, 1.0f));
        styleVar.PushStyleVar(ed::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
        styleVar.PushStyleVar(ed::StyleVar_LinkStrength, 0.0f);
        styleVar.PushStyleVar(ed::StyleVar_PinBorderWidth, 1.0f);
        styleVar.PushStyleVar(ed::StyleVar_PinRadius, 5.0f);

        ed::BeginNode(GetNodeID());
        {
            ImGui::BeginVertical(this);

            CalculateInputPinRect();
            DrawLabel();
            CalculateOutputPinRect();

            ImGui::EndVertical();
        }
        ed::EndNode();

        DrawRect();
    }

    void BTNode::CalculateInputPinRect()
    {
        ImGui::BeginHorizontal("input");
        {

            if (true == HasInputPin())
            {
                ImGui::Spring(0, _padding * 2.0f);
                ImGui::Dummy(ImVec2(0, _padding));

                auto pin = GetInputPin();
                ImGui::Spring(1, 0);
                _inputRect = ImGuiHelper::GetItemRect();

                NodeGraph::NodeEditorStyleVarBuilder styleVar;
                styleVar.PushStyleVar(ed::StyleVar_PinArrowSize, 10.0f);
                styleVar.PushStyleVar(ed::StyleVar_PinArrowWidth, 10.0f);
                styleVar.PushStyleVar(ed::StyleVar_PinCorners, ImDrawFlags_RoundCornersBottom);

                ed::BeginPin(pin->GetPinID(), ed::PinKind::Input);
                ed::PinPivotRect(_inputRect.GetTL(), _inputRect.GetBR());
                ed::PinRect(_inputRect.GetTL(), _inputRect.GetBR());
                ed::EndPin();
            }
            else
            {
                ImGui::BeginVertical("padding");
                ImGui::Spring(1, _padding * 0.5f);
                ImGui::EndVertical();
            }
            ImGui::Spring(0, _padding * 2.0f);
        }
        ImGui::EndHorizontal();
    }

    void BTNode::CalculateOutputPinRect()
    {
        ImGui::BeginHorizontal("output");
        {
            ImGui::Spring(0, _padding * 2.0f);
            ImGui::Dummy(ImVec2(0, _padding));
            if (true == HasOutputPin())
            {
                auto pin = GetOutputPin();
                ImGui::Spring(1, 0);
                _outputRect = ImGuiHelper::GetItemRect();

                NodeGraph::NodeEditorStyleVarBuilder styleVar;
                styleVar.PushStyleVar(ed::StyleVar_PinCorners, ImDrawFlags_RoundCornersTop);

                ed::BeginPin(pin->GetPinID(), ed::PinKind::Output);
                ed::PinPivotRect(_outputRect.GetTL(), _outputRect.GetBR());
                ed::PinRect(_outputRect.GetTL(), _outputRect.GetBR());
                ed::EndPin();
            }
            else
            {
                ImGui::BeginVertical("padding");
                ImGui::Spring(1, _padding * 0.5f);
                ImGui::EndVertical();
            }
            ImGui::Spring(0, _padding * 2.0f);
        }
        ImGui::EndHorizontal();
    }

    void BTNode::DrawRect()
    {
        auto drawList = ed::GetNodeBackgroundDrawList(GetNodeID());
        if (true == HasInputPin())
        {
            ImVec2      tl = _inputRect.GetTL(), br = _inputRect.GetBR();
            ImDrawFlags flag = ImDrawFlags_RoundCornersBottom;
            drawList->AddRectFilled(tl, br, _inputRectColor, _pinRectRounding, flag);
            drawList->AddRect(tl, br, _inputRectColor, _pinRectRounding, flag);
        }
        if (true == HasOutputPin())
        {
            ImVec2      tl = _outputRect.GetTL(), br = _outputRect.GetBR();
            ImDrawFlags flag = ImDrawFlags_RoundCornersTop;
            drawList->AddRectFilled(tl, br, _outputRectColor, _pinRectRounding, flag);
            drawList->AddRect(tl, br, _outputRectColor, _pinRectRounding, flag);
        }
        // Draw label background
        {
            ImVec2 tl = _labelRect.GetTL(), br = _labelRect.GetBR();
            drawList->AddRectFilled(_labelRect.GetTL(), _labelRect.GetBR(), _labelRectColor, _labelRectRounding);
            drawList->AddRect(_labelRect.GetTL(), _labelRect.GetBR(), _labelRectColor, _labelRectRounding);
        }
    }

    void BTNode::DrawLabel()
    {
        ImVec2 textSize = ImGui::CalcTextSize(_label.c_str());

        ImGui::BeginHorizontal("content_frame");
        ImGui::Spring(1, _padding);

        ImGui::BeginVertical("content", ImVec2(0.0f, 0.0f));
        ImGui::Dummy(ImVec2(textSize.x + 10.0f, 0));
        ImGui::Spring(1);
        ImGui::TextUnformatted(_label.c_str());
        ImGui::Spring(1);
        ImGui::EndVertical();
        _labelRect = ImGuiHelper::GetItemRect();

        ImGui::Spring(1, _padding);
        ImGui::EndHorizontal();
    }
} // namespace BehaviorTree