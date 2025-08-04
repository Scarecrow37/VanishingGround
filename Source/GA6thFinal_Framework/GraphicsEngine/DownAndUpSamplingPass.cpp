#include "pch.h"
#include "DownAndUpSamplingPass.h"
#include "Module/GaussianBlurModule.h"
#include "Module/AccumulationModule.h"

DownAndUpSamplingPass::DownAndUpSamplingPass() = default;

DownAndUpSamplingPass::~DownAndUpSamplingPass() = default;

void DownAndUpSamplingPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    __super::Initialize(ownerScene, ownerTechnique, commandList);

    _shaders[DOWN_SAMPLING] = std::make_unique<ShaderBuilder>();
    _shaders[DOWN_SAMPLING]->BeginBuild();
    _shaders[DOWN_SAMPLING]->SetShader(L"../Shaders/vs_quad.hlsl", ShaderBuilder::Type::VS);
    _shaders[DOWN_SAMPLING]->SetShader(L"../Shaders/ps_down_sample.hlsl", ShaderBuilder::Type::PS);
    _shaders[DOWN_SAMPLING]->EndBuild();

    _shaders[UP_SAMPLING] = std::make_unique<ShaderBuilder>();
    _shaders[UP_SAMPLING]->BeginBuild();
    _shaders[UP_SAMPLING]->SetShader(L"../Shaders/vs_quad.hlsl", ShaderBuilder::Type::VS);
    _shaders[UP_SAMPLING]->SetShader(L"../Shaders/ps_up_sample.hlsl", ShaderBuilder::Type::PS);
    _shaders[UP_SAMPLING]->EndBuild();    

    ID3D12Device* device = Global::device->GetDevice();
    HRESULT       hr     = S_OK;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psodesc = {};
    psodesc.RasterizerState                    = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psodesc.BlendState                         = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psodesc.DepthStencilState                  = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psodesc.DepthStencilState.DepthEnable      = FALSE;
    psodesc.SampleMask                         = UINT_MAX;
    psodesc.PrimitiveTopologyType              = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psodesc.NumRenderTargets                   = 1;
    psodesc.RTVFormats[0]                      = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psodesc.SampleDesc                         = {1, 0};

    psodesc.InputLayout    = _shaders[DOWN_SAMPLING]->GetInputLayout();
    psodesc.pRootSignature = _shaders[DOWN_SAMPLING]->GetRootSignature();
    psodesc.VS             = _shaders[DOWN_SAMPLING]->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS             = _shaders[DOWN_SAMPLING]->GetShaderByteCode(ShaderBuilder::Type::PS);

    hr = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&_pipelineStates[DOWN_SAMPLING]));
    FAILED_CHECK_MESSAGE(hr, L"DownAndUpSamplingPass::Initialize device->CreateGraphicsPipelineState Failed");

    psodesc.InputLayout    = _shaders[UP_SAMPLING]->GetInputLayout();
    psodesc.pRootSignature = _shaders[UP_SAMPLING]->GetRootSignature();
    psodesc.VS             = _shaders[UP_SAMPLING]->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS             = _shaders[UP_SAMPLING]->GetShaderByteCode(ShaderBuilder::Type::PS);

    hr = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&_pipelineStates[UP_SAMPLING]));
    FAILED_CHECK_MESSAGE(hr, L"DownAndUpSamplingPass::Initialize device->CreateGraphicsPipelineState Failed");

    _pingpongTarget[0] = MakeSharedResource<RenderTarget>();
    _pingpongTarget[1] = MakeSharedResource<RenderTarget>();

    const auto& resolution = Global::device->GetResolution();
    auto        desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT, 
                                                    resolution.Width >> 1, 
                                                    resolution.Height >> 1, 
                                                    1, 
                                                    MAX_MIPMAP_LEVEL, 
                                                    1, 
                                                    0, 
                                                    D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

    _pingpongTarget[0]->Initialize(desc, 0.247f);
    _pingpongTarget[1]->Initialize(desc, 0.247f);
}

void DownAndUpSamplingPass::AddRenderPassDatas(std::string_view sceneName)
{
    auto device = Global::device->GetDevice();
    auto desc   = _meshRenderTarget->GetResource()->GetDesc();

    // 디버그 텍스처 생성
    auto    heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    HRESULT hr        = device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc,
                                                        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr,
                                                        IID_PPV_ARGS(&_finalTexture));

    FAILED_CHECK_MESSAGE(hr, L"Failed to create debug texture for DownAndUpSamplingPass");
    _finalTexture->SetName(L"Bloom_DebugTexture");

    // 디버그 텍스처용 SRV 생성
    Global::viewManager->AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _finalHandle);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format                          = desc.Format;
    srvDesc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Texture2D.MipLevels             = 1;
    device->CreateShaderResourceView(_finalTexture.Get(), &srvDesc, _finalHandle.CPU);

    Global::renderPassDatas->AddRenderPassProperty(sceneName, "BloomPass", BloomPassProperty({1.f, 1.f, 0.2f}));
    Global::renderPassDatas->AddRenderPassImage(sceneName, "BloomPass", "BloomTexture", _finalHandle.GPU);
}

void DownAndUpSamplingPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    _sharedRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    _pingpongTarget[0]->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    _pingpongTarget[1]->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);

    _activeSRVs.clear();
}

void DownAndUpSamplingPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    // Down Scale Pass
    commandList->SetPipelineState(_pipelineStates[DOWN_SAMPLING].Get());
    commandList->SetGraphicsRootSignature(_shaders[DOWN_SAMPLING]->GetRootSignature());

    UINT currentIndex = 0;
    D3D12_GPU_DESCRIPTOR_HANDLE currentSRVHandle = _sharedRenderTarget->GetSRVHandle();

    for (UINT i = 0; i < MAX_MIPMAP_LEVEL; i++)
    {
        _pingpongTarget[currentIndex]->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);

        commandList->OMSetRenderTargets(1, &_pingpongTarget[currentIndex]->GetRTVHandle(i), NULL, nullptr);
        commandList->RSSetViewports(1, &_pingpongTarget[currentIndex]->GetViewport(i));
        commandList->RSSetScissorRects(1, &_pingpongTarget[currentIndex]->GetScissorRect(i));

        int mipLevel = std::max(0, (int)i - 1);
        commandList->SetGraphicsRoot32BitConstants(_shaders[DOWN_SAMPLING]->GetRootParameterIndex("bit32_1_mipLevel"), 1, &mipLevel, 0);
        commandList->SetGraphicsRootDescriptorTable(_shaders[DOWN_SAMPLING]->GetRootParameterIndex("sourceTexture"), currentSRVHandle);

        _ownerScene->_frameQuad->Render(commandList);

        _pingpongTarget[currentIndex]->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        currentSRVHandle = _pingpongTarget[currentIndex]->GetSRVHandle();
        _activeSRVs.push_back(currentSRVHandle);

        currentIndex = (currentIndex + 1) % 2;
    }

    // Up Scale Pass
    commandList->SetPipelineState(_pipelineStates[UP_SAMPLING].Get());
    commandList->SetGraphicsRootSignature(_shaders[UP_SAMPLING]->GetRootSignature());

    currentSRVHandle = _activeSRVs.back();
    const auto& renderTargetGroup = Global::multiRenderTargetManager->GetRenderTargetGroup("Mipmap");
    UINT        mipLevel[2]       = {MAX_MIPMAP_LEVEL - 1, 0};

    //     0     /    1    /    2     /   3
    // 1920x1080 / 960x540 / 480x270 / 240x135
    for (int i = MAX_MIPMAP_LEVEL - 2; i >= 0; i--)
    {
        mipLevel[1] = i;

        RenderTarget* currentTarget = renderTargetGroup[i + 1].Get();
        currentTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);

        commandList->OMSetRenderTargets(1, &currentTarget->GetRTVHandle(), NULL, nullptr);
        commandList->RSSetViewports(1, &currentTarget->GetViewport());
        commandList->RSSetScissorRects(1, &currentTarget->GetScissorRect());

        commandList->SetGraphicsRoot32BitConstants(_shaders[UP_SAMPLING]->GetRootParameterIndex("bit32_2_mipLevel"), 2, &mipLevel, 0);
        commandList->SetGraphicsRootDescriptorTable(_shaders[UP_SAMPLING]->GetRootParameterIndex("lowTexture"), currentSRVHandle);
        commandList->SetGraphicsRootDescriptorTable(_shaders[UP_SAMPLING]->GetRootParameterIndex("highTexture"), _activeSRVs[i]);

        _ownerScene->_frameQuad->Render(commandList);

        currentTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        currentSRVHandle = currentTarget->GetSRVHandle();
        mipLevel[0] = 0;
    }

    // Finalize the last mip level
    renderTargetGroup[0]->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);

    commandList->OMSetRenderTargets(1, &renderTargetGroup[0]->GetRTVHandle(), NULL, nullptr);
    commandList->RSSetViewports(1, &renderTargetGroup[0]->GetViewport());
    commandList->RSSetScissorRects(1, &renderTargetGroup[0]->GetScissorRect());

    mipLevel[1] = 0;
    commandList->SetGraphicsRoot32BitConstants(_shaders[UP_SAMPLING]->GetRootParameterIndex("bit32_2_mipLevel"), 2, &mipLevel, 0);
    commandList->SetGraphicsRootDescriptorTable(_shaders[UP_SAMPLING]->GetRootParameterIndex("lowTexture"), currentSRVHandle);
    commandList->SetGraphicsRootDescriptorTable(_shaders[UP_SAMPLING]->GetRootParameterIndex("highTexture"), _sharedRenderTarget->GetSRVHandle());

    _ownerScene->_frameQuad->Render(commandList);
}

void DownAndUpSamplingPass::End(ID3D12GraphicsCommandList* commandList)
{
    const auto& renderTargetGroup = Global::multiRenderTargetManager->GetRenderTargetGroup("Mipmap");

    renderTargetGroup[0]->TransitionResource(commandList, D3D12_RESOURCE_STATE_COPY_SOURCE);
    _sharedRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_COPY_DEST);

    auto br = CD3DX12_RESOURCE_BARRIER::Transition(_finalTexture.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
    commandList->ResourceBarrier(1, &br);

    commandList->CopyResource(_sharedRenderTarget->GetResource(), renderTargetGroup[0]->GetResource());
    commandList->CopyResource(_finalTexture.Get(), renderTargetGroup[0]->GetResource());

    _sharedRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    br = CD3DX12_RESOURCE_BARRIER::Transition(_finalTexture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandList->ResourceBarrier(1, &br);

    _pingpongTarget[0]->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    _pingpongTarget[1]->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    auto accumulationModule = Global::moduleManager->GetModule<AccumulationModule>();
    accumulationModule->Execute(commandList, _sharedRenderTarget->GetSRVHandle(), _ownerScene->_accumulationBuffer);
}