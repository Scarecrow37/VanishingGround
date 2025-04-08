#include "pch.h"
#include "SkeletalMeshRenderer.h"
#include "Animator.h"
#include "SkeletalMesh.h"
#include "Model.h"
#include "VertexShader.h"

void SkeletalMeshRenderer::Initialize(const wchar_t* filePath)
{
	_model = g_pResourceMgr->LoadResource<SkeletalMesh>(filePath);

	_pAnimator = new Animator;
	_pAnimator->Initialize(filePath, _model->GetSkeleton());

	_vertexShader = g_pResourceMgr->LoadResource<VertexShader>(L"Assets/Shaders/MeshVS_Skeletal.cso");
	_type = MeshType::Skeletal;

	_model->BindVertexShader(_vertexShader);
}