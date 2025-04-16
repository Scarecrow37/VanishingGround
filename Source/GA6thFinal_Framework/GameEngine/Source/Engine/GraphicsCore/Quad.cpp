#include "pch.h"
#include "Quad.h"
#include "VIBuffer.h"
#include "GeometryGenerator.h"

Quad::Quad()
{
}

Quad::~Quad()
{
}

void Quad::Initialize(float x, float y, float w, float h, float depth)
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData meshData = geoGen.CreateQuad(x, y, w, h, depth);
	VIBuffer::Descriptor descriptor
	{
		.vertexData = static_cast<void*>(meshData.vertices.data()),
		.indexData = static_cast<void*>(meshData.indices32.data()),
		.vertexSize = static_cast<UINT>(sizeof(Vertex) * meshData.vertices.size()),
		.vertexStride = sizeof(Vertex),
		.indexSize = static_cast<UINT>(sizeof(UINT) * meshData.indices32.size()),
		.indexCount = static_cast<UINT>(meshData.indices32.size()),
	};

	BaseMesh::Initialize(descriptor);
}