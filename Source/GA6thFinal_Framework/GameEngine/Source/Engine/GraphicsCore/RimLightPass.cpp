#include "pch.h"
#include "RimLightPass.h"
#include "RenderScene.h"
#include "RenderTarget.h"
#include "DepthStencilView.h"

RimLightPass::RimLightPass() {}

RimLightPass::~RimLightPass() {}

void RimLightPass::Initialize(const D3D12_VIEWPORT& viewPort, const D3D12_RECT& sissorRect)
{
    __super::Initialize(viewPort, sissorRect);

    _shader = std::make_unique<ShaderBuilder>();
    _shader->BeginBuild();
    _shader->SetShader(L"../Shaders/vs_quad.hlsl", ShaderBuilder::Type::VS);
    _shader->SetShader(L"../Shaders/ps_rim_light.hlsl", ShaderBuilder::Type::PS);
    _shader->EndBuild(ShaderBuilder::BindType::DIRECT);

    ID3D12Device* device = UmDevice.GetDevice();

    D3D12_BLEND_DESC blendDesc       = {};
    blendDesc.AlphaToCoverageEnable  = FALSE;
    blendDesc.IndependentBlendEnable = TRUE;
    auto& rtDesc                     = blendDesc.RenderTarget[0];
    rtDesc.BlendEnable               = TRUE;
    rtDesc.SrcBlend                  = D3D12_BLEND_SRC_ALPHA;
    rtDesc.DestBlend                 = D3D12_BLEND_INV_SRC_ALPHA;
    rtDesc.BlendOp                   = D3D12_BLEND_OP_ADD;
    rtDesc.SrcBlendAlpha             = D3D12_BLEND_ONE;
    rtDesc.DestBlendAlpha            = D3D12_BLEND_ZERO;
    rtDesc.BlendOpAlpha              = D3D12_BLEND_OP_ADD;
    rtDesc.RenderTargetWriteMask     = D3D12_COLOR_WRITE_ENABLE_ALL;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psodesc{};
    psodesc.RasterizerState               = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psodesc.RasterizerState.CullMode      = D3D12_CULL_MODE_NONE;
    psodesc.BlendState                    = blendDesc;
    psodesc.DepthStencilState             = CommonStates::DepthRead;
    psodesc.DSVFormat                     = _ownerScene->_depthStencilView->GetFormat();
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
    FAILED_CHECK_MESSAGE(hr, L"RimLightPass::Initialize device->CreateGraphicsPipelineState Failed");
}

void RimLightPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    _ownerScene->_depthStencilView->TransitionResource(commandList, D3D12_RESOURCE_STATE_DEPTH_READ);

    commandList->OMSetRenderTargets(1, &_meshRenderTarget->GetRTVHandle(), FALSE, &_ownerScene->_depthStencilView->GetDSVHandle());
    commandList->RSSetViewports(1, &_viewPort);
    commandList->RSSetScissorRects(1, &_sissorRect);
}

void RimLightPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_shader->GetRootSignature());
    
    commandList->SetGraphicsRootConstantBufferView(_shader->GetRootParameterIndex("cameraData"), _ownerScene->_cameraBuffer->GetGPUVirtualAddress());
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    commandList->DrawInstanced(4, 1, 0, 0);
}

void RimLightPass::End(ID3D12GraphicsCommandList* commandList)
{
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    _ownerScene->_depthStencilView->TransitionResource(commandList, D3D12_RESOURCE_STATE_PRESENT);
}