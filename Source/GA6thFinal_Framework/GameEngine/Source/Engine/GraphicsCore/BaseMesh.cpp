#include "pch.h"
#include "BaseMesh.h"

BaseMesh::BaseMesh()
	: _viBuffer(std::make_unique<VIBuffer>())
{
}

BaseMesh::~BaseMesh()
{
}

void BaseMesh::Initialize(const VIBuffer::Descriptor& descriptor)
{
	_viBuffer->Initialize(descriptor);
}

void BaseMesh::Render(ID3D12GraphicsCommandList* commandList)
{
	_viBuffer->DrawIndexedInstanced(commandList);
}