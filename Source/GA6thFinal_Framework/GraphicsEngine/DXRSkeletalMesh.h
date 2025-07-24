#pragma once

class DXRSkeletalMesh
{
public:
    DXRSkeletalMesh() = default;
    ~DXRSkeletalMesh() = default;

public:
    class VIBuffer* _vibuffer = nullptr;
    ComPtr<ID3D12Resource> _updateVertexBuffer;
};
