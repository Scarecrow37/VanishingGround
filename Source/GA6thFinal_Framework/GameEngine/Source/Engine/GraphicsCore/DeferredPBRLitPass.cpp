#include "pch.h"
#include "DeferredPBRLitPass.h"
#include "RenderTarget.h"
#include "RenderScene.h"
#include "Quad.h"

std::vector<UINT> DeferredPBRLitPass::s_gBufferIndex;

DeferredPBRLitPass::~DeferredPBRLitPass() {}

void DeferredPBRLitPass::Initialize(const D3D12_VIEWPORT& viewPort, const D3D12_RECT& sissorRect)
{
    __super::Initialize(viewPort, sissorRect);

    InitShaderAndPSO();

    static bool isInitialized = false;
    if (!isInitialized)
    {
        const auto& renderTargetGroup = UmMultiRenderTargetManager.GetRenderTargetGroup("GBuffer");
        s_gBufferIndex.reserve(GBuffer::GBUFFER_END);

        for (auto& gBuffer : renderTargetGroup)
        {
            s_gBufferIndex.push_back(gBuffer->GetID());
        }

        isInitialized = true;
    } 
}

void DeferredPBRLitPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
    _meshRenderTarget->ClearRenderTarget(commandList);

    commandList->OMSetRenderTargets(1, &_meshRenderTarget->GetRTVHandle(), FALSE, nullptr);
    commandList->RSSetViewports(1, &_viewPort);
    commandList->RSSetScissorRects(1, &_sissorRect);
}

void DeferredPBRLitPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    auto resource = UmViewManager.GetShaderResourceHeap();

    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_shader->GetRootSignature());

    commandList->SetGraphicsRoot32BitConstants(_shader->GetRootParameterIndex("bit32_3_numLight"), 3, &_ownerScene->_numLight, 0);
    commandList->SetGraphicsRoot32BitConstants(_shader->GetRootParameterIndex("bit32_7_gBufferID"), 7, s_gBufferIndex.data(), 0);
    commandList->SetGraphicsRootConstantBufferView(_shader->GetRootParameterIndex("cameraData"), _ownerScene->_cameraBuffer->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(_shader->GetRootParameterIndex("lightData"), _ownerScene->_lightBuffer->GetGPUVirtualAddress());
    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("textures"), resource->GetGPUDescriptorHandleForHeapStart());

    //quad draw하기
    _ownerScene->_frameQuad->Render(commandList);
}

void DeferredPBRLitPass::End(ID3D12GraphicsCommandList* commandList)
{
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void DeferredPBRLitPass::InitShaderAndPSO()
{
    _shader = std::make_unique<ShaderBuilder>();
    _shader->BeginBuild();
    _shader->SetShader(L"../Shaders/vs_quad.hlsl", ShaderBuilder::Type::VS);
    _shader->SetShader(L"../Shaders/ps_pbr_lighting.hlsl", ShaderBuilder::Type::PS);
    _shader->EndBuild();

    ID3D12Device*                      device = UmDevice.GetDevice();
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psodesc{};
    HRESULT                            hr = S_OK;

    psodesc.RasterizerState               = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psodesc.RasterizerState.CullMode      = D3D12_CULL_MODE_NONE;
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

    hr = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&_pipelineState));
    FAILED_CHECK_MESSAGE(hr, L"DeferredPBRLitPass::InitShaderAndPSO device->CreateGraphicsPipelineState Failed");
}