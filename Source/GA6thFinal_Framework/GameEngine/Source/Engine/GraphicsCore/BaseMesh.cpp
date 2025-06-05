#include "pch.h"
#include "BaseMesh.h"

BaseMesh::BaseMesh()
	: _viBuffer(std::make_unique<VIBuffer>())
    , _vertices(nullptr)
    , _vertexStride(0)
    , _vertexSize(0)
{
}

BaseMesh::~BaseMesh()
{
    if (_vertices) delete[] _vertices;
}

void BaseMesh::GetVertexInfo(char*& vertices, unsigned int& vertexStirde, unsigned int& vertexSize)
{
    vertices     = _vertices;
    vertexStirde = _vertexStride;
    vertexSize   = _vertexSize;
}

void BaseMesh::Initialize(const VIBuffer::Descriptor& descriptor, bool createVertexInfo)
{
    if (createVertexInfo)
    {
        _vertexStride = descriptor.vertexStride;
        _vertexSize   = descriptor.vertexSize;
        _vertices     = new char[_vertexSize];
        memcpy(_vertices, descriptor.vertexData, _vertexSize);
    }

	_viBuffer->Initialize(descriptor);
}

void BaseMesh::Render(ID3D12GraphicsCommandList* commandList)
{
	_viBuffer->DrawIndexedInstanced(commandList);
}