#include "pch.h"
#include "Quad.h"
#include "VIBuffer.h"
#include "VertexShader.h"

void Quad::Initialize()
{
	_pVIBuffer = new VIBuffer;

	std::vector<std::pair<Vector4, Vector2>> vertices(4);
	unsigned int indices[6];

	vertices[0] = { { -1.0f, -1.0f, 0.0f, 1.f }, { 0.0f, 1.0f } };
	vertices[1] = { { -1.0f,  1.0f, 0.0f, 1.f }, { 0.0f, 0.0f } };
	vertices[2] = { {  1.0f,  1.0f, 0.0f, 1.f }, { 1.0f, 0.0f } };
	vertices[3] = { {  1.0f, -1.0f, 0.0f, 1.f }, { 1.0f, 1.0f } };

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
	_vsQuad = g_pResourceMgr->LoadResource<VertexShader>(L"Assets/Shaders/QuadVS.cso");
}

void Quad::Render()
{
	_vsQuad->SetVertexShader();
	_pVIBuffer->SetParameters(_pDeviceContext);
	_pVIBuffer->DrawIndexed(_pDeviceContext);
}

void Quad::Free()
{
	SafeRelease(_pVIBuffer);
}
