#pragma once

class Box;
class SkyBox
{
public:
    SkyBox();
    ~SkyBox();

public:
    void Initialize();
    void SetTexture(std::wstring_view path);
    void Render(ID3D12GraphicsCommandList* commnadList,UINT rootParameterIndex);
    void ResetResource();
    D3D12_GPU_DESCRIPTOR_HANDLE GetCubeMapSRV() { return _cubeSRVHandles.GPU; }
    bool HasTexture() { return _hasTexture; }

private:
    ComPtr<ID3D12Resource> CreateTexture2D(ID3D12Device* device, int w, int h, DXGI_FORMAT format);
    ComPtr<ID3D12Resource> CreateCubeMap(ID3D12Device* device, UINT size, DXGI_FORMAT format);
    void UploadToTexture2D(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ID3D12Resource* texture,
                           const void* data, size_t dataSize);
    void CreateHDRSRV(ID3D12Resource* resource);
    void CreateSRV(ID3D12Resource* resource);
    void CreateUAV(ID3D12Resource* resource);
    void CreateComputePSO();
    void BindResources(UINT cubeSize, UINT faceIndex);
    void SetPipelineState();

private:
    std::unique_ptr<ShaderBuilder>      _shader;
    std::unique_ptr<Box>                _box;

    ComPtr<ID3D12Resource>              _skyboxCubeMap;
    ComPtr<ID3D12Resource>              _skyboxhdrTexture;
    DescriptorHandles                   _hdrSRVHandles;
    DescriptorHandles                   _cubeSRVHandles;
    DescriptorHandles                   _cubeUAVHandles;

    ComPtr<ID3D12PipelineState>         _computePSO;
    std::vector<ComPtr<ID3D12Resource>> _cbs;

    bool _hasTexture;
};
