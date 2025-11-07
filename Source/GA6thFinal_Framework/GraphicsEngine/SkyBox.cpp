#include "pch.h"
#include "SkyBox.h"
#include "Box.h"

constexpr UINT CUBE_MAP_SIZE        = 512;
constexpr UINT IRRADIANCE_MAP_SIZE  = 128;
constexpr UINT PREFILTERED_MAP_SIZE = 256;
constexpr UINT BRDF_LUT_SIZE        = 512;

SkyBox::SkyBox()
    : _box{std::make_unique<Box>()}
    , _hasEnvTexture{false} 
    , _hasIBLTexture{false}
{}

SkyBox::~SkyBox() = default;

void SkyBox::SetEnvironmentTexture(std::wstring_view path)
{
    // HDR/EXR 이미지 로드
    ScratchImage image;
    TexMetadata  metadata;

    HRESULT hr = LoadFromHDRFile(path.data(), &metadata, image);
    if (FAILED(hr))
    {
        _hasEnvTexture = false;
        return;
    }
    FAILED_CHECK_MESSAGE(hr, L"SkyBox::SetEnvironmentTexture LoadFromHDRFile Failed");

    const Image* img = image.GetImage(0, 0, 0);

    size_t                     imageSize   = img->slicePitch;
    ID3D12Device*              device      = Global::device->GetDevice();
    ID3D12GraphicsCommandList* commandList = Global::device->GetCommandList();

    // DirectXTex에서 가져온 포맷 사용 (보통 R32G32B32A32_FLOAT)
    _skyboxhdrTexture[ENV] = CreateTexture2D(device, static_cast<int>(metadata.width), static_cast<int>(metadata.height), metadata.format);

    UploadToTexture2D(device, commandList, _skyboxhdrTexture[ENV].Get(), img->pixels, imageSize);

    CreateHDRSRV(_skyboxhdrTexture[ENV].Get(), _hdrSRVHandles[ENV]);

    // CubeMap 생성
    auto descriptorHeap = Global::viewManager->GetShaderResourceHeap();
    commandList->SetDescriptorHeaps(1, &descriptorHeap);

    commandList->SetPipelineState(_pipelineState[CUBE_MAP].Get());
    commandList->SetComputeRootSignature(_fxCubeMap.GetRootSignature());

    commandList->SetComputeRootDescriptorTable(_fxCubeMap.GetRootParameterIndex("equirectangularMap"), _hdrSRVHandles[ENV].GPU);
    commandList->SetComputeRootDescriptorTable(_fxCubeMap.GetRootParameterIndex("cubeMap"), _cubeMap[ENV]->GetUAVHandle());
    commandList->SetComputeRoot32BitConstants(_fxCubeMap.GetRootParameterIndex("bit32_1_cubeMapInfo"), 1, &CUBE_MAP_SIZE, 0);
    commandList->Dispatch((CUBE_MAP_SIZE + 15) / 16, (CUBE_MAP_SIZE + 15) / 16, 6);

    _cubeMap[ENV]->ResourceBarrier(commandList);
    _cubeMap[ENV]->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    _hasEnvTexture = true;
}

void SkyBox::SetIBLTexture(std::wstring_view path)
{
    // HDR/EXR 이미지 로드
    ScratchImage image;
    TexMetadata  metadata;

    HRESULT hr = LoadFromHDRFile(path.data(), &metadata, image);
    if (FAILED(hr))
    {
        _hasIBLTexture = false;
        return;
    }
    FAILED_CHECK_MESSAGE(hr, L"SkyBox::SetIBLTexture LoadFromHDRFile Failed");

    const Image* img = image.GetImage(0, 0, 0);

    size_t                     imageSize    = img->slicePitch;
    ID3D12Device*              pDevice      = Global::device->GetDevice();
    ID3D12GraphicsCommandList* commandList  = Global::device->GetCommandList();

    // DirectXTex에서 가져온 포맷 사용 (보통 R32G32B32A32_FLOAT)
    _skyboxhdrTexture[IBL] = CreateTexture2D(pDevice, static_cast<int>(metadata.width), static_cast<int>(metadata.height), metadata.format);

    UploadToTexture2D(pDevice, commandList, _skyboxhdrTexture[IBL].Get(), img->pixels, imageSize);

    CreateHDRSRV(_skyboxhdrTexture[IBL].Get(), _hdrSRVHandles[IBL]);

    // CubeMap 생성
    auto descriptorHeap = Global::viewManager->GetShaderResourceHeap();
    commandList->SetDescriptorHeaps(1, &descriptorHeap);

    commandList->SetPipelineState(_pipelineState[CUBE_MAP].Get());
    commandList->SetComputeRootSignature(_fxCubeMap.GetRootSignature());
    commandList->SetComputeRootDescriptorTable(_fxCubeMap.GetRootParameterIndex("equirectangularMap"), _hdrSRVHandles[IBL].GPU);
    commandList->SetComputeRootDescriptorTable(_fxCubeMap.GetRootParameterIndex("cubeMap"), _cubeMap[IBL]->GetUAVHandle());
    commandList->SetComputeRoot32BitConstants(_fxCubeMap.GetRootParameterIndex("bit32_1_cubeMapInfo"), 1, &CUBE_MAP_SIZE, 0);
    commandList->Dispatch((CUBE_MAP_SIZE + 15) / 16, (CUBE_MAP_SIZE + 15) / 16, 6);
    
    _cubeMap[IBL]->ResourceBarrier(commandList);
    _cubeMap[IBL]->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    // BRDF LUT 생성
    commandList->SetPipelineState(_pipelineState[BRDF_LUT].Get());
    commandList->SetComputeRootSignature(_fxBrdfLUT.GetRootSignature());
    commandList->SetComputeRootDescriptorTable(_fxBrdfLUT.GetRootParameterIndex("brdfLUT"), _brdfLUT->GetUAVHandle());

    // 타일 크기 설정 (예: 64x64 픽셀 단위로 작업을 나눔)
    UINT tileWidth  = 32;
    UINT tileHeight = 32;
    UINT groupSize = 8; // 셰이더의 [numthreads(8, 8, 1)] 에 맞춤

    for (UINT y = 0; y < BRDF_LUT_SIZE; y += tileHeight)
    {
        for (UINT x = 0; x < BRDF_LUT_SIZE; x += tileWidth)
        {
            // 타일 크기에 맞춰 디스패치
            UINT dispatchX = (tileWidth + groupSize - 1) / groupSize;
            UINT dispatchY = (tileHeight + groupSize - 1) / groupSize;
            
            UINT offset[2] = {x, y};
            commandList->SetComputeRoot32BitConstants(_fxBrdfLUT.GetRootParameterIndex("bit32_2_brdfConstants"), 2, offset, 0);
            commandList->Dispatch(dispatchX, dispatchY, 1);
        }
    }
    // commandList->Dispatch((BRDF_LUT_SIZE + 15) / 16, (BRDF_LUT_SIZE + 15) / 16, 1);

    // IrradianceMap 생성
    commandList->SetPipelineState(_pipelineState[IRRADIANCE_MAP].Get());
    commandList->SetComputeRootSignature(_fxIrradianceMap.GetRootSignature());
    commandList->SetComputeRootDescriptorTable(_fxIrradianceMap.GetRootParameterIndex("environmentMap"), _cubeMap[IBL]->GetSRVHandle());
    commandList->SetComputeRootDescriptorTable(_fxIrradianceMap.GetRootParameterIndex("irradianceMap"), _irradianceMap->GetUAVHandle());
    commandList->Dispatch((IRRADIANCE_MAP_SIZE + 15) / 16, (IRRADIANCE_MAP_SIZE + 15) / 16, 6);

    // PrefilteredMap 생성
    commandList->SetPipelineState(_pipelineState[PREFILTERED_MAP].Get());
    commandList->SetComputeRootSignature(_fxPrefilteredMap.GetRootSignature());
    commandList->SetComputeRootDescriptorTable(_fxPrefilteredMap.GetRootParameterIndex("environmentMap"), _cubeMap[IBL]->GetSRVHandle());
    commandList->SetComputeRootDescriptorTable(_fxPrefilteredMap.GetRootParameterIndex("prefilteredMap"), _prefilteredMap->GetUAVHandle());

    const auto& desc = _prefilteredMap->GetResourceDesc();
    float preFilterParams[2] = {CUBE_MAP_SIZE, 0.0f};
    
    for (UINT16 i = 0; i < desc.MipLevels; i++)
    {
        preFilterParams[1] = static_cast<float>(i) / static_cast<float>(desc.MipLevels - 1);

        commandList->SetComputeRoot32BitConstants(_fxPrefilteredMap.GetRootParameterIndex("bit32_2_preFilter"), 2, preFilterParams, 0);
        commandList->SetComputeRootDescriptorTable(_fxPrefilteredMap.GetRootParameterIndex("prefilteredMap"), _prefilteredMap->GetUAVHandle(i));

        UINT mipWidth  = PREFILTERED_MAP_SIZE >> i;
        UINT mipHeight = PREFILTERED_MAP_SIZE >> i;
        UINT dispatchX = (mipWidth + 15) / 16;
        UINT dispatchY = (mipHeight + 15) / 16;

        commandList->Dispatch(dispatchX, dispatchY, 6);
    }

    _hasIBLTexture = true;
}

void SkyBox::Initialize()
{
    _box->InitializeInverted(500.f, 500.f, 500.f, 0);
    HRESULT hr = S_OK;

    FAILED_CHECK_MESSAGE(hr, L"SkyBox::Initialize device->CreateDescriptorHeap Failed");
    CreatePipelineState();

    _cubeMap[ENV]   = std::make_unique<UnorderedAccessView>();
    _cubeMap[IBL]   = std::make_unique<UnorderedAccessView>();
    _irradianceMap  = std::make_unique<UnorderedAccessView>();
    _prefilteredMap = std::make_unique<UnorderedAccessView>();
    _brdfLUT        = std::make_unique<UnorderedAccessView>();

    auto desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT, CUBE_MAP_SIZE, CUBE_MAP_SIZE, 6, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    _cubeMap[IBL]->InitializeAsTexture(desc, UnorderedAccessView::UAVSliceType::PER_MIP, true, D3D12_SRV_DIMENSION_TEXTURECUBE);
    _cubeMap[ENV]->InitializeAsTexture(desc, UnorderedAccessView::UAVSliceType::PER_MIP, true, D3D12_SRV_DIMENSION_TEXTURECUBE);

    desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT, IRRADIANCE_MAP_SIZE, IRRADIANCE_MAP_SIZE, 6, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    _irradianceMap->InitializeAsTexture(desc, UnorderedAccessView::UAVSliceType::PER_MIP, true, D3D12_SRV_DIMENSION_TEXTURECUBE);

    desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT, PREFILTERED_MAP_SIZE, PREFILTERED_MAP_SIZE, 6, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    _prefilteredMap->InitializeAsTexture(desc, UnorderedAccessView::UAVSliceType::PER_MIP, true, D3D12_SRV_DIMENSION_TEXTURECUBE);

    desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT, BRDF_LUT_SIZE, BRDF_LUT_SIZE, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    _brdfLUT->InitializeAsTexture(desc, UnorderedAccessView::UAVSliceType::PER_MIP, true, D3D12_SRV_DIMENSION_TEXTURE2D);

    Global::viewManager->AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _hdrSRVHandles[ENV]);
    Global::viewManager->AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _hdrSRVHandles[IBL]);
}

void SkyBox::Render(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex)
{
    commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, _cubeMap[ENV]->GetSRVHandle());
    _box->Render(commandList);
}

void SkyBox::ResetEnvironmentResource()
{
    _hasEnvTexture = false;
}

void SkyBox::ResetIBLResource()
{
    _hasIBLTexture = false;
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

void SkyBox::UploadToTexture2D(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ID3D12Resource* texture, const void* data, size_t dataSize)
{
    D3D12_RESOURCE_DESC texDesc = texture->GetDesc();

    UINT64 uploadBufferSize = 0;
    device->GetCopyableFootprints(&texDesc, 0, 1, 0, nullptr, nullptr, nullptr, &uploadBufferSize);

    auto                  heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC bufferDesc   = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

    ComPtr<ID3D12Resource> uploadResrouce;
   
    HRESULT hr = S_OK;
    hr         = device->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&uploadResrouce));
    FAILED_CHECK_MESSAGE(hr, L"SkyBox::UploadToTexture2D device->CreateCommittedResource Failed");

    Global::device->UpdateBuffer(uploadResrouce, data, dataSize);

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

    Global::device->UploadResource(uploadResrouce);
}

void SkyBox::CreateHDRSRV(ID3D12Resource* resource, DescriptorHandles& handles)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format                  = resource->GetDesc().Format;
    srvDesc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels     = 1;
    
    Global::device->GetDevice()->CreateShaderResourceView(resource, &srvDesc, handles.CPU);
}

void SkyBox::CreatePipelineState()
{
    ComputePipelineStateStream pss;
    _fxCubeMap.SetPipelineStateStream(pss);
    _pipelineState[CUBE_MAP] = Global::pipelineStateManager->GetPipelineState(pss);
    
    _fxIrradianceMap.SetPipelineStateStream(pss);
    _pipelineState[IRRADIANCE_MAP] = Global::pipelineStateManager->GetPipelineState(pss);
    
    _fxPrefilteredMap.SetPipelineStateStream(pss);
    _pipelineState[PREFILTERED_MAP] = Global::pipelineStateManager->GetPipelineState(pss);
    
    _fxBrdfLUT.SetPipelineStateStream(pss);
    _pipelineState[BRDF_LUT] = Global::pipelineStateManager->GetPipelineState(pss);    
}