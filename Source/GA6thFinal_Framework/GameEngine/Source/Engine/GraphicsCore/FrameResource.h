#pragma once

class FrameResource
{
public:
    FrameResource();
    ~FrameResource();

public:
    void SetFrameResource(UINT index, UINT rootParametorIndex, ID3D12GraphicsCommandList* commandList);
    void AddFrameResource(UINT stride, UINT numObject);

public:
    void CopyStructuredBuffer(ID3D12GraphicsCommandList* commandList, UINT index, void* data, UINT size);

private:
    std::vector<std::unique_ptr<StructuredBuffer>> _structuredBuffers;
    UINT64                                         _fenceValue{0};
    UINT                                           _handleSize{0};
};