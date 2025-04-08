#include "pch.h"
#include "SpriteRenderer.h"
#include "Texture.h"

#include "CameraSystem.h"
#include "Camera.h"

void SpriteRenderer::Initialize(const wchar_t* filePath)
{
	_texture = g_pResourceMgr->LoadResource<Texture>(filePath);
}

void SpriteRenderer::Render(ID3D11DeviceContext* pDeviceContext)
{
	if (!_description.activeDraw) return;

	Camera* pCamera = g_pCameraSystem->GetCurrentCamera();
	if (nullptr == pCamera)
		return;

	CameraData data
	{
		.view = pCamera->GetViewMatrix().Transpose(),
		.projection = g_orthoGraphic,
		.cameraPosition = pCamera->GetPosition()
	};

	if (GE::SpriteDescription::D2D != _description.type)
	{
		data.projection = pCamera->GetProjectionMatrix().Transpose();
	}

	g_pConstantBuffer->UpdateConstantBuffer(L"CameraData", &data);
	g_pConstantBuffer->UpdateConstantBuffer(L"SpriteAlpha", &_description.alpha);

	pDeviceContext->PSSetShaderResources(0, 1, _texture->Get());
	g_pSprite->Render(_description.type);
}

void SpriteRenderer::Release()
{
	delete this;
}

void SpriteRenderer::GetDesc(GE::SpriteDescription* out)
{
	(*out) = _description;
}

void SpriteRenderer::SetDesc(const GE::SpriteDescription* in)
{
	_description = (*in);
}

void SpriteRenderer::GetImageSize(float* outWidth, float* outHeight)
{
	auto [width, height] = _texture->GetSize();
	(*outWidth) = width;
	(*outHeight) = height;
}