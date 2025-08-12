#pragma once

class Box;
class SkyBox
{
    enum { CUBE_MAP, IRRADIANCE_MAP, PREFILTERED_MAP, BRDF_LUT, END };

public:
    SkyBox();
    ~SkyBox();

public:
    const D3D12_GPU_DESCRIPTOR_HANDLE GetCubeMapSRV() const { return _envCubeMap->GetSRVHandle(); }
    const D3D12_GPU_DESCRIPTOR_HANDLE GetIrradianceMapSRV() const { return _irradianceMap->GetSRVHandle(); }
    const D3D12_GPU_DESCRIPTOR_HANDLE GetPrefilteredMapSRV() const { return _prefilteredMap->GetSRVHandle(); }
    const D3D12_GPU_DESCRIPTOR_HANDLE GetBrdfLUTSRV() const { return _brdfLUT->GetSRVHandle(); }
    bool HasEnvTexture() { return _hasEnvTexture; }
    bool HasIBLTexture() { return _hasIBLTexture; }

public:
    void SetEnvironmentTexture(std::wstring_view path);
    void SetIBLTexture(std::wstring_view path);

public:
    void Initialize();
    void Render(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex);
    void ResetEnvironmentResource();
    void ResetIBLResource();

private:
    ComPtr<ID3D12Resource> CreateTexture2D(ID3D12Device* device, int w, int h, DXGI_FORMAT format);
    ComPtr<ID3D12Resource> CreateCubeMap(ID3D12Device* device, UINT size, DXGI_FORMAT format);
    void UploadToTexture2D(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ID3D12Resource* texture, const void* data, size_t dataSize);
    void CreateHDRSRV(ID3D12Resource* resource);
    void CreatePipelineState();

private:
    DescriptorHandles      _hdrSRVHandles;
    ComPtr<ID3D12Resource> _skyboxhdrTexture;
    std::unique_ptr<Box>   _box;

    std::unique_ptr<UnorderedAccessView> _envCubeMap;
    std::unique_ptr<UnorderedAccessView> _iblCubeMap;
    std::unique_ptr<UnorderedAccessView> _irradianceMap;
    std::unique_ptr<UnorderedAccessView> _prefilteredMap;
    std::unique_ptr<UnorderedAccessView> _brdfLUT;

    std::unique_ptr<ShaderBuilder>      _shader[END];
    ComPtr<ID3D12PipelineState>         _pipelineState[END];

    bool _hasEnvTexture;
    bool _hasIBLTexture;
};
