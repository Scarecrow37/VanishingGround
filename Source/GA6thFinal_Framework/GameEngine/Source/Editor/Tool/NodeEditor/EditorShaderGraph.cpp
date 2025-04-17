#include "EditorShaderGraph.h"

using namespace Global;

EditorShaderGraph::EditorShaderGraph()
{
    using namespace u8_literals;
    this->SetLabel(u8"노드 에디터"_c_str);
}

EditorShaderGraph::~EditorShaderGraph()
{
}

void EditorShaderGraph::OnStartGui()
{
    _bluePrint.OnStart();
}

void EditorShaderGraph::OnPreFrame()
{
}

void EditorShaderGraph::OnFrame()
{
    _bluePrint.OnFrame(engineCore->Time.deltaTime());
}

void EditorShaderGraph::OnPostFrame()
{
}
