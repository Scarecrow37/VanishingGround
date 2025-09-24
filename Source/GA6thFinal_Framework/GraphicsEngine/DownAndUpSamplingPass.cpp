#include "pch.h"
#include "DownAndUpSamplingPass.h"
#include "Module/GaussianBlurModule.h"
#include "Module/AccumulationModule.h"

DownAndUpSamplingPass::DownAndUpSamplingPass() = default;

DownAndUpSamplingPass::~DownAndUpSamplingPass() = default;

void DownAndUpSamplingPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    RenderPass::Initialize(ownerScene, ownerTechnique, commandList);

    PipelineStateStream pss;
    pss.BlendState                        = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pss.RasterizerState                   = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pss.DepthStencilState                 = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    (&pss.DepthStencilState)->DepthEnable = FALSE;
    pss.PrimitiveTopology                 = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pss.RTVFormats                        = {{DXGI_FORMAT_R32G32B32A32_FLOAT}, 1};

    _fxDownSampling.SetPipelineStateStream(pss);
    _pipelineStates[DOWN_SAMPLING] = Global::pipelineStateManager->GetPipelineState(pss);

    _fxUpSampling.SetPipelineStateStream(pss);
    _pipelineStates[UP_SAMPLING] = Global::pipelineStateManager->GetPipelineState(pss);    

    
    _pingpongTarget[0] = MakeSharedResource<RenderTarget>();
    _pingpongTarget[1] = MakeSharedResource<RenderTarget>();

    const auto& resolution = Global::device->GetResolution();
    auto        desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT,
                                                    resolution.cx >> 2,
                                                    resolution.cy >> 2,
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

    Global::renderPassDatas->AddRenderPassProperty("BloomPass", BloomPassProperty({1.f, 1.f, 0.2f}));
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
    D3D12_GPU_DESCRIPTOR_HANDLE levelInputSrv = _sharedRenderTarget->GetSRVHandle();
    auto gaussianBlurModule = Global::moduleManager->GetModule<GaussianBlurModule>();
    _activeSRVs.clear();

    UINT currentIndex = 0;

    for (UINT i = 0; i < MAX_MIPMAP_LEVEL; i++)
    {
        UINT  destIndex = (currentIndex + 1) % 2;
        auto& downsampleAndFinalBlurTarget = *_pingpongTarget[destIndex];
        auto& blurTempTarget = *_pingpongTarget[currentIndex];

        // 1. Downsample from the previous level's result into the destination target
        downsampleAndFinalBlurTarget.TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
        commandList->SetPipelineState(_pipelineStates[DOWN_SAMPLING].Get());
        commandList->SetGraphicsRootSignature(_fxDownSampling.GetRootSignature());

        commandList->OMSetRenderTargets(1, &downsampleAndFinalBlurTarget.GetRTVHandle(i), NULL, nullptr);
        commandList->RSSetViewports(1, &downsampleAndFinalBlurTarget.GetViewport(i));
        commandList->RSSetScissorRects(1, &downsampleAndFinalBlurTarget.GetScissorRect(i));

        int mipLevel = std::max(0, (int)i - 1);
        commandList->SetGraphicsRoot32BitConstants(_fxDownSampling.GetRootParameterIndex("bit32_1_mipLevel"), 1, &mipLevel, 0);
        commandList->SetGraphicsRootDescriptorTable(_fxDownSampling.GetRootParameterIndex("sourceTexture"), levelInputSrv);
        _ownerScene->_frameQuad->Render(commandList);
        downsampleAndFinalBlurTarget.TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        // 2. Gaussian Blur X-axis: downsampleTarget -> blurTempTarget
        blurTempTarget.TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
        gaussianBlurModule->Execute(commandList, &downsampleAndFinalBlurTarget, &blurTempTarget, blurTempTarget.GetFormat(), GaussianBlurModule::BlurType::AXIS_X, i);
        blurTempTarget.TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        // 3. Gaussian Blur Y-axis: blurTempTarget -> downsampleTarget (Final result for this level)
        downsampleAndFinalBlurTarget.TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
        gaussianBlurModule->Execute(commandList, &blurTempTarget, &downsampleAndFinalBlurTarget, downsampleAndFinalBlurTarget.GetFormat(), GaussianBlurModule::BlurType::AXIS_Y, i);
        downsampleAndFinalBlurTarget.TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        // The final blurred result for this level is in downsampleAndFinalBlurTarget.
        // It becomes the input for the next downsample iteration.
        levelInputSrv = downsampleAndFinalBlurTarget.GetSRVHandle();
        _activeSRVs.push_back(levelInputSrv);

        currentIndex = (currentIndex + 1) % 2;
    }

    // Up Scale Pass
    commandList->SetPipelineState(_pipelineStates[UP_SAMPLING].Get());
    commandList->SetGraphicsRootSignature(_fxUpSampling.GetRootSignature());

    D3D12_GPU_DESCRIPTOR_HANDLE currentSRVHandle = _activeSRVs.back();
    const auto& renderTargetGroup = Global::multiRenderTargetManager->GetRenderTargetGroup("Mipmap");
    UINT        mipLevel[2]       = {MAX_MIPMAP_LEVEL - 1, 0};

    for (int i = MAX_MIPMAP_LEVEL - 2; i >= 0; i--)
    {
        mipLevel[1] = i;

        RenderTarget* currentTarget = renderTargetGroup[i + 1].Get();
        currentTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);

        commandList->OMSetRenderTargets(1, &currentTarget->GetRTVHandle(), NULL, nullptr);
        commandList->RSSetViewports(1, &currentTarget->GetViewport());
        commandList->RSSetScissorRects(1, &currentTarget->GetScissorRect());

        commandList->SetGraphicsRoot32BitConstants(_fxUpSampling.GetRootParameterIndex("bit32_2_mipLevel"), 2, &mipLevel, 0);
        commandList->SetGraphicsRootDescriptorTable(_fxUpSampling.GetRootParameterIndex("lowTexture"), currentSRVHandle);
        commandList->SetGraphicsRootDescriptorTable(_fxUpSampling.GetRootParameterIndex("highTexture"), _activeSRVs[i]);

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
    commandList->SetGraphicsRoot32BitConstants(_fxUpSampling.GetRootParameterIndex("bit32_2_mipLevel"), 2, &mipLevel, 0);
    commandList->SetGraphicsRootDescriptorTable(_fxUpSampling.GetRootParameterIndex("lowTexture"), currentSRVHandle);
    commandList->SetGraphicsRootDescriptorTable(_fxUpSampling.GetRootParameterIndex("highTexture"), _sharedRenderTarget->GetSRVHandle());

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