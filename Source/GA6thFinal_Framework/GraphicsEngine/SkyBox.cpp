#include "pch.h"
#include "SkyBox.h"
#include "Box.h"

constexpr UINT CUBE_MAP_SIZE        = 512;
constexpr UINT IRRADIANCE_MAP_SIZE  = 128;
constexpr UINT PREFILTERED_MAP_SIZE = 256;
constexpr UINT BRDF_LUT_SIZE        = 512;

SkyBox::SkyBox()
    : _box{std::make_unique<Box>()}
    , _hasTexture{false}
{
}

SkyBox::~SkyBox() {}

void SkyBox::SetEnvironmentTexture(std::wstring_view path)
{
    // HDR/EXR 이미지 로드
    ScratchImage image;
    TexMetadata  metadata;

    HRESULT hr = LoadFromHDRFile(path.data(), &metadata, image);
    if (FAILED(hr))
    {
        _hasTexture = false;
        return;
    }
    FAILED_CHECK_MESSAGE(hr, L"SkyBox::SetTexture LoadFromHDRFile Failed");

    const Image* img = image.GetImage(0, 0, 0);

    size_t                     imageSize    = img->slicePitch;
    ID3D12Device*              pDevice      = Global::device->GetDevice();
    ID3D12GraphicsCommandList* commandList  = Global::device->GetCommandList();

    // DirectXTex에서 가져온 포맷 사용 (보통 R32G32B32A32_FLOAT)
    _skyboxhdrTexture = CreateTexture2D(pDevice, static_cast<int>(metadata.width), static_cast<int>(metadata.height), metadata.format);

    UploadToTexture2D(pDevice, commandList, _skyboxhdrTexture.Get(), img->pixels, imageSize);

    CreateHDRSRV(_skyboxhdrTexture.Get());

    // CubeMap 생성
    commandList->SetPipelineState(_pipelineState[CUBE_MAP].Get());
    commandList->SetComputeRootSignature(_shader[CUBE_MAP]->GetRootSignature());

    auto descriptorHeap = Global::viewManager->GetShaderResourceHeap();
    commandList->SetDescriptorHeaps(1, &descriptorHeap);

    commandList->SetComputeRootSignature(_shader[CUBE_MAP]->GetRootSignature());
    commandList->SetComputeRootDescriptorTable(_shader[CUBE_MAP]->GetRootParameterIndex("equirectangularMap"), _hdrSRVHandles.GPU);
    commandList->SetComputeRootDescriptorTable(_shader[CUBE_MAP]->GetRootParameterIndex("cubeMap"), _cubeMap->GetUAVHandle());
    commandList->SetComputeRoot32BitConstants(_shader[CUBE_MAP]->GetRootParameterIndex("bit32_1_cubeMapInfo"), 1, &CUBE_MAP_SIZE, 0);
    commandList->Dispatch((CUBE_MAP_SIZE + 15) / 16, (CUBE_MAP_SIZE + 15) / 16, 6);
}

void SkyBox::SetIBLTexture(std::wstring_view path)
{
    // HDR/EXR 이미지 로드
    ScratchImage image;
    TexMetadata  metadata;

    HRESULT hr = LoadFromHDRFile(path.data(), &metadata, image);
    if (FAILED(hr))
    {
        _hasTexture = false;
        return;
    }
    FAILED_CHECK_MESSAGE(hr, L"SkyBox::SetTexture LoadFromHDRFile Failed");

    const Image* img = image.GetImage(0, 0, 0);

    size_t                     imageSize    = img->slicePitch;
    ID3D12Device*              pDevice      = Global::device->GetDevice();
    ID3D12GraphicsCommandList* commandList  = Global::device->GetCommandList();

    // DirectXTex에서 가져온 포맷 사용 (보통 R32G32B32A32_FLOAT)
    _skyboxhdrTexture = CreateTexture2D(pDevice, static_cast<int>(metadata.width), static_cast<int>(metadata.height), metadata.format);

    UploadToTexture2D(pDevice, commandList, _skyboxhdrTexture.Get(), img->pixels, imageSize);

    CreateHDRSRV(_skyboxhdrTexture.Get());

    // CubeMap 생성
    commandList->SetPipelineState(_pipelineState[CUBE_MAP].Get());
    commandList->SetComputeRootSignature(_shader[CUBE_MAP]->GetRootSignature());

    auto descriptorHeap = Global::viewManager->GetShaderResourceHeap();
    commandList->SetDescriptorHeaps(1, &descriptorHeap);

    commandList->SetComputeRootSignature(_shader[CUBE_MAP]->GetRootSignature());
    commandList->SetComputeRootDescriptorTable(_shader[CUBE_MAP]->GetRootParameterIndex("equirectangularMap"), _hdrSRVHandles.GPU);
    commandList->SetComputeRootDescriptorTable(_shader[CUBE_MAP]->GetRootParameterIndex("cubeMap"), _cubeMap->GetUAVHandle());
    commandList->SetComputeRoot32BitConstants(_shader[CUBE_MAP]->GetRootParameterIndex("bit32_1_cubeMapInfo"), 1, &CUBE_MAP_SIZE, 0);
    commandList->Dispatch((CUBE_MAP_SIZE + 15) / 16, (CUBE_MAP_SIZE + 15) / 16, 6);
    
    _cubeMap->ResourceBarrier(commandList);
    _cubeMap->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    // BRDF LUT 생성
    commandList->SetPipelineState(_pipelineState[BRDF_LUT].Get());
    commandList->SetComputeRootSignature(_shader[BRDF_LUT]->GetRootSignature());
    commandList->SetComputeRootDescriptorTable(_shader[BRDF_LUT]->GetRootParameterIndex("brdfLUT"), _brdfLUT->GetUAVHandle());

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
            commandList->SetComputeRoot32BitConstants(_shader[BRDF_LUT]->GetRootParameterIndex("bit32_2_brdfConstants"), 2, offset, 0);
            commandList->Dispatch(dispatchX, dispatchY, 1);
        }
    }
    // commandList->Dispatch((BRDF_LUT_SIZE + 15) / 16, (BRDF_LUT_SIZE + 15) / 16, 1);

    // IrradianceMap 생성
    commandList->SetPipelineState(_pipelineState[IRRADIANCE_MAP].Get());
    commandList->SetComputeRootSignature(_shader[IRRADIANCE_MAP]->GetRootSignature());
    commandList->SetComputeRootDescriptorTable(_shader[IRRADIANCE_MAP]->GetRootParameterIndex("environmentMap"), _cubeMap->GetSRVHandle());
    commandList->SetComputeRootDescriptorTable(_shader[IRRADIANCE_MAP]->GetRootParameterIndex("irradianceMap"), _irradianceMap->GetUAVHandle());
    commandList->Dispatch((IRRADIANCE_MAP_SIZE + 15) / 16, (IRRADIANCE_MAP_SIZE + 15) / 16, 6);

    // PrefilteredMap 생성
    commandList->SetPipelineState(_pipelineState[PREFILTERED_MAP].Get());
    commandList->SetComputeRootSignature(_shader[PREFILTERED_MAP]->GetRootSignature());
    commandList->SetComputeRootDescriptorTable(_shader[PREFILTERED_MAP]->GetRootParameterIndex("environmentMap"), _cubeMap->GetSRVHandle());
    commandList->SetComputeRootDescriptorTable(_shader[PREFILTERED_MAP]->GetRootParameterIndex("prefilteredMap"), _prefilteredMap->GetUAVHandle());

    const auto& desc = _prefilteredMap->GetResourceDesc();
    float preFilterParams[2] = {CUBE_MAP_SIZE, 0.0f};
    
    for (UINT16 i = 0; i < desc.MipLevels; i++)
    {
        preFilterParams[1] = static_cast<float>(i) / static_cast<float>(desc.MipLevels - 1);

        commandList->SetComputeRoot32BitConstants(_shader[PREFILTERED_MAP]->GetRootParameterIndex("bit32_2_preFilter"), 2, preFilterParams, 0);
        commandList->SetComputeRootDescriptorTable(_shader[PREFILTERED_MAP]->GetRootParameterIndex("prefilteredMap"), _prefilteredMap->GetUAVHandle(i));

        UINT mipWidth  = PREFILTERED_MAP_SIZE >> i;
        UINT mipHeight = PREFILTERED_MAP_SIZE >> i;
        UINT dispatchX = (mipWidth + 15) / 16;
        UINT dispatchY = (mipHeight + 15) / 16;

        commandList->Dispatch(dispatchX, dispatchY, 6);
    }

    _hasTexture = true;
}

void SkyBox::Initialize()
{
    _box->InitializeInverted(1000.f, 1000.f, 1000.f, 0);
    HRESULT hr = S_OK;

    FAILED_CHECK_MESSAGE(hr, L"SkyBox::Initialize device->CreateDescriptorHeap Failed");
    CreatePipelineState();

    _cubeMap        = std::make_unique<UnorderedAccessView>();
    _irradianceMap  = std::make_unique<UnorderedAccessView>();
    _prefilteredMap = std::make_unique<UnorderedAccessView>();
    _brdfLUT        = std::make_unique<UnorderedAccessView>();

    auto desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT, CUBE_MAP_SIZE, CUBE_MAP_SIZE, 6, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    _cubeMap->Initialize(desc, D3D12_UAV_DIMENSION_TEXTURE2DARRAY, D3D12_SRV_DIMENSION_TEXTURECUBE);

    desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT, IRRADIANCE_MAP_SIZE, IRRADIANCE_MAP_SIZE, 6, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    _irradianceMap->Initialize(desc, D3D12_UAV_DIMENSION_TEXTURE2DARRAY, D3D12_SRV_DIMENSION_TEXTURECUBE);

    desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT, PREFILTERED_MAP_SIZE, PREFILTERED_MAP_SIZE, 6, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    _prefilteredMap->Initialize(desc, D3D12_UAV_DIMENSION_TEXTURE2DARRAY, D3D12_SRV_DIMENSION_TEXTURECUBE);

    desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT, BRDF_LUT_SIZE, BRDF_LUT_SIZE, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    _brdfLUT->Initialize(desc, D3D12_UAV_DIMENSION_TEXTURE2D);

    Global::viewManager->AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _hdrSRVHandles);
}

void SkyBox::Render(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex)
{
    commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, _cubeMap->GetSRVHandle());
    _box->Render(commandList);
}

void SkyBox::ResetResource() 
{
    _skyboxhdrTexture.Reset();
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

    Global::device->UploadResource(uploadResrouce);
}

void SkyBox::CreateHDRSRV(ID3D12Resource* resource)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format                  = resource->GetDesc().Format;
    srvDesc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels     = 1;
    
    Global::device->GetDevice()->CreateShaderResourceView(resource, &srvDesc, _hdrSRVHandles.CPU);
}

void SkyBox::CreatePipelineState()
{    
    ID3D12Device*                     device = Global::device->GetDevice();
    HRESULT                           hr     = S_OK;
    D3D12_COMPUTE_PIPELINE_STATE_DESC psodesc{};

    _shader[CUBE_MAP] = std::make_unique<ShaderBuilder>();
    _shader[CUBE_MAP]->BeginBuild();
    _shader[CUBE_MAP]->SetShader(L"../Shaders/cs_cube_texture_convertor.hlsl", ShaderBuilder::Type::CS);
    _shader[CUBE_MAP]->EndBuild();

    psodesc.pRootSignature = _shader[CUBE_MAP]->GetRootSignature();
    psodesc.CS             = _shader[CUBE_MAP]->GetShaderByteCode(ShaderBuilder::Type::CS);
    psodesc.Flags          = D3D12_PIPELINE_STATE_FLAG_NONE;

    hr = device->CreateComputePipelineState(&psodesc, IID_PPV_ARGS(&_pipelineState[CUBE_MAP]));
    FAILED_CHECK_MESSAGE(hr, L"SkyBox::CreateComputePSO device->CreateComputePipelineState Failed");

    _shader[IRRADIANCE_MAP] = std::make_unique<ShaderBuilder>();
    _shader[IRRADIANCE_MAP]->BeginBuild();
    _shader[IRRADIANCE_MAP]->SetShader(L"../Shaders/cs_irradiance_map.hlsl", ShaderBuilder::Type::CS);
    _shader[IRRADIANCE_MAP]->EndBuild();

    psodesc.pRootSignature = _shader[IRRADIANCE_MAP]->GetRootSignature();
    psodesc.CS             = _shader[IRRADIANCE_MAP]->GetShaderByteCode(ShaderBuilder::Type::CS);
    psodesc.Flags          = D3D12_PIPELINE_STATE_FLAG_NONE;

    hr = device->CreateComputePipelineState(&psodesc, IID_PPV_ARGS(&_pipelineState[IRRADIANCE_MAP]));
    FAILED_CHECK_MESSAGE(hr, L"SkyBox::CreateComputePSO device->CreateComputePipelineState Failed");

    _shader[PREFILTERED_MAP] = std::make_unique<ShaderBuilder>();
    _shader[PREFILTERED_MAP]->BeginBuild();
    _shader[PREFILTERED_MAP]->SetShader(L"../Shaders/cs_prefiltered_map.hlsl", ShaderBuilder::Type::CS);
    _shader[PREFILTERED_MAP]->EndBuild();
    psodesc.pRootSignature = _shader[PREFILTERED_MAP]->GetRootSignature();
    psodesc.CS             = _shader[PREFILTERED_MAP]->GetShaderByteCode(ShaderBuilder::Type::CS);
    psodesc.Flags          = D3D12_PIPELINE_STATE_FLAG_NONE;

    hr = device->CreateComputePipelineState(&psodesc, IID_PPV_ARGS(&_pipelineState[PREFILTERED_MAP]));
    FAILED_CHECK_MESSAGE(hr, L"SkyBox::CreateComputePSO device->CreateComputePipelineState Failed");

    _shader[BRDF_LUT] = std::make_unique<ShaderBuilder>();
    _shader[BRDF_LUT]->BeginBuild();
    _shader[BRDF_LUT]->SetShader(L"../Shaders/cs_brdf_lut.hlsl", ShaderBuilder::Type::CS);
    _shader[BRDF_LUT]->EndBuild();

    psodesc.pRootSignature = _shader[BRDF_LUT]->GetRootSignature();
    psodesc.CS             = _shader[BRDF_LUT]->GetShaderByteCode(ShaderBuilder::Type::CS);
    psodesc.Flags          = D3D12_PIPELINE_STATE_FLAG_NONE;

    hr = device->CreateComputePipelineState(&psodesc, IID_PPV_ARGS(&_pipelineState[BRDF_LUT]));
    FAILED_CHECK_MESSAGE(hr, L"SkyBox::CreateComputePSO device->CreateComputePipelineState Failed");
}