#pragma once
#include "../NodeGraphNode.h"

namespace NodeGraph
{
    class TreeNode : public Node
    {
    public:
        using Node::Node;

    public:
        virtual void Draw() override;

    private:
        ImVec4 _nodeBgColor = ImColor(229, 229, 229, 200);
        ImVec4 _nodeRectColor = ImColor(125, 125, 125, 200);
        ImVec4 _pinInputColor = ImColor(229, 229, 229, 60);
    };
} // namespace NodeGraph