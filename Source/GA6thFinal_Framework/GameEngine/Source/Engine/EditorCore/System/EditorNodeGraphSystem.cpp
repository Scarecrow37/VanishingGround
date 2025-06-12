#include "pch.h"
#include "EditorNodeGraphSystem.h"

void EditorNodeGraphSystem::RegisterNodeCreator() 
{

}

bool EditorNodeGraphSystem::CreateNodeGraphContext(NodeGraphContext** ptr)
{
    if (nullptr == ptr)
        return false;
    *ptr = new NodeGraphContext();
    if (nullptr == *ptr)
    {
        return false;
    }
    (*ptr)->SetCurrentContext();
}

void EditorNodeGraphSystem::DestroyNodeGraphContext(NodeGraphContext** context) 
{
    if (nullptr == context || nullptr == *context)
        return;
    delete *context;
    *context = nullptr;
}

void EditorNodeGraphSystem::SetCurrentContext(NodeGraphContext* context) 
{
    if (nullptr == context)
        return;
    context->SetCurrentContext();
    _previousContext = _currentContext;
    _currentContext = context;
}

NodeGraphContext* EditorNodeGraphSystem::GetCurrentContext() const
{
    return _currentContext;
}

void EditorNodeGraphSystem::SetCurrentNode(NodeGraph::Node* node) 
{
    if (nullptr == node)
        return;
    _currentNode = node;
}

NodeGraph::Node* EditorNodeGraphSystem::GetCurrentNode() const
{
    return _currentNode;
}

namespace NodeGraph
{
    UINT NodeGraph::GetUniqueID()
    {
        auto& system  = Global::editorModule->GetNodeGraphSystem();
        auto  context = system.GetCurrentContext();
        return context->GetUniqueID();
    }

    void NodeGraph::SetCurrentNodeGraphContext(NodeGraphContext* context)
    {
        auto& system = Global::editorModule->GetNodeGraphSystem();
        system.SetCurrentContext(context);
    }
    void NodeGraph::SetCurrentNode(NodeGraph::Node* node) 
    {
        auto& system = Global::editorModule->GetNodeGraphSystem();
        system.SetCurrentNode(node);
    }

    NodeGraphContext* NodeGraph::GetCurrentNodeGraphContext()
    {
        auto& system = Global::editorModule->GetNodeGraphSystem();
        return system.GetCurrentContext();
    }
    NodeGraph::Node* NodeGraph::GetCurrentNode()
    {
        auto& system = Global::editorModule->GetNodeGraphSystem();
        return system.GetCurrentNode();
    }
} // namespace NodeGraph