#pragma once

class VIBuffer
{
public:
	struct Descriptor
	{
		void* vertexData;
		void* indexData;
		UINT vertexSize;
		UINT vertexStride;
		UINT indexSize;
		UINT indexCount;
	};

public:
	void Initialize(const VIBuffer::Descriptor& descriptor);
	void DrawIndexedInstanced(ID3D12GraphicsCommandList* commandList, UINT instanceCount = 1);

private:
	D3D12_VERTEX_BUFFER_VIEW _vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW _indexBufferView;
	ComPtr<ID3D12Resource> _vertexBuffer;
	ComPtr<ID3D12Resource> _indexBuffer;
	UINT _indexCount;
};