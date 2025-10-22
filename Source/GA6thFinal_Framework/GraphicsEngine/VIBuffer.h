#pragma once

struct AccelerationStructureBuffers;
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
    void Initialize(ID3D12GraphicsCommandList* commandList, const VIBuffer::Descriptor& descriptor);
	void DrawIndexedInstanced(ID3D12GraphicsCommandList* commandList, UINT instanceCount = 1);
    void MakeAccelerationBuffer(ID3D12Device5* device, ID3D12GraphicsCommandList4* cmdList, AccelerationStructureBuffers* outBuffer);

public:
	D3D12_VERTEX_BUFFER_VIEW _vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW  _indexBufferView;
    DescriptorHandles        _vertexBufferSrv;
    DescriptorHandles        _indexBufferSrv;
	ComPtr<ID3D12Resource>   _vertexBuffer;
	ComPtr<ID3D12Resource>   _indexBuffer;
	UINT                     _indexCount;
    UINT                     _vertexCount;
    VertexBufferID           _vertexBufferID;
    IndexBufferID            _indexBufferID;
};