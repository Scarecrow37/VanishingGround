#include "pch.h"
#include "VIBuffer.h"

void VIBuffer::Initialize(const VIBuffer::Descriptor& descriptor)
{
    UmDevice.CreateVertexBuffer(descriptor.vertexData, 
								descriptor.vertexSize, 
								descriptor.vertexStride, 
								_vertexBuffer,
								_vertexBufferView);

	UmDevice.CreateIndexBuffer(descriptor.indexData, 
							   descriptor.indexSize, 
							   DXGI_FORMAT_R32_UINT,
							   _indexBuffer,
							   _indexBufferView);

	_indexCount = descriptor.indexCount;
}

void VIBuffer::DrawIndexedInstanced(ID3D12GraphicsCommandList* commandList, UINT instanceCount)
{
	commandList->IASetVertexBuffers(0, 1, &_vertexBufferView);
	commandList->IASetIndexBuffer(&_indexBufferView);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->DrawIndexedInstanced(_indexCount, instanceCount, 0, 0, 0);
}
