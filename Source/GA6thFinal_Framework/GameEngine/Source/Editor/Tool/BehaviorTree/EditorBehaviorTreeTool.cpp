#include "pch.h"
#include "EditorBehaviorTreeTool.h"
#include "Engine/EditorCore/Gui/NodeGraph/Nodes/Custom/NodeGraphHoudiniNode.h"
#include "Nodes/BehaviorTreeNode.h"

EditorBehaviorTreeTool::EditorBehaviorTreeTool() 
{
    SetLabel("BehaviorTree");
    SetDockLayout(ImGuiDir_Down);
}

EditorBehaviorTreeTool::~EditorBehaviorTreeTool() {}

void EditorBehaviorTreeTool::OnTickGui() {}

void EditorBehaviorTreeTool::OnStartGui() 
{
    _context = new NodeGraphContext();
    NodeGraph::SetCurrentNodeGraphContext(_context);
    {
        auto node = _context->AddNode<NodeGraph::BehaviorTreeNode>("root");
        node->AddPin("Output", "Out", ed::PinKind::Output);
        node->SetPosition(ImVec2(0, 0));
    }
    {
        auto node = _context->AddNode<NodeGraph::BehaviorTreeNode>("node1");
        node->AddPin("Input", "In", ed::PinKind::Input);
        node->AddPin("Output", "Out", ed::PinKind::Output);
        node->SetPosition(ImVec2(-100, -100));
    }
    {
        auto node = _context->AddNode<NodeGraph::BehaviorTreeNode>("node2");
        node->AddPin("Input", "In", ed::PinKind::Input);
        node->AddPin("Output", "Out", ed::PinKind::Output);
        node->SetPosition(ImVec2(100, 100));
    }
}

void EditorBehaviorTreeTool::OnEndGui() {}

void EditorBehaviorTreeTool::OnPreFrameBegin() {}

void EditorBehaviorTreeTool::OnPostFrameBegin() {}

void EditorBehaviorTreeTool::OnFrameRender() 
{
    NodeGraph::SetCurrentNodeGraphContext(_context);
    _context->Render();
}

void EditorBehaviorTreeTool::OnFrameEnd() {}
