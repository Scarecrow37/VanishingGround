#include "SkyBox.h"
#include "Box.h"
#include "pch.h"
#include "ShaderBuilder.h"
#include <stb_image.h>

SkyBox::SkyBox():_box{std::make_unique<Box>()} {}

SkyBox::~SkyBox() {}

void SkyBox::Initialize()
{
    _box->Initialize(1000.f, 1000.f, 1000.f, 0);
}

void SkyBox::SetTexture(std::string_view path)
{
    int    width, height, channels;
    float* data = stbi_loadf(path.data(), &width, &height, &channels, 4);
    if (!data)
        throw std::runtime_error("Failed to load HDR image");

    size_t        imageSize = width * height * 4 * sizeof(float);
    ID3D12Device* pDevice   = UmDevice.GetDevice().Get();
    ID3D12GraphicsCommandList* pCommandList = UmDevice.GetCommandList().Get();
    ComPtr<ID3D12Resource> hdrTexture = CreateTexture2D(pDevice, width, height, DXGI_FORMAT_R32G32B32A32_FLOAT);
    UploadToTexture2D(pDevice, pCommandList, hdrTexture.Get(), data, imageSize);
    stbi_image_free(data);
    // Create texture srv
    CreateSRV(hdrTexture.Get());
    // Create CubeMap texture (UAV)
    const UINT             cubeSize = 512;
    ComPtr<ID3D12Resource> cubeMap  = CreateCubeMap(pDevice, cubeSize, DXGI_FORMAT_R16G16B16A16_FLOAT);
    CreateUAV(cubeMap.Get());

    // Dispatch compute shader per face (0~5)
    for (UINT face = 0; face < 6; ++face)
    {
        SetPipelineState();
        BindResources(cubeSize, face);
        pCommandList->Dispatch((cubeSize + 15) / 16, (cubeSize + 15) / 16, 1);
    }

    _skyboxCubeMap = cubeMap;
}


ComPtr<ID3D12Resource> SkyBox::CreateTexture2D(ID3D12Device* device, int w, int h, DXGI_FORMAT format)
{
    D3D12_RESOURCE_DESC desc         = {};
    desc.Dimension                   = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Width                       = w;
    desc.Height                      = h;
    desc.DepthOrArraySize            = 1;
    desc.MipLevels                   = 1;
    desc.Format                      = format;
    desc.SampleDesc.Count            = 1;
    desc.Layout                      = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    desc.Flags                       = D3D12_RESOURCE_FLAG_NONE;
    auto property = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    ComPtr<ID3D12Resource>  texture;
    FAILED_CHECK_BREAK(device->CreateCommittedResource(
        &property, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&texture)));

    return texture;
}

ComPtr<ID3D12Resource> SkyBox::CreateCubeMap(ID3D12Device* device, UINT size, DXGI_FORMAT format)
{
    D3D12_RESOURCE_DESC desc         = {};
    desc.Dimension                   = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Width                       = size;
    desc.Height                      = size;
    desc.DepthOrArraySize            = 6;
    desc.MipLevels                   = 1;
    desc.Format                      = format;
    desc.SampleDesc.Count            = 1;
    desc.Layout                      = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    desc.Flags                       = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    auto property                    = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    ComPtr<ID3D12Resource> texture;
    FAILED_CHECK_BREAK(device->CreateCommittedResource(&property, D3D12_HEAP_FLAG_NONE, &desc,
                                                       D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr,
                                                       IID_PPV_ARGS(&texture)));

    return texture;
}

void SkyBox::UploadToTexture2D(ID3D12Device* device, ID3D12GraphicsCommandList* commandList,
                               ID3D12Resource* texture, 
                               const void*     data,    
                               size_t          dataSize)
{
    D3D12_RESOURCE_DESC texDesc = texture->GetDesc();

    UINT64 uploadBufferSize = 0;
    device->GetCopyableFootprints(&texDesc, 0, 1, 0, nullptr, nullptr, nullptr, &uploadBufferSize);

    auto                  heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC   bufferDesc   = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

    ComPtr<ID3D12Resource> uploadBuffer;
    FAILED_CHECK_BREAK(device->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &bufferDesc,
                                                       D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                       IID_PPV_ARGS(&uploadBuffer)));
    void*         mappedData = nullptr;
    CD3DX12_RANGE readRange(0, 0);
    FAILED_CHECK_BREAK(uploadBuffer->Map(0, &readRange, &mappedData));
    memcpy(mappedData, data, dataSize);
    uploadBuffer->Unmap(0, nullptr);

    D3D12_TEXTURE_COPY_LOCATION dst = {};
    dst.pResource                   = texture;
    dst.Type                        = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dst.SubresourceIndex            = 0;

    D3D12_TEXTURE_COPY_LOCATION src = {};
    src.pResource                   = uploadBuffer.Get();
    src.Type                        = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
    UINT                               numRows;
    UINT64                             rowSizeInBytes;
    UINT64                             totalBytes;
    device->GetCopyableFootprints(&texDesc, 0, 1, 0, &footprint, &numRows, &rowSizeInBytes, &totalBytes);
    src.PlacedFootprint = footprint;

    CD3DX12_RESOURCE_BARRIER barrier =
        CD3DX12_RESOURCE_BARRIER::Transition(texture, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COPY_DEST);
    commandList->ResourceBarrier(1, &barrier);

    commandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

    barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        texture, D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE); 
    commandList->ResourceBarrier(1, &barrier);
}

void SkyBox::CreateSRV(ID3D12Resource* resource)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format                  = resource->GetDesc().Format;
    srvDesc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels     = 1;
    UmViewManager.AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _hdrSRV);
    UmDevice.GetDevice()->CreateShaderResourceView(resource, &srvDesc, _hdrSRV);
}

void SkyBox::CreateUAV(ID3D12Resource* resource)
{
    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
    uavDesc.Format        = resource->GetDesc().Format;
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
    uavDesc.Texture2DArray.MipSlice = 0;
    uavDesc.Texture2DArray.FirstArraySlice = 0;
    uavDesc.Texture2DArray.PlaneSlice      = 0;
    uavDesc.Texture2DArray.ArraySize       = 6;
    UmViewManager.AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _cubeUAV);
    UmDevice.GetDevice()->CreateUnorderedAccessView(resource, nullptr, &uavDesc, _cubeUAV);
}

void SkyBox::CreateComputePSO()
{
    _shader = std::make_unique<ShaderBuilder>();
    _shader->BeginBuild();
    _shader->SetShader(L"../Shaders/cs_cube_texture_convertor.hlsl",ShaderBuilder::Type::CS);
    _shader->EndBuild();

    ComPtr<ID3D12Device>               device = UmDevice.GetDevice();
    D3D12_COMPUTE_PIPELINE_STATE_DESC psodesc{};
    HRESULT                            hr = S_OK;
    psodesc.pRootSignature                = _shader->GetRootSignature().Get();
    psodesc.CS                            = _shader->GetShaderByteCode(ShaderBuilder::Type::CS);
    psodesc.Flags                         = D3D12_PIPELINE_STATE_FLAG_NONE;

    hr = device->CreateComputePipelineState(&psodesc, IID_PPV_ARGS(_computePSO.GetAddressOf()));
    FAILED_CHECK_BREAK(hr);
}

void SkyBox::BindResource() 
{

}

void SkyBox::SetPipelineStete(UINT cubeSize, UINT faceIndex) 
{

}

void SkyBox::Render()
{

}