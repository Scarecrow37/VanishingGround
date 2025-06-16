#pragma once

namespace BehaviorTree
{
    class BTNode : public NodeGraph::Node
    {
    public:
        // Node을(를) 통해 상속됨
        void Draw() override;
        void OnCreate() override;
        void OnNodePopup() override;
        void OnPinPopup(UINT64 _pinID) override;
        void OnQueryNewLink(NodeGraph::Pin* from, NodeGraph::Pin* to, bool isReject) override;

    private:
        void DrawNode();
        void CalculateInputPinRect();
        void CalculateOutputPinRect();
        void DrawRect();
        void DrawLabel();

    public:
        inline void SetLabel(const char* label) { _label = label; }
        inline void SetNodeRectFillColor(const ImColor& color) { _nodeRectFillColor = color; }
        inline void SetNodeRectBorderColor(const ImColor& color) { _nodeRectBorderColor = color; }

    private:
        std::string _label;

        ImColor _nodeRectFillColor      = ImColor(128, 128, 128, 200);
        ImColor _nodeRectBorderColor    = ImColor(32, 32, 32, 0);

        inline static float _nodeRounding = 4.0f;
        inline static float _pinRectRounding  = 4.0f;
        inline static float _labelRectRounding = 0.0f;
        inline static float _padding = 10.0f;

        ImRect _inputRect;
        ImRect _outputRect;
        ImRect _labelRect;

        inline static ImU32 _inputRectColor  = IM_COL32(45, 45, 45, 255);
        inline static ImU32 _outputRectColor = IM_COL32(45, 45, 45, 255);
        inline static ImU32 _labelRectColor  = IM_COL32(167, 65, 200, 255);
    };
}
