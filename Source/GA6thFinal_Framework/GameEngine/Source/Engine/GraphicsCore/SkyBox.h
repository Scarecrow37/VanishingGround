#pragma once
class Box;
class ShaderBuilder;
class SkyBox
{
public:
    SkyBox();
    ~SkyBox();

public:
    void Initialize();
    void SetTexture(std::string_view path);
    void Render();

private:
    ComPtr<ID3D12Resource> CreateTexture2D(ID3D12Device* device, int w, int h, DXGI_FORMAT format);
    ComPtr<ID3D12Resource> CreateCubeMap(ID3D12Device* device, UINT size, DXGI_FORMAT format);
    void UploadToTexture2D(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ID3D12Resource* texture,
                           const void* data, size_t dataSize);
    void CreateSRV(ID3D12Resource* resource);
    void CreateUAV(ID3D12Resource* resource);
    void CreateComputePSO();
    void BindResource();
    void SetPipelineStete(UINT cubeSize, UINT faceIndex);

private:
    std::unique_ptr<ShaderBuilder> _shader;
    std::unique_ptr<Box>           _box;
    ComPtr<ID3D12Resource>         _skyboxCubeMap;
    D3D12_CPU_DESCRIPTOR_HANDLE    _hdrSRV;
    D3D12_CPU_DESCRIPTOR_HANDLE    _cubeUAV;
    ComPtr<ID3D12PipelineState>    _computePSO;
};
