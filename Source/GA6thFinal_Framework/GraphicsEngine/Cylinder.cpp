#include "pch.h"
#include "Cylinder.h"
#include "VIBuffer.h"
#include "GeometryGenerator.h"

Cylinder::Cylinder()
{
}

Cylinder::~Cylinder()
{
}

void Cylinder::Initialize(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount)
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData meshData = geoGen.CreateCylinder(bottomRadius, topRadius, height, sliceCount, stackCount);

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