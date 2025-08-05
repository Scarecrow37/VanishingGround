#pragma once
class VIBuffer;
class UnorderedAccessView;
class DXRSkeletalMesh
{
public:
    DXRSkeletalMesh(VIBuffer* vibuffer);
    ~DXRSkeletalMesh() = default;

public:
    void Initialize(UINT vertexCount,UINT vertexStride);
    class VIBuffer* GetVIBuffer() const { return _vibuffer; }
    ID3D12Resource* GetUpdateVertexBuffer() const { return _skinnedVertexBuffer.Get(); }
    DescriptorHandles GetUAV() { return _skinnedVertexBufferUAV; }
    DescriptorHandles SetSRV() { return _skinnedVertexBufferSRV; }
    UINT              GetID() const { return _ID; } // DXR에서 사용하기 위한 Vertex Buffer ID
    uint64_t          GetInstanceID() const { return reinterpret_cast<uint64_t>(this); }

private:
    VIBuffer* _vibuffer = nullptr;
    ComPtr<ID3D12Resource> _skinnedVertexBuffer;
    DescriptorHandles      _skinnedVertexBufferUAV;
    DescriptorHandles      _skinnedVertexBufferSRV;
    UINT                   _ID = 0; // DXR에서 사용하기 위한 Vertex Buffer ID
};
