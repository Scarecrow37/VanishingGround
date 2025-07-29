#include "pch.h"
#include "BrightExtractPass.h"

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
    auto renderTarget = Global::multiRenderTargetManager->GetAvailableRenderTarget();
    auto desc         = renderTarget->GetResource()->GetDesc();

    Global::multiRenderTargetManager->ReturnRenderTarget(renderTarget);

    // 디버그 텍스처 생성
    auto    heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    HRESULT hr        = device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc,
                                                        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr,
                                                        IID_PPV_ARGS(&_brightExtractTexture));
    FAILED_CHECK_MESSAGE(hr, L"Failed to create debug texture for BrightExtractPass");
    _brightExtractTexture->SetName(L"BrightExtract_DebugTexture");
    
    // 디버그 텍스처용 SRV 생성
    Global::viewManager->AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _brightExtractHandle);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format                           = desc.Format;
    srvDesc.ViewDimension                    = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Shader4ComponentMapping          = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Texture2D.MipLevels              = 1;
    device->CreateShaderResourceView(_brightExtractTexture.Get(), &srvDesc, _brightExtractHandle.CPU);

    Global::renderPassDatas->AddRenderPassProperty(sceneName, "BrightExtractPass", BloomPassProperty({1.f, 1.f, 0.f}));
    Global::renderPassDatas->AddRenderPassImage(sceneName, "BrightExtractPass", "BrightExtractTexture", _brightExtractHandle.GPU);
}

void BrightExtractPass::Begin(ID3D12GraphicsCommandList* commandList)
{    
    _renderTarget = Global::multiRenderTargetManager->GetAvailableRenderTarget();
    _renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    _renderTarget->ClearRenderTarget(commandList);

    commandList->OMSetRenderTargets(1, &_renderTarget->GetRTVHandle(), FALSE, nullptr);
    commandList->RSSetViewports(1, &_renderTarget->GetViewPort());
    commandList->RSSetScissorRects(1, &_renderTarget->GetScissorRect());
}

void BrightExtractPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    const auto&     resolution = Global::device->GetResolution();
    PostProcessData postProcessData{.ScreenSize      = {(float)resolution.Width, (float)resolution.Height},
                                    .PostProcessMask = PostProcess::BLOOM};

    const auto& bloomProperty = std::any_cast<const BloomPassProperty&>(_ownerScene->GetRenderPassProperty("BrightExtractPass"));

    auto customDepthTarget = Global::multiRenderTargetManager->GetRenderTarget("CustomDepth");

    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_shader->GetRootSignature());

    commandList->SetGraphicsRoot32BitConstants(_shader->GetRootParameterIndex("bit32_5_postProcessData"), 5, &postProcessData, 0);
    commandList->SetGraphicsRoot32BitConstants(_shader->GetRootParameterIndex("bit32_3_bloomProperty"), 3, &bloomProperty, 0);
    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("screenTexture"), _meshRenderTarget->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("customDepthTexture"), customDepthTarget->GetSRVHandle());
    
    _ownerScene->_frameQuad->Render(commandList);
}

void BrightExtractPass::End(ID3D12GraphicsCommandList* commandList)
{
    _renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_COPY_SOURCE);

    auto br = CD3DX12_RESOURCE_BARRIER::Transition(_brightExtractTexture.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
    commandList->ResourceBarrier(1, &br);
    
    commandList->CopyResource(_brightExtractTexture.Get(), _renderTarget->GetResource());
    
    br = CD3DX12_RESOURCE_BARRIER::Transition(_brightExtractTexture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandList->ResourceBarrier(1, &br);

    _renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}