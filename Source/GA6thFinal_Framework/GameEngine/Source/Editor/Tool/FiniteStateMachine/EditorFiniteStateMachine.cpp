#include "pch.h"
#include "EditorFiniteStateMachine.h"
#include "Engine/EditorCore/Gui/NodeGraph/Nodes/Custom/NodeGraphHoudiniNode.h"

EditorFiniteStateMachine::EditorFiniteStateMachine()
{
    SetLabel("PlayerFSM");
    SetDockLayout(ImGuiDir_Down);
}

EditorFiniteStateMachine::~EditorFiniteStateMachine() {}

void EditorFiniteStateMachine::OnTickGui() {}

void EditorFiniteStateMachine::OnStartGui() 
{
    _nodeGraph = new EditorNodeGraph();
    _nodeGraph->AddNode<NodeGraph::HoudiniNode>("Houdini1")->SetPosition(ImVec2(255,255));
    _nodeGraph->AddNode<NodeGraph::HoudiniNode>("Houdini2")->SetPosition(ImVec2(-255, -255));
}

void EditorFiniteStateMachine::OnEndGui() 
{
    if (nullptr != _nodeGraph)
    {
        delete _nodeGraph;
        _nodeGraph = nullptr;
    }
    
}

void EditorFiniteStateMachine::OnPreFrameBegin() {}

void EditorFiniteStateMachine::OnPostFrameBegin() {}

void EditorFiniteStateMachine::OnFrameRender() 
{
    _nodeGraph->Render();
}

void EditorFiniteStateMachine::OnFrameEnd() {}
