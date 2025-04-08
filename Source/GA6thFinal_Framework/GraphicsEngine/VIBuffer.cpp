#include "pch.h"
#include "VIBuffer.h"

void VIBuffer::SetParameters(ID3D11DeviceContext* pDeviceContext)
{
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContext->IASetVertexBuffers(0, 1, &_pVertexBuffer, &_vertexBufferStride, &_vertexBufferOffset);
	pDeviceContext->IASetIndexBuffer(_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
}

void VIBuffer::DrawIndexed(ID3D11DeviceContext* pDeviceContext)
{
	pDeviceContext->DrawIndexed(_indexCount, 0, 0);
}

HRESULT VIBuffer::Initialize(const VIBufferInfo& info)
{
	ID3D11Device* pDevice = g_pGraphicDevice->GetDevice();

	// VertexBuffer
	D3D11_BUFFER_DESC vbDesc
	{
		.ByteWidth = info.vertexByteWidth,
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
	};		

	D3D11_SUBRESOURCE_DATA vbData
	{
		.pSysMem = info.vertices,
	};

	FAILED_CHECK_BREAK(pDevice->CreateBuffer(&vbDesc, &vbData, &_pVertexBuffer));

	// IndexBuffer
	D3D11_BUFFER_DESC ibDesc
	{
		.ByteWidth = info.indexByteWidth,
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_INDEX_BUFFER,
	};

	D3D11_SUBRESOURCE_DATA ibData
	{
		.pSysMem = info.indices,
	};

	FAILED_CHECK_BREAK(pDevice->CreateBuffer(&ibDesc, &ibData, &_pIndexBuffer));

	_vertexBufferOffset = info.vertexBufferOffset;
	_vertexBufferStride = info.vertexBufferStride;
	_indexCount = info.indexCount;

	return S_OK;
}

void VIBuffer::Free()
{
	SafeRelease(_pIndexBuffer);
	SafeRelease(_pVertexBuffer);
}
