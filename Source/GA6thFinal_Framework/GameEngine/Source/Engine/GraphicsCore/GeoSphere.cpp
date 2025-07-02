#include "pch.h"
#include "GeoSphere.h"
#include "VIBuffer.h"
#include "GeometryGenerator.h"

GeoSphere::GeoSphere()
{
}

GeoSphere::~GeoSphere()
{
}

void GeoSphere::Initialize(float radius, uint32_t numSubdivisions)
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData meshData
		= geoGen.CreateGeosphere(radius,numSubdivisions);

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