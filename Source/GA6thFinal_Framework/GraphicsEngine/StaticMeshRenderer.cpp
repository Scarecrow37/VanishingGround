#include "pch.h"
#include "StaticMeshRenderer.h"
#include "StaticMesh.h"
#include "VertexShader.h"

void StaticMeshRenderer::Initialize(const wchar_t* filePath)
{
	_model = g_pResourceMgr->LoadResource<StaticMesh>(filePath);

	_vertexShader = g_pResourceMgr->LoadResource<VertexShader>(L"Assets/Shaders/MeshVS.cso");
	_type = MeshType::Static;

	_model->BindVertexShader(_vertexShader);
}