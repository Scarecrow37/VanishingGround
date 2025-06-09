#pragma once
#include "../NodeGraphNode.h"

namespace NodeGraph
{
    class HoudiniNode : public Node
    {
    public:
        HoudiniNode(EditorNodeGraph* owner, const char* name);
        virtual ~HoudiniNode();

    public:
        virtual void Draw() override;

    private:
        void DrawNode();
        void DrawInputPins();
        void DrawOutputPins();
        void DrawLabel();
        void DrawPin(Pin* pin, ImVec4 color, ImRect rect, int roundFlag);

    private:
        inline static float _nodeRounding = 10.0f; // Rounding for node corners
        inline static float _pinPadding   = 12.0f; // Padding around pins

    };
} // namespace NodeGraph