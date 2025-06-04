#include "pch.h"
#include "EditorShaderGraph.h"

using namespace Global;

EditorShaderGraph::EditorShaderGraph()
{
    SetLabel("NodeEditor");
    SetDockLayout(ImGuiDir_Down);
    _bluePrint = new blueprint();
}

EditorShaderGraph::~EditorShaderGraph()
{
}

void EditorShaderGraph::OnStartGui()
{
    _bluePrint->OnStart();
}

void EditorShaderGraph::OnEndGui()
{
    if (nullptr != _bluePrint)
    {
        _bluePrint->OnStop();
        delete _bluePrint;
        _bluePrint = nullptr;
    }
}

void EditorShaderGraph::OnFrameRender()
{
    _bluePrint->OnFrame(engineCore->Time.DeltaTime());
}

