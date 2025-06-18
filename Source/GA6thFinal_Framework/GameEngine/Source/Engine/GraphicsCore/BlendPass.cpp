#include "pch.h"
#include "BlendPass.h"
#include "Quad.h"
#include "RenderScene.h"
#include "RenderTarget.h"
#include "ShaderBuilder.h"

BlendPass::BlendPass() {}

BlendPass::~BlendPass() {}

void BlendPass::Initialize(const D3D12_VIEWPORT& viewPort, const D3D12_RECT& sissorRect)
{
    D3D12_VIEWPORT viewport = viewPort;
    D3D12_RECT     rect     = sissorRect;

    viewport.Width = 1920.f;
    viewport.Height = 1080.f;
    rect.right      = 1920;
    rect.bottom     = 1080;

    __super::Initialize(viewport, rect);

    _shader = std::make_shared<ShaderBuilder>();
    _shader->BeginBuild();
    _shader->SetShader(L"../Shaders/vs_quad.hlsl", ShaderBuilder::Type::VS);
    _shader->SetShader(L"../Shaders/ps_blend.hlsl", ShaderBuilder::Type::PS);
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
    FAILED_CHECK_MESSAGE(hr, L"BlendPass::Initialize device->CreateGraphicsPipelineState Failed");
}

void BlendPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    auto& renderTarget = _ownerScene->_finalTarget;

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

void BlendPass::End(ID3D12GraphicsCommandList* commandList)
{
    auto br = CD3DX12_RESOURCE_BARRIER::Transition(_ownerScene->_finalTarget->GetResource(),
                                                   D3D12_RESOURCE_STATE_RENDER_TARGET,
                                                   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandList->ResourceBarrier(1, &br);
}

void BlendPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    const auto&           mode     = UmDevice.GetMode();
    auto                  resource = UmViewManager.GetShaderResourceHeap();
    ID3D12DescriptorHeap* hps[]    = {
        resource,
    };

    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_shader->GetRootSignature());
    commandList->SetDescriptorHeaps(_countof(hps), hps);

    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("screenTexture"), _ownerScene->_meshLightingTarget->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("grayScaleTexture"), _ownerScene->_renderTargets[0]->GetSRVHandle());

    // quad draw하기
    _ownerScene->_frameQuad->Render(commandList);
}