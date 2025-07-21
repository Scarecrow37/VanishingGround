#pragma once

class Box;
class SkyBox
{
    enum { CUBE_MAP, IRRADIANCE_MAP, PREFILTERED_MAP, BRDF_LUT, END };

public:
    SkyBox();
    ~SkyBox();

public:
    void Initialize();
    void SetTexture(std::wstring_view path);
    void Render(ID3D12GraphicsCommandList* commnadList,UINT rootParameterIndex);
    void ResetResource();
    const D3D12_GPU_DESCRIPTOR_HANDLE GetCubeMapSRV() const { return _cubeMap->GetSRVHandle(); }
    bool HasTexture() { return _hasTexture; }

private:
    ComPtr<ID3D12Resource> CreateTexture2D(ID3D12Device* device, int w, int h, DXGI_FORMAT format);
    ComPtr<ID3D12Resource> CreateCubeMap(ID3D12Device* device, UINT size, DXGI_FORMAT format);
    void UploadToTexture2D(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ID3D12Resource* texture, const void* data, size_t dataSize);
    void CreateHDRSRV(ID3D12Resource* resource);
    void CreatePipelineState();

private:
    DescriptorHandles                   _hdrSRVHandles;
    ComPtr<ID3D12Resource>              _skyboxhdrTexture;
    std::unique_ptr<Box>                 _box;

    std::unique_ptr<UnorderedAccessView> _cubeMap;
    std::unique_ptr<UnorderedAccessView> _irradianceMap;
    std::unique_ptr<UnorderedAccessView> _prefilteredMap;
    std::unique_ptr<UnorderedAccessView> _brdfLUT;

    std::unique_ptr<ShaderBuilder>      _shader[END];
    ComPtr<ID3D12PipelineState>         _pipelineState[END];

    bool _hasTexture;
};
