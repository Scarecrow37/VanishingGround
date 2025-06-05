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

    };
} // namespace NodeGraph