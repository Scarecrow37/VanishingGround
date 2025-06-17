#pragma once

class Resource
{
public:
    Resource()          = default;
    virtual ~Resource() = default;

public:
    const D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const { return _handle.CPU; }
    const D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const { return _handle.GPU; }
    const UINT                        GetID() const { return _ID; }

public:
    virtual void LoadResource(const std::filesystem::path& filePath) = 0;

protected:
    ComPtr<ID3D12Resource> _resource;
    DescriptorHandles      _handle;
    UINT                   _ID{0};
};