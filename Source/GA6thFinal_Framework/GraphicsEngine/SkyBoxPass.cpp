#include "pch.h"
#include "SkyBoxPass.h"
#include "SkyBox.h"

SkyBoxPass::SkyBoxPass() {}

SkyBoxPass::~SkyBoxPass() {}

void SkyBoxPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    __super::Initialize(ownerScene, ownerTechnique, commandList);

    _skyBox = _ownerScene->GetSkyBox();
    InitShaderAndPSO();
}

void SkyBoxPass::Begin(ID3D12GraphicsCommandList* commandList) 
{
    commandList->OMSetRenderTargets(1, &_meshRenderTarget->GetRTVHandle(), FALSE, nullptr);
    commandList->RSSetViewports(1, &_meshRenderTarget->GetViewport());
    commandList->RSSetScissorRects(1, &_meshRenderTarget->GetScissorRect());
}

void SkyBoxPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    bool isActive = _skyBox->HasEnvTexture();

    if (isActive)
    {
        commandList->SetGraphicsRootSignature(_fx.GetRootSignature());
        commandList->SetGraphicsRootConstantBufferView(_fx.GetRootParameterIndex("cameraData"), _ownerScene->_cameraBuffer->GetGPUVirtualAddress());
        commandList->SetPipelineState(_pipelineState.Get());
        _skyBox->Render(commandList, _fx.GetRootParameterIndex("evnTexture"));
    }
}

void SkyBoxPass::End(ID3D12GraphicsCommandList* commandList) 
{
}

void SkyBoxPass::InitShaderAndPSO() 
{
    PipelineStateStream pss;
    pss.BlendState                        = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pss.RasterizerState                   = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pss.DepthStencilState                 = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    (&pss.DepthStencilState)->DepthEnable = FALSE;
    pss.PrimitiveTopology                 = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pss.RTVFormats                        = {{DXGI_FORMAT_R32G32B32A32_FLOAT}, 1};

    _fx.SetPipelineStateStream(pss);
    _pipelineState = Global::pipelineStateManager->GetPipelineState(pss);    
}
