#include "pch.h"
#include "OutLinePass.h"
#include "RenderScene.h"
#include "RenderTarget.h"
#include "Quad.h"
#include "UnorderedAccessView.h"

OutLinePass::OutLinePass() {}

OutLinePass::~OutLinePass() {}

void OutLinePass::Initialize()
{
    __super::Initialize();

    _shader = std::make_unique<ShaderBuilder>();
    _shader->BeginBuild();
    _shader->SetShader(L"../Shaders/vs_quad.hlsl", ShaderBuilder::Type::VS);
    _shader->SetShader(L"../Shaders/ps_outline.hlsl", ShaderBuilder::Type::PS);
    _shader->EndBuild(ShaderBuilder::BindType::DIRECT);

    ID3D12Device*                      device = UmDevice.GetDevice();
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
    FAILED_CHECK_MESSAGE(hr, L"OutLinePass::Initialize device->CreateGraphicsPipelineState Failed");
}

void OutLinePass::Begin(ID3D12GraphicsCommandList* commandList)
{
    commandList->OMSetRenderTargets(0, nullptr, FALSE, nullptr);
    commandList->RSSetViewports(1, &_meshRenderTarget->GetViewPort());
    commandList->RSSetScissorRects(1, &_meshRenderTarget->GetScissorRect());
}

void OutLinePass::Draw(ID3D12GraphicsCommandList* commandList)
{
    const auto&     mode = UmDevice.GetMode();
    PostProcessData postProcessData{.ScreenSize      = {(float)mode.Width, (float)mode.Height},
                                    .PostProcessMask = PostProcess::OUTLINE};
    postProcessData.TexelSize = 1.f / postProcessData.ScreenSize;

    auto worldTarget       = UmMultiRenderTargetManager.GetRenderTarget("WorldPosition");
    auto customDepthTarget = UmMultiRenderTargetManager.GetRenderTarget("CustomDepth");

    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_shader->GetRootSignature());
    
    commandList->SetGraphicsRootConstantBufferView(_shader->GetRootParameterIndex("cameraData"), _ownerScene->_cameraBuffer->GetGPUVirtualAddress());
    commandList->SetGraphicsRoot32BitConstants(_shader->GetRootParameterIndex("bit32_5_postProcessData"), 5, &postProcessData, 0);
    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("worldTexture"), worldTarget->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("customDepthTexture"), customDepthTarget->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("accumulation"), _ownerScene->_accumulationBuffer->GetUAVHandle());

    _ownerScene->_frameQuad->Render(commandList);
}

void OutLinePass::End(ID3D12GraphicsCommandList* commandList)
{
}