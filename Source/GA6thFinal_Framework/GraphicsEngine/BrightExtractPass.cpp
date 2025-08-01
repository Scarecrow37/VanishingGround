#include "pch.h"
#include "BrightExtractPass.h"
#include "Module/GaussianBlurModule.h"

BrightExtractPass::BrightExtractPass() {}

BrightExtractPass::~BrightExtractPass() {}

void BrightExtractPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    __super::Initialize(ownerScene, ownerTechnique, commandList);

    _shader = std::make_unique<ShaderBuilder>();
    _shader->BeginBuild();
    _shader->SetShader(L"../Shaders/vs_quad.hlsl", ShaderBuilder::Type::VS);
    _shader->SetShader(L"../Shaders/ps_bright_extract.hlsl", ShaderBuilder::Type::PS);
    _shader->EndBuild();

    ID3D12Device*                      device = Global::device->GetDevice();
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psodesc{};
    psodesc.RasterizerState               = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psodesc.BlendState                    = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psodesc.DepthStencilState             = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psodesc.DepthStencilState.DepthEnable = FALSE;
    psodesc.SampleMask                    = UINT_MAX;
    psodesc.PrimitiveTopologyType         = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psodesc.InputLayout                   = _shader->GetInputLayout();
    psodesc.NumRenderTargets              = 1;
    psodesc.RTVFormats[0]                 = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psodesc.pRootSignature                = _shader->GetRootSignature();
    psodesc.SampleDesc                    = {1, 0};
    psodesc.VS                            = _shader->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS                            = _shader->GetShaderByteCode(ShaderBuilder::Type::PS);
    
    HRESULT hr = S_OK;
    hr         = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&_pipelineState));
    FAILED_CHECK_MESSAGE(hr, L"BrightExtractPass::Initialize device->CreateGraphicsPipelineState Failed");    
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
    commandList->SetGraphicsRootSignature(_shader->GetRootSignature());

    commandList->SetGraphicsRoot32BitConstants(_shader->GetRootParameterIndex("bit32_5_postProcessData"), 5, &postProcessData, 0);
    commandList->SetGraphicsRoot32BitConstants(_shader->GetRootParameterIndex("bit32_3_bloomProperty"), 3, &bloomProperty, 0);
    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("screenTexture"), _meshRenderTarget->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("customDepthTexture"), customDepthTarget->GetSRVHandle());
    
    _ownerScene->_frameQuad->Render(commandList);

    auto gaussianBlurModule = Global::moduleManager->GetModule<GaussianBlurModule>();

    // x tab
    auto renderTarget = Global::multiRenderTargetManager->GetAvailableRenderTarget();
    renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);

    gaussianBlurModule->Execute(commandList, _sharedRenderTarget->GetSRVHandle(), renderTarget.Get(), GaussianBlurModule::GaussianBlurType::AXIS_X);
    renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    // y tab
    _sharedRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    
    gaussianBlurModule->Execute(commandList, renderTarget->GetSRVHandle(), _sharedRenderTarget, GaussianBlurModule::GaussianBlurType::AXIS_Y);

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