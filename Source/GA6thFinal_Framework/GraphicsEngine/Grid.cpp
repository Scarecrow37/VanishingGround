#include "pch.h"
#include "Grid.h"
#include "VIBuffer.h"
#include "GeometryGenerator.h"

Grid::Grid()
{
}

Grid::~Grid()
{
}

void Grid::Initialize(float width, float depth, uint32_t m, uint32_t n)
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData meshData = geoGen.CreateGrid(width,depth,m,n);

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