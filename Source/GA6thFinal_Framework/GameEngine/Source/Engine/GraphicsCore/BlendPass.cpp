#include "pch.h"
#include "BlendPass.h"
#include "Quad.h"
#include "RenderScene.h"
#include "RenderTarget.h"
#include "UnorderedAccessView.h"

BlendPass::BlendPass() {}

BlendPass::~BlendPass() {}

void BlendPass::Initialize(const D3D12_VIEWPORT& viewPort, const D3D12_RECT& sissorRect)
{
    __super::Initialize(viewPort, sissorRect);

    _shader = std::make_unique<ShaderBuilder>();
    _shader->BeginBuild();
    _shader->SetShader(L"../Shaders/vs_quad.hlsl", ShaderBuilder::Type::VS);
    _shader->SetShader(L"../Shaders/ps_blend.hlsl", ShaderBuilder::Type::PS);
    _shader->EndBuild(ShaderBuilder::BindType::DIRECT);

    ID3D12Device*                      device = UmDevice.GetDevice();
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psodesc = {};

    psodesc.RasterizerState               = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psodesc.BlendState                    = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psodesc.DepthStencilState             = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psodesc.DepthStencilState.DepthEnable = FALSE;
    psodesc.SampleMask                    = UINT_MAX;
    psodesc.PrimitiveTopologyType         = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psodesc.InputLayout                   = _shader->GetInputLayout();
    psodesc.NumRenderTargets              = 1;
    psodesc.pRootSignature                = _shader->GetRootSignature();
    psodesc.SampleDesc                    = {1, 0};
    psodesc.VS                            = _shader->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS                            = _shader->GetShaderByteCode(ShaderBuilder::Type::PS);

    if constexpr (IS_EDITOR)
    {
        psodesc.RTVFormats[0]                 = DXGI_FORMAT_R32G32B32A32_FLOAT;
    }
    else
    {        
        psodesc.RTVFormats[0]                 = DXGI_FORMAT_R8G8B8A8_UNORM;
    }

    HRESULT hr = S_OK;
    hr         = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&_pipelineState));
    FAILED_CHECK_MESSAGE(hr, L"BlendPass::Initialize device->CreateGraphicsPipelineState Failed");
}

void BlendPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    if constexpr (IS_EDITOR)
    {
        _finalRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
        _finalRenderTarget->ClearRenderTarget(commandList);
        commandList->OMSetRenderTargets(1, &_finalRenderTarget->GetRTVHandle(), FALSE, nullptr);
    }
    else
    {
        UmDevice.SetBackBuffer();
    }

    _ownerScene->_accumulationBuffer->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    commandList->RSSetViewports(1, &_viewPort);
    commandList->RSSetScissorRects(1, &_sissorRect);
}

void BlendPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_shader->GetRootSignature());

    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("screenTexture"), _meshRenderTarget->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("sourceTexture"), _ownerScene->_accumulationBuffer->GetSRVHandle());

    _ownerScene->_frameQuad->Render(commandList);
}

void BlendPass::End(ID3D12GraphicsCommandList* commandList)
{
    if constexpr (IS_EDITOR)
    {
        _finalRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }
}