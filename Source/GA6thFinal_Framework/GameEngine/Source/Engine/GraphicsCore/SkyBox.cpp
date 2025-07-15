#include "pch.h"
#include "SkyBox.h"
#include "Box.h"

SkyBox::SkyBox() : _box{std::make_unique<Box>()}, _hasTexture{false} {}

SkyBox::~SkyBox() {}

void SkyBox::Initialize()
{
    _box->InitializeInverted(1000.f, 1000.f, 1000.f, 0);
    HRESULT hr = S_OK;
    
    
    FAILED_CHECK_MESSAGE(hr, L"SkyBox::Initialize device->CreateDescriptorHeap Failed");
    CreateComputePSO();
}

void SkyBox::SetTexture(std::wstring_view path)
{
    // HDR/EXR 이미지 로드
    ScratchImage image;
    TexMetadata  metadata;

    HRESULT hr = LoadFromHDRFile(path.data(), &metadata, image);
    if (FAILED(hr))
    {
        _hasTexture = false;
        UmLogger.Log(LogLevel::LEVEL_ERROR, u8"HDR Texture 로드에 실패 하였습니다.");
        return;
    }
    FAILED_CHECK_MESSAGE(hr, L"SkyBox::SetTexture LoadFromHDRFile Failed");

    const Image* img = image.GetImage(0, 0, 0);

    size_t                     imageSize    = img->slicePitch;
    ID3D12Device*              pDevice      = UmDevice.GetDevice();
    ID3D12GraphicsCommandList* pCommandList = UmDevice.GetCommandList();

    // DirectXTex에서 가져온 포맷 사용 (보통 R32G32B32A32_FLOAT)
    _skyboxhdrTexture = CreateTexture2D(pDevice, static_cast<int>(metadata.width), static_cast<int>(metadata.height), metadata.format);

    UploadToTexture2D(pDevice, pCommandList, _skyboxhdrTexture.Get(), img->pixels, imageSize);

    // Create SRV
    CreateHDRSRV(_skyboxhdrTexture.Get());

    // Create CubeMap texture (UAV)
    const UINT cubeSize = 512;
    _skyboxCubeMap      = CreateCubeMap(pDevice, cubeSize, DXGI_FORMAT_R32G32B32A32_FLOAT);
    CreateUAV(_skyboxCubeMap.Get());
    CreateSRV(_skyboxCubeMap.Get());
    SetPipelineState();

    // Dispatch compute shader per face (0~5)
    for (UINT face = 0; face < 6; ++face)
    {
        BindResources(cubeSize, face);
        pCommandList->Dispatch((cubeSize + 15) / 16, (cubeSize + 15) / 16, 1);
    }

    _hasTexture = true;
}

void SkyBox::Render(ID3D12GraphicsCommandList* commnadList, UINT rootParameterIndex)
{
    commnadList->SetGraphicsRootDescriptorTable(rootParameterIndex, _cubeSRVHandles.GPU);
    _box->Render(commnadList);
}

void SkyBox::ResetResource() 
{
    _skyboxCubeMap.Reset();
    _skyboxhdrTexture.Reset();
    for (auto& it : _cbs)
    {
        it.Reset();
    }
    _hasTexture = false;
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
    HRESULT                hr = S_OK;
    hr = device->CreateCommittedResource(&property, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&texture));
    FAILED_CHECK_MESSAGE(hr, L"SkyBox::CreateTexture2D device->CreateCommittedResource Failed");

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
    HRESULT                hr = S_OK;
    hr = device->CreateCommittedResource(&property, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr, IID_PPV_ARGS(&texture));
    FAILED_CHECK_MESSAGE(hr, L"SkyBox::CreateCubeMap device->CreateCommittedResource Failed");

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

    ComPtr<ID3D12Resource> uploadResrouce;
   
    HRESULT hr = S_OK;
    hr         = device->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&uploadResrouce));
    FAILED_CHECK_MESSAGE(hr, L"SkyBox::UploadToTexture2D device->CreateCommittedResource Failed");

    void*         mappedData = nullptr;
    CD3DX12_RANGE readRange(0, 0);

    hr = uploadResrouce->Map(0, &readRange, &mappedData);
    FAILED_CHECK_MESSAGE(hr, L"SkyBox::UploadToTexture2D uploadResrouce->Map Failed");

    memcpy(mappedData, data, dataSize);
    uploadResrouce->Unmap(0, nullptr);

    D3D12_TEXTURE_COPY_LOCATION dst = {};
    dst.pResource                   = texture;
    dst.Type                        = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dst.SubresourceIndex            = 0;

    D3D12_TEXTURE_COPY_LOCATION src = {};
    src.pResource                   = uploadResrouce.Get();
    src.Type                        = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
    UINT                               numRows;
    UINT64                             rowSizeInBytes;
    UINT64                             totalBytes;
    device->GetCopyableFootprints(&texDesc, 0, 1, 0, &footprint, &numRows, &rowSizeInBytes, &totalBytes);
    src.PlacedFootprint = footprint;

    commandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(texture, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandList->ResourceBarrier(1, &barrier);

    UmDevice.UploadResource(uploadResrouce);
}

void SkyBox::CreateHDRSRV(ID3D12Resource* resource)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format                  = resource->GetDesc().Format;
    srvDesc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels     = 1;

    UmViewManager.AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _hdrSRVHandles);
    UmDevice.GetDevice() ->CreateShaderResourceView(resource, &srvDesc, _hdrSRVHandles.CPU);
}

void SkyBox::CreateSRV(ID3D12Resource* resource)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Shader4ComponentMapping    = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING; 
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
    srvDesc.TextureCube.MostDetailedMip = 0;
    srvDesc.TextureCube.MipLevels       = 1;
    
    UmViewManager.AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _cubeSRVHandles);
    UmDevice.GetDevice()->CreateShaderResourceView(resource, &srvDesc, _cubeSRVHandles.CPU);
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

    UmViewManager.AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _cubeUAVHandles);
    UmDevice.GetDevice()->CreateUnorderedAccessView(resource, nullptr, &uavDesc, _cubeUAVHandles.CPU);
}

void SkyBox::CreateComputePSO()
{
    _shader = std::make_unique<ShaderBuilder>();
    _shader->BeginBuild();
    _shader->SetShader(L"../Shaders/cs_cube_texture_convertor.hlsl",ShaderBuilder::Type::CS);
    _shader->EndBuild();

    ID3D12Device*               device = UmDevice.GetDevice();
    D3D12_COMPUTE_PIPELINE_STATE_DESC psodesc{};
    HRESULT                            hr = S_OK;
    psodesc.pRootSignature                = _shader->GetRootSignature();
    psodesc.CS                            = _shader->GetShaderByteCode(ShaderBuilder::Type::CS);
    psodesc.Flags                         = D3D12_PIPELINE_STATE_FLAG_NONE;

    hr = device->CreateComputePipelineState(&psodesc, IID_PPV_ARGS(_computePSO.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr, L"SkyBox::CreateComputePSO device->CreateComputePipelineState Failed");
}

void SkyBox::BindResources(UINT cubeSize, UINT faceIndex) 
{
    ID3D12GraphicsCommandList* cmdList = UmDevice.GetCommandList();

    struct CubeConvertConstants
    {
        UINT FaceIndex;
        UINT CubeSize;
        UINT Padding[2]; // 16바이트 정렬
    };

    CubeConvertConstants cb{};
    cb.FaceIndex = faceIndex;
    cb.CubeSize  = cubeSize;

    ComPtr<ID3D12Resource> _cb;
    UmDevice.CreateConstantBuffer(&cb, sizeof(CubeConvertConstants), _cb);

    cmdList->SetComputeRootSignature(_shader->GetRootSignature());
    cmdList->SetComputeRootDescriptorTable(_shader->GetRootParameterIndex("EquirectangularMap"), _hdrSRVHandles.GPU);
    cmdList->SetComputeRootDescriptorTable(_shader->GetRootParameterIndex("CubeMap"), _cubeUAVHandles.GPU);
    cmdList->SetComputeRootConstantBufferView(_shader->GetRootParameterIndex("CubeMapInfo"), _cb->GetGPUVirtualAddress());
    _cbs.push_back(_cb);
}

void SkyBox::SetPipelineState() 
{
    ID3D12GraphicsCommandList* cmdList = UmDevice.GetCommandList();
    cmdList->SetPipelineState(_computePSO.Get());
    cmdList->SetComputeRootSignature(_shader->GetRootSignature());
    
    auto descriptorHeap = UmViewManager.GetShaderResourceHeap();
    cmdList->SetDescriptorHeaps(1, &descriptorHeap);
}