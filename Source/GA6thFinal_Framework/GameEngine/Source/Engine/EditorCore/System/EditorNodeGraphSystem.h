#pragma once
#include "Engine/EditorCore/Gui/NodeGraph/EditorNodeGraph.h"

namespace NodeGraph
{
    class Node;
    class Pin;
    class Link;

    UINT GetUniqueID();
    
    void SetCurrentNodeGraphContext(NodeGraphContext* context);
    void SetCurrentNode(NodeGraph::Node* node);

    NodeGraphContext* GetCurrentNodeGraphContext();
    NodeGraph::Node*  GetCurrentNode();
}

class EditorNodeGraphSystem
{
public:
    void RegisterNodeCreator();

    bool CreateNodeGraphContext(NodeGraphContext** ptr);
    void DestroyNodeGraphContext(NodeGraphContext** context);

    void SetCurrentContext(NodeGraphContext* context);
    NodeGraphContext* GetCurrentContext() const;

    void SetCurrentNode(NodeGraph::Node* node);
    NodeGraph::Node* GetCurrentNode() const;

private:
    NodeGraphContext* _currentContext = nullptr;
    NodeGraphContext* _previousContext = nullptr;

    NodeGraph::Node* _currentNode = nullptr;

    std::unordered_map<std::string, std::function<NodeGraph::Node*(EditorNodeGraph*, const char*)>> _nodeCreators;

};
