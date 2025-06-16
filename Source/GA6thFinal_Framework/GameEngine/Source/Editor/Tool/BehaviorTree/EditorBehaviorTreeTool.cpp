#include "pch.h"
#include "EditorBehaviorTreeTool.h"
#include "Nodes/BTRootNode.h"

EditorBehaviorTreeTool::EditorBehaviorTreeTool() 
    : _context(nullptr)
{
    SetLabel("BehaviorTree");
    SetDockLayout(ImGuiDir_Down);
}

EditorBehaviorTreeTool::~EditorBehaviorTreeTool() {}

void EditorBehaviorTreeTool::OnTickGui() {}

void EditorBehaviorTreeTool::OnStartGui() 
{
    _context = new NodeGraphContext();
    auto filter = [](const NodeGraph::Pin* self, const NodeGraph::Pin* dest) -> bool {
        // 예시: 같은 종류의 핀은 연결할 수 없습니다.
        if (self->GetPinKind() == dest->GetPinKind())
        {
            return false;
        }
        return true;
    };
    NodeGraph::SetCurrentNodeGraphContext(_context);
    {
        auto node = _context->AddNode<BehaviorTree::RootNode>();
        node->SetPosition(ImVec2(0, 0));
    }
    {
        auto node = _context->AddNode<BehaviorTree::BTNode>();
        node->SetLabel("Child Node 1");
        node->AddPin("Input", ed::PinKind::Input, filter);
        node->AddPin("Output", ed::PinKind::Output, filter);
        node->SetPosition(ImVec2(-100, -100));
    }
    {
        auto node = _context->AddNode<BehaviorTree::BTNode>();
        node->SetLabel("Child Node 2");
        node->AddPin("Input", ed::PinKind::Input, filter);
        node->AddPin("Output", ed::PinKind::Output, filter);
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
