#include "pch.h"
#include "Sphere.h"
#include "VIBuffer.h"
#include "GeometryGenerator.h"

Sphere::Sphere()
{
}

Sphere::~Sphere()
{
}

void Sphere::Initialize(float radius, uint32_t sliceCount, uint32_t stackCount)
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData meshData = geoGen.CreateSphere(radius, sliceCount, stackCount);

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