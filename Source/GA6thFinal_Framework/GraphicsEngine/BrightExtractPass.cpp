#include "pch.h"
#include "BrightExtractPass.h"
#include "Module/GaussianBlurModule.h"

BrightExtractPass::BrightExtractPass() {}

BrightExtractPass::~BrightExtractPass() {}

void BrightExtractPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    __super::Initialize(ownerScene, ownerTechnique, commandList);

    PipelineStateStream pss;
    pss.BlendState                        = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pss.RasterizerState                   = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pss.DepthStencilState                 = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    (&pss.DepthStencilState)->DepthEnable = FALSE;
    pss.PrimitiveTopology                 = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pss.RTVFormats                        = {{DXGI_FORMAT_R32G32B32A32_FLOAT}, 1};

    _fx.SetPipelineStateStream(pss);
    _pipelineState = Global::pipelineStateManager->GetPipelineState(pss);    
}

void BrightExtractPass::AddRenderPassDatas(std::string_view sceneName)
{
    auto device       = Global::device->GetDevice();
    auto desc         = _meshRenderTarget->GetResource()->GetDesc();

    // 디버그 텍스처 생성
    auto    heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    HRESULT hr        = device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc,
                                                        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr,
                                                        IID_PPV_ARGS(&_finalTexture));

    FAILED_CHECK_MESSAGE(hr, L"Failed to create debug texture for BrightExtractPass");
    _finalTexture->SetName(L"BrightExtract_DebugTexture");

    // 디버그 텍스처용 SRV 생성
    Global::viewManager->AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _finalHandle);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format                           = desc.Format;
    srvDesc.ViewDimension                    = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Shader4ComponentMapping          = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Texture2D.MipLevels              = 1;
    device->CreateShaderResourceView(_finalTexture.Get(), &srvDesc, _finalHandle.CPU);

    Global::renderPassDatas->AddRenderPassProperty(sceneName, "BloomPass", BloomPassProperty({1.f, 1.f, 0.2f}));
    Global::renderPassDatas->AddRenderPassImage(sceneName, "BloomPass", "BrightExtractTexture", _finalHandle.GPU);
}

void BrightExtractPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    _sharedRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);

    commandList->OMSetRenderTargets(1, &_sharedRenderTarget->GetRTVHandle(), FALSE, nullptr);
    commandList->RSSetViewports(1, &_sharedRenderTarget->GetViewport());
    commandList->RSSetScissorRects(1, &_sharedRenderTarget->GetScissorRect());
}

void BrightExtractPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    const auto&     resolution = Global::device->GetResolution();
    PostProcessData postProcessData{.ScreenSize      = {(float)resolution.Width, (float)resolution.Height},
                                    .PostProcessMask = PostProcess::BLOOM};

    const auto& bloomProperty = std::any_cast<const BloomPassProperty&>(_ownerScene->GetRenderPassProperty("BloomPass"));

    auto customDepthTarget = Global::multiRenderTargetManager->GetRenderTarget("CustomDepth");

    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_fx.GetRootSignature());

    commandList->SetGraphicsRoot32BitConstants(_fx.GetRootParameterIndex("bit32_6_postProcessData"), 6, &postProcessData, 0);
    commandList->SetGraphicsRoot32BitConstants(_fx.GetRootParameterIndex("bit32_3_bloomProperty"), 3, &bloomProperty, 0);
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("screenTexture"), _meshRenderTarget->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("customDepthTexture"), customDepthTarget->GetSRVHandle());
    
    _ownerScene->_frameQuad->Render(commandList);

    auto gaussianBlurModule = Global::moduleManager->GetModule<GaussianBlurModule>();

    // x tab
    auto renderTarget = Global::multiRenderTargetManager->GetAvailableRenderTarget();
    renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    _sharedRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    gaussianBlurModule->Execute(commandList, _sharedRenderTarget->GetSRVHandle(), renderTarget.Get(), DXGI_FORMAT_R32G32B32A32_FLOAT, GaussianBlurModule::BlurType::AXIS_X);
    renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    // y tab
    _sharedRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);

    gaussianBlurModule->Execute(commandList, renderTarget->GetSRVHandle(), _sharedRenderTarget, DXGI_FORMAT_R32G32B32A32_FLOAT, GaussianBlurModule::BlurType::AXIS_Y);

    Global::multiRenderTargetManager->ReturnRenderTarget(renderTarget);
}

void BrightExtractPass::End(ID3D12GraphicsCommandList* commandList)
{
    _sharedRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_COPY_SOURCE);

    auto br = CD3DX12_RESOURCE_BARRIER::Transition(_finalTexture.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
    commandList->ResourceBarrier(1, &br);

    commandList->CopyResource(_finalTexture.Get(), _sharedRenderTarget->GetResource());

    br = CD3DX12_RESOURCE_BARRIER::Transition(_finalTexture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandList->ResourceBarrier(1, &br);

    _sharedRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}