#include "pch.h"
#include "Box.h"
#include "VIBuffer.h"
#include "GeometryGenerator.h"

Box::Box()
{	
}

Box::~Box()
{
}

void Box::Initialize(float width, float height, float depth, uint32_t numSubdivisions)
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData meshData = geoGen.CreateBox(width, height, depth, numSubdivisions);

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

void Box::InitializeInverted(float width, float height, float depth, uint32_t numSubdivisions)
{
    GeometryGenerator           geoGen;
    GeometryGenerator::MeshData meshData = geoGen.CreateInvertedBox(width, height, depth, numSubdivisions);

    VIBuffer::Descriptor descriptor{
        .vertexData   = static_cast<void*>(meshData.vertices.data()),
        .indexData    = static_cast<void*>(meshData.indices32.data()),
        .vertexSize   = static_cast<UINT>(sizeof(Vertex) * meshData.vertices.size()),
        .vertexStride = sizeof(Vertex),
        .indexSize    = static_cast<UINT>(sizeof(UINT) * meshData.indices32.size()),
        .indexCount   = static_cast<UINT>(meshData.indices32.size()),
    };

    BaseMesh::Initialize(descriptor);
}
