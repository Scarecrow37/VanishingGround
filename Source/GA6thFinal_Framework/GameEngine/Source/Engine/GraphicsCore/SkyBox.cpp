#include "pch.h"
#include "SkyBox.h"
#include "Box.h"
#include "ShaderBuilder.h"

SkyBox::SkyBox():_box{std::make_unique<Box>()} {}

SkyBox::~SkyBox() {}

void SkyBox::Initialize()
{
    _box->InitializeInverted(1000.f, 1000.f, 1000.f, 0);
    HRESULT hr = S_OK;
    ComPtr<ID3D12Device> device = UmDevice.GetDevice();
    D3D12_DESCRIPTOR_HEAP_DESC hpDesc{.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                                      .NumDescriptors = 3,
                                      .Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
                                      .NodeMask       = 0};
    hr = device->CreateDescriptorHeap(&hpDesc, IID_PPV_ARGS(_descriptorHeap.GetAddressOf()));
    FAILED_CHECK_BREAK(hr);
    CreateComputePSO();
}

void SkyBox::SetTexture(std::string path)
{
    // HDR/EXR 이미지 로드
    ScratchImage image;
    TexMetadata  metadata;

    std::wstring widePath(path.begin(), path.end()); // UTF-8 → UTF-16 변환

    HRESULT hr = LoadFromHDRFile(widePath.c_str(), &metadata, image);
    FAILED_CHECK_BREAK(hr);

    const Image* img = image.GetImage(0, 0, 0);

    size_t                     imageSize    = img->slicePitch;
    ID3D12Device*              pDevice      = UmDevice.GetDevice().Get();
    ID3D12GraphicsCommandList* pCommandList = UmDevice.GetCommandList().Get();

    // DirectXTex에서 가져온 포맷 사용 (보통 R32G32B32A32_FLOAT)
    _skyboxhdrTexture =
        CreateTexture2D(pDevice, static_cast<int>(metadata.width), static_cast<int>(metadata.height), metadata.format);

    UploadToTexture2D(pDevice, pCommandList, _skyboxhdrTexture.Get(), img->pixels, imageSize);

    // Create SRV
    CreateHDRSRV(_skyboxhdrTexture.Get());

    // Create CubeMap texture (UAV)
    const UINT             cubeSize = 512;
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
}

void SkyBox::Render(ID3D12GraphicsCommandList* commnadList, UINT rootParameterIndex)
{
    commnadList->SetGraphicsRootDescriptorTable(rootParameterIndex, _cubeSRVGPU);
    _box->Render(commnadList);
}

void SkyBox::SetDescriptorHeap(ID3D12GraphicsCommandList* commnadList) 
{
    ID3D12DescriptorHeap* hps[] = {_descriptorHeap.Get()};
    commnadList->SetDescriptorHeaps(_countof(hps), hps);
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

    ComPtr<ID3D12Resource> uploadResrouce;
   
    FAILED_CHECK_BREAK(device->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &bufferDesc,
                                                       D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                       IID_PPV_ARGS(&uploadResrouce)));
    void*         mappedData = nullptr;
    CD3DX12_RANGE readRange(0, 0);
    FAILED_CHECK_BREAK(uploadResrouce->Map(0, &readRange, &mappedData));
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

    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        texture, D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE); 
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
    UINT _shaderResourceDescriptorSize   = UmDevice.GetCBVSRVUAVDescriptorSize();
    _hdrSRVCPU.ptr = _descriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr + _shaderResourceDescriptorSize;
    _hdrSRVGPU.ptr = _descriptorHeap->GetGPUDescriptorHandleForHeapStart().ptr + _shaderResourceDescriptorSize;
    UmDevice.GetDevice()->CreateShaderResourceView(resource, &srvDesc, _hdrSRVCPU);
}

void SkyBox::CreateSRV(ID3D12Resource* resource)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Shader4ComponentMapping    = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING; 
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
    srvDesc.TextureCube.MostDetailedMip = 0;
    srvDesc.TextureCube.MipLevels       = 1;
    _cubeSRVCPU.ptr = _descriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr;
    _cubeSRVGPU.ptr = _descriptorHeap->GetGPUDescriptorHandleForHeapStart().ptr;
    UmDevice.GetDevice()->CreateShaderResourceView(resource, &srvDesc, _cubeSRVCPU);
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

    UINT _shaderResourceDescriptorSize = UmDevice.GetCBVSRVUAVDescriptorSize();
    _cubeUAVCPU.ptr = _descriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr + (_shaderResourceDescriptorSize*2);
    _cubeUAVGPU.ptr = _descriptorHeap->GetGPUDescriptorHandleForHeapStart().ptr + (_shaderResourceDescriptorSize*2);
    UmDevice.GetDevice()->CreateUnorderedAccessView(resource, nullptr, &uavDesc, _cubeUAVCPU);
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

void SkyBox::BindResources(UINT cubeSize, UINT faceIndex) 
{
    ID3D12GraphicsCommandList* cmdList = UmDevice.GetCommandList().Get();

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
    UmDevice.CreateConstantBuffer(&cb,sizeof(CubeConvertConstants),_cb);

    cmdList->SetComputeRootSignature(_shader->GetRootSignature().Get());
    cmdList->SetComputeRootDescriptorTable(_shader->GetRootSignatureIndex("EquirectangularMap"),
                                                _hdrSRVGPU);  
    cmdList->SetComputeRootDescriptorTable(_shader->GetRootSignatureIndex("CubeMap"),
                                               _cubeUAVGPU);
    cmdList->SetComputeRootConstantBufferView(_shader->GetRootSignatureIndex("CubeMapInfo"),
                                              _cb->GetGPUVirtualAddress());
    _cbs.push_back(_cb);
}

void SkyBox::SetPipelineState() 
{
    ComPtr<ID3D12GraphicsCommandList> cmdList = UmDevice.GetCommandList();
    cmdList->SetPipelineState(_computePSO.Get());
    cmdList->SetComputeRootSignature(_shader->GetRootSignature().Get());
    cmdList->SetDescriptorHeaps(1,_descriptorHeap.GetAddressOf());
}