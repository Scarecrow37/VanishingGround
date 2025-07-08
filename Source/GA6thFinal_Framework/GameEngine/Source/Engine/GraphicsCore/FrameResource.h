#pragma once

class StructuredBuffer;
class FrameResource
{
public:
    enum Type
    {
        TRANSFORM,
        BONE_MATRIXES,
        MATERIAL,
        STATIC_MESH_MATERIAL,
        SKELETAL_MESH_MATERIAL,
        VERTEX_BUFFER_ID,
        INDEX_BUFFER_ID,
        END
    };

public:
    FrameResource();
    ~FrameResource();

public:
    void SetFrameResource(Type type, UINT rootParametorIndex, ID3D12GraphicsCommandList* commandList);

public:
    void Initialize(const UINT numObjects);
    void CopyStructuredBuffer(ID3D12GraphicsCommandList* commandList, void* data, UINT size, FrameResource::Type type);
    // void CopyDescriptorsSimple(const D3D12_CPU_DESCRIPTOR_HANDLE handle, UINT destStartIndex, UINT numDescriptors);
    // void CopyDescriptors(const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& handles);

private:
    std::unique_ptr<StructuredBuffer> _structuredBuffer[END];
    UINT64                            _fenceValue{0};
    UINT                              _handleSize{0};
};