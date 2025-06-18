#include "pch.h"
#include "BrightExtractPass.h"
#include "Quad.h"
#include "RenderScene.h"
#include "RenderTarget.h"
#include "ShaderBuilder.h"

BrightExtractPass::BrightExtractPass() {}

BrightExtractPass::~BrightExtractPass() {}

void BrightExtractPass::Initialize(const D3D12_VIEWPORT& viewPort, const D3D12_RECT& sissorRect)
{
    __super::Initialize(viewPort, sissorRect);

    _shader = std::make_shared<ShaderBuilder>();
    _shader->BeginBuild();
    _shader->SetShader(L"../Shaders/vs_quad.hlsl", ShaderBuilder::Type::VS);
    _shader->SetShader(L"../Shaders/ps_bright_extract.hlsl", ShaderBuilder::Type::PS);
    _shader->EndBuild(ShaderBuilder::BindType::DIRECT);

    ID3D12Device*                      device = UmDevice.GetDevice();
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psodesc{};
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
    
    HRESULT hr = S_OK;
    hr         = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&_pipelineState));
    FAILED_CHECK_MESSAGE(hr, L"BrightExtractPass::Initialize device->CreateGraphicsPipelineState Failed");


}

void BrightExtractPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    auto& renderTarget = _ownerScene->_renderTargets[0];

    CD3DX12_RESOURCE_BARRIER br = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandList->ResourceBarrier(1, &br);

    float                       clearValue = renderTarget->clearValue;
    Color                       clearColor = {clearValue, clearValue, clearValue, 1.f};
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle  = renderTarget->GetRTVHandle();
    commandList->ClearRenderTargetView(cpuHandle, clearColor, 0, nullptr);

    commandList->OMSetRenderTargets(1, &renderTarget->GetRTVHandle(), FALSE, nullptr);
    commandList->RSSetViewports(1, &_viewPort);
    commandList->RSSetScissorRects(1, &_sissorRect);
}

void BrightExtractPass::End(ID3D12GraphicsCommandList* commandList)
{
    ID3D12Resource* rt = _ownerScene->_renderTargets[0]->GetResource();
    auto            br = CD3DX12_RESOURCE_BARRIER::Transition(rt, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandList->ResourceBarrier(1, &br);
}

void BrightExtractPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    const auto&           mode     = UmDevice.GetMode();
    auto                  resource = UmViewManager.GetShaderResourceHeap();
    ID3D12DescriptorHeap* hps[]    = {
        resource,
    };

    PostProcessData postProcessData{
        .ScreenSize      = {(float)mode.Width, (float)mode.Height},
        .PostProcessMask = PostProcess::BLOOM,
    };

    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_shader->GetRootSignature());
    commandList->SetDescriptorHeaps(_countof(hps), hps);

    commandList->SetGraphicsRoot32BitConstants(_shader->GetRootParameterIndex("bit32_5_postProcessData"), 5, &postProcessData, 0);
    // commandList->SetGraphicsRoot32BitConstants(_shader->GetRootParameterIndex("bit32_1_threshold"), 1, _ownerScene->_gBufferIndex.data(), 0);
    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("screenTexture"), _ownerScene->_meshLightingTarget->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("customDepthTexture"), _ownerScene->_gBuffer[RenderScene::GBuffer::CUSTOMDEPTH]->GetSRVHandle());

    // quad draw하기
    _ownerScene->_frameQuad->Render(commandList);
}