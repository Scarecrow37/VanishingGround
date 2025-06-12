#pragma once

namespace NodeGraph
{
    class BehaviorTreeNode : public Node
    {
    public:
        using Node::Node;
        virtual ~BehaviorTreeNode();

    private:
        // Node을(를) 통해 상속됨
        void Draw() override;

        void OnCreate() override;
        void OnNodePopup() override;
        void OnPinPopup(Pin* pin) override;

    protected:
        //virtual void OnPostBeginNode();

    private:
        void DrawNode();
        void CalculateInputPinRect();
        void CalculateOutputPinRect();
        void DrawRect();
        void DrawLabel();

    private:
        inline static float _nodeRounding = 4.0f;
        inline static float _pinRectRounding  = 4.0f;
        inline static float _labelRectRounding = 0.0f;
        inline static float _padding = 10.0f;

        ImRect              _inputRect;
        ImRect              _outputRect;
        ImRect              _labelRect;

        inline static ImU32 _inputRectColor  = IM_COL32(45, 45, 45, 255);
        inline static ImU32 _outputRectColor = IM_COL32(45, 45, 45, 255);
        inline static ImU32 _labelRectColor  = IM_COL32(167, 65, 200, 255);
    };
}
