#include "pch.h"
#include "BlurYPass.h"

BlurYPass::BlurYPass() {}

BlurYPass::~BlurYPass() {}

void BlurYPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    __super::Initialize(ownerScene, ownerTechnique, commandList);

    _shader = std::make_unique<ShaderBuilder>();
    _shader->BeginBuild();
    _shader->SetShader(L"../Shaders/vs_quad.hlsl", ShaderBuilder::Type::VS);
    _shader->SetShader(L"../Shaders/ps_blur_y.hlsl", ShaderBuilder::Type::PS);
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
    psodesc.NumRenderTargets              = 0;
    psodesc.pRootSignature                = _shader->GetRootSignature();
    psodesc.SampleDesc                    = {1, 0};
    psodesc.VS                            = _shader->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS                            = _shader->GetShaderByteCode(ShaderBuilder::Type::PS);

    HRESULT hr = S_OK;
    hr         = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&_pipelineState));
    FAILED_CHECK_MESSAGE(hr, L"BlurYPass::Initialize device->CreateGraphicsPipelineState Failed");
}

void BlurYPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    commandList->OMSetRenderTargets(0, nullptr, FALSE, nullptr);
    commandList->RSSetViewports(1, &_meshRenderTarget->GetViewPort());
    commandList->RSSetScissorRects(1, &_meshRenderTarget->GetScissorRect());
}

void BlurYPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    const auto&     resolution = Global::device->GetResolution();
    PostProcessData postProcessData{.TexelSize = {1.f / (float)resolution.Width, 1.f / (float)resolution.Height}};

    auto&       multiRenderTargetManager = Global::multiRenderTargetManager;
    const auto& usedRenderTargets        = multiRenderTargetManager->GetUsedRenderTargets();

    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_shader->GetRootSignature());

    commandList->SetGraphicsRoot32BitConstants(_shader->GetRootParameterIndex("bit32_5_postProcessData"), 5, &postProcessData, 0);
    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("sourceTexture"), usedRenderTargets.front()->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("accumulation"), _ownerScene->_accumulationBuffer->GetUAVHandle());

    _ownerScene->_frameQuad->Render(commandList);
}

void BlurYPass::End(ID3D12GraphicsCommandList* commandList)
{
    const auto& usedRenderTargets = Global::multiRenderTargetManager->GetUsedRenderTargets();
    Global::multiRenderTargetManager->ReturnRenderTarget(usedRenderTargets.back());
}