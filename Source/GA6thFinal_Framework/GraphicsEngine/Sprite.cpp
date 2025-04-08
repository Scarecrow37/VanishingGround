#include "pch.h"
#include "Sprite.h"
#include "VIBuffer.h"
#include "VertexShader.h"

void Sprite::Initialize()
{
	_pVIBuffer = new VIBuffer;

	std::vector<std::pair<Vector4, Vector2>> vertices(4);
	unsigned int indices[6];

	vertices[0] = { { -0.5f, -0.5f, 0.0f, 1.f }, { 0.0f, 1.0f } };
	vertices[1] = { { -0.5f,  0.5f, 0.0f, 1.f }, { 0.0f, 0.0f } };
	vertices[2] = { {  0.5f,  0.5f, 0.0f, 1.f }, { 1.0f, 0.0f } };
	vertices[3] = { {  0.5f, -0.5f, 0.0f, 1.f }, { 1.0f, 1.0f } };

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;

	VIBuffer::VIBufferInfo info
	{
		.vertices = vertices.data(),
		.indices = indices,
		.vertexByteWidth = (unsigned int)vertices.size() * sizeof(std::pair<Vector4, Vector2>),
		.vertexBufferStride = sizeof(std::pair<Vector4, Vector2>),
		.vertexBufferOffset = 0,
		.indexByteWidth = sizeof(indices),
		.indexCount = 6
	};

	_pVIBuffer->Initialize(info);
	_pDeviceContext = g_pGraphicDevice->GetDeviceContext();
	_vsSprite[D2D] = g_pResourceMgr->LoadResource<VertexShader>(L"Assets/Shaders/SpriteVS.cso");
	_vsSprite[D3D] = g_pResourceMgr->LoadResource<VertexShader>(L"Assets/Shaders/SpriteVS_D3D.cso");
	_vsSprite[D3D_BillBoard] = g_pResourceMgr->LoadResource<VertexShader>(L"Assets/Shaders/SpriteVS_D3D_Billboard.cso");
}

void Sprite::Render(unsigned int type)
{
	_vsSprite[type]->SetVertexShader();
	_pVIBuffer->SetParameters(_pDeviceContext);
	_pVIBuffer->DrawIndexed(_pDeviceContext);
}

void Sprite::Free()
{
	SafeRelease(_pVIBuffer);
}