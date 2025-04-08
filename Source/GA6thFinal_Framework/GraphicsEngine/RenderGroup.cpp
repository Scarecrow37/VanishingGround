#include "pch.h"
#include "RenderGroup.h"
#include "MeshRenderer.h"

void RenderGroup::RegisterRenderQueue(GE::IMeshRenderer* pMeshRenderer, Matrix* pMatrix)
{
	auto compare = [pMeshRenderer](const auto& pComponent)
		{ return static_cast<MeshRenderer*>(pMeshRenderer)->GetID() == pComponent.first->GetID();};

	if (std::ranges::any_of(_datas, compare))
	{
		ASSERT(false, L"Already registered MeshRenderer.");
		return;
	}

	_datas.emplace_back(static_cast<MeshRenderer*>(pMeshRenderer), pMatrix);
}

void RenderGroup::UnRegisterRenderQueue(GE::IMeshRenderer* pMeshRenderer)
{
	std::erase_if(_datas, [pMeshRenderer](const auto& datas)
		{ return static_cast<MeshRenderer*>(pMeshRenderer)->GetID() == datas.first->GetID(); });
}

void RenderGroup::Free()
{
}