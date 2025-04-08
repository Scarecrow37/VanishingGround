#include "pch.h"
#include "SpriteSystem.h"
#include "SpriteRenderer.h"
#include "PixelShader.h"

void SpriteSystem::CreateTextRenderer(GE::ISpriteRenderer** ppComponent, const wchar_t* filePath)
{
	SpriteRenderer* pComponent = new SpriteRenderer;
	pComponent->Initialize(filePath);
	(*ppComponent) = pComponent;
}

void SpriteSystem::RegisterRenderQueue(GE::ISpriteRenderer* pComponent, GE::Matrix4x4* pMatrix)
{
	auto compare = [pComponent](const auto component)
		{ return static_cast<SpriteRenderer*>(pComponent)->GetID() == component.first->GetID(); };

	if (std::ranges::any_of(_components, compare))
	{
		ASSERT(false, L"Already registered SpriteRenderer.");
		return;
	}

	_components.emplace_back(static_cast<SpriteRenderer*>(pComponent), static_cast<Matrix*>(pMatrix));
}

void SpriteSystem::UnRegisterRenderQueue(GE::ISpriteRenderer* pComponent)
{
	std::erase_if(_components, [pComponent](const auto& component)
		{ return static_cast<SpriteRenderer*>(pComponent)->GetID() == component.first->GetID(); });
}

void SpriteSystem::Initialize()
{
	_pDeviceContext = g_pGraphicDevice->GetDeviceContext();
	_psSprite = g_pResourceMgr->LoadResource<PixelShader>(L"Assets/Shaders/SpritePS.cso");
}

void SpriteSystem::Render()
{
	auto* blendState = g_pStateManagement->GetBlendState(L"AlphaBlend");
	_pDeviceContext->OMSetBlendState(blendState, nullptr, 0xFFFFFFFF);

	_psSprite->SetPixelShader();

	for (auto& [component, matrix] : _components)
	{
		Matrix transpose = matrix->Transpose();
		g_pConstantBuffer->UpdateConstantBuffer(L"World", &transpose);
		component->Render(_pDeviceContext);
	}

	_pDeviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
}

void SpriteSystem::Free()
{
}