#pragma once

class Resource
{
public:
	Resource() = default;
	virtual ~Resource() = default;

public:
	const D3D12_CPU_DESCRIPTOR_HANDLE GetHandle() const { return _handle; }

public:
    virtual HRESULT LoadResource(const std::filesystem::path& filePath);

protected:
	ComPtr<ID3D12Resource> _resource;
	D3D12_CPU_DESCRIPTOR_HANDLE _handle;
};