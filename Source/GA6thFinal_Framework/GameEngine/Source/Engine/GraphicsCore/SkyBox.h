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
    void SetTexture(std::string path);

private:
    ComPtr<ID3D12Resource> CreateTexture2D(ID3D12Device* device, int w, int h, DXGI_FORMAT format);
    ComPtr<ID3D12Resource> CreateCubeMap(ID3D12Device* device, UINT size, DXGI_FORMAT format);
    void UploadToTexture2D(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ID3D12Resource* texture,
                           const void* data, size_t dataSize);
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
    D3D12_CPU_DESCRIPTOR_HANDLE         _hdrSRVCPU;
    D3D12_GPU_DESCRIPTOR_HANDLE         _hdrSRVGPU;
    D3D12_CPU_DESCRIPTOR_HANDLE         _cubeUAVCPU;
    D3D12_GPU_DESCRIPTOR_HANDLE         _cubeUAVGPU;
    ComPtr<ID3D12PipelineState>         _computePSO;
    ComPtr<ID3D12DescriptorHeap>        _descriptorHeap;
    std::vector<ComPtr<ID3D12Resource>> _cbs;
    ComPtr<ID3D12Resource>              uploadBuffer;
};
