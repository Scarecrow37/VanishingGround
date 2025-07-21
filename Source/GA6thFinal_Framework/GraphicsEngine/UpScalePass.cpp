#include "pch.h"
#include "UpScalePass.h"

UpScalePass::UpScalePass() {}

UpScalePass::~UpScalePass() {}

void UpScalePass::Initialize(RenderScene* ownerScene, ID3D12GraphicsCommandList* commandList)
{
    __super::Initialize(ownerScene, commandList);

    _shader = std::make_unique<ShaderBuilder>();
    _shader->BeginBuild();
    _shader->SetShader(L"../Shaders/vs_quad.hlsl", ShaderBuilder::Type::VS);
    _shader->SetShader(L"../Shaders/ps_up_scale.hlsl", ShaderBuilder::Type::PS);
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
    FAILED_CHECK_MESSAGE(hr, L"UpScalePass::Initialize device->CreateGraphicsPipelineState Failed");
}

void UpScalePass::Begin(ID3D12GraphicsCommandList* commandList)
{
    auto renderTarget = Global::multiRenderTargetManager->GetAvailableRenderTarget();
    renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    renderTarget->ClearRenderTarget(commandList);

    commandList->OMSetRenderTargets(1, &renderTarget->GetRTVHandle(), TRUE, nullptr);
    commandList->RSSetViewports(1, &renderTarget->GetViewPort());
    commandList->RSSetScissorRects(1, &renderTarget->GetScissorRect());
}

void UpScalePass::Draw(ID3D12GraphicsCommandList* commandList)
{
    const auto& mipmapTarget = Global::multiRenderTargetManager->GetRenderTargetGroup("Mipmap");

    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_shader->GetRootSignature());
    
    commandList->SetGraphicsRoot32BitConstants(_shader->GetRootParameterIndex("bit32_1_numTextures"), 1, &MAX_MIPMAP_LEVEL, 0);
    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("textures"), mipmapTarget[0]->GetSRVHandle());

    _ownerScene->_frameQuad->Render(commandList);
}

void UpScalePass::End(ID3D12GraphicsCommandList* commandList)
{
    const auto& renderTargets = Global::multiRenderTargetManager->GetUsedRenderTargets();
    auto        renderTarget  = renderTargets.front();
    renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}