#include "pch.h"
#include "DownScalePass.h"
#include "Quad.h"
#include "RenderScene.h"
#include "RenderTarget.h"

DownScalePass::DownScalePass() {}

DownScalePass::~DownScalePass() {}

void DownScalePass::Initialize(const D3D12_VIEWPORT& viewPort, const D3D12_RECT& sissorRect)
{
    __super::Initialize(viewPort, sissorRect);

    _shader = std::make_unique<ShaderBuilder>();
    _shader->BeginBuild();
    _shader->SetShader(L"../Shaders/vs_quad.hlsl", ShaderBuilder::Type::VS);
    _shader->SetShader(L"../Shaders/ps_down_scale.hlsl", ShaderBuilder::Type::PS);
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
    psodesc.NumRenderTargets              = 1;
    psodesc.RTVFormats[0]                 = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psodesc.pRootSignature                = _shader->GetRootSignature();
    psodesc.SampleDesc                    = {1, 0};
    psodesc.VS                            = _shader->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS                            = _shader->GetShaderByteCode(ShaderBuilder::Type::PS);

    HRESULT hr = S_OK;
    hr         = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&_pipelineState));
    FAILED_CHECK_MESSAGE(hr, L"DownScalePass::Initialize device->CreateGraphicsPipelineState Failed");
}

void DownScalePass::Begin(ID3D12GraphicsCommandList* commandList)
{
    const auto& mipmapTarget = UmMultiRenderTargetManager.GetRenderTargetGroup("Mipmap");

    for (UINT i = 0; i < 4; i++)
    {
        mipmapTarget[i]->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
        mipmapTarget[i]->ClearRenderTarget(commandList);
    }
}

void DownScalePass::End(ID3D12GraphicsCommandList* commandList)
{
    auto&       multiRenderTargetManager = UmMultiRenderTargetManager;
    const auto& mipmapTarget             = multiRenderTargetManager.GetRenderTargetGroup("Mipmap");
    for (UINT i = 0; i < 4; i++)
    {
        mipmapTarget[i]->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }

    const auto& usedRenderTargets = multiRenderTargetManager.GetUsedRenderTargets();
    multiRenderTargetManager.ReturnRenderTarget(usedRenderTargets.front().get());
}

void DownScalePass::Draw(ID3D12GraphicsCommandList* commandList)
{
    const auto&           mode     = UmDevice.GetMode();
    auto                  resource = UmViewManager.GetShaderResourceHeap();
    ID3D12DescriptorHeap* hps[]    = { resource, };

    auto&       multiRenderTargetManager = UmMultiRenderTargetManager;
    const auto& mipmapTarget             = multiRenderTargetManager.GetRenderTargetGroup("Mipmap");
    const auto& usedRenderTargets        = multiRenderTargetManager.GetUsedRenderTargets();

    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_shader->GetRootSignature());
    commandList->SetDescriptorHeaps(_countof(hps), hps);

    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("brightExtractTexture"), usedRenderTargets.front()->GetSRVHandle());
    
    D3D12_VIEWPORT viewPort = {0.f, 0.f, 1024.f, 1024.f};
    D3D12_RECT     sissorRect = {0, 0, 1024, 1024};

    // Down sample 4번
    for (UINT i = 0; i < 4; i++)
    {
        commandList->OMSetRenderTargets(1, &mipmapTarget[i]->GetRTVHandle(), NULL, nullptr);
        commandList->RSSetViewports(1, &viewPort);
        commandList->RSSetScissorRects(1, &sissorRect);

        _ownerScene->_frameQuad->Render(commandList);

        viewPort.Width *= 0.5f;
        viewPort.Height *= 0.5f;

        sissorRect.right >>= 1;
        sissorRect.bottom >>= 1;
    }
}