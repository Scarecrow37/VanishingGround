#include "pch.h"
#include "EditorFiniteStateMachine.h"

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
