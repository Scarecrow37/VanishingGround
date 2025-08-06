#include "pch.h"
#include "SSRPass.h"

SSRPass::SSRPass() {}

SSRPass::~SSRPass() {}

void SSRPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique,
                         ID3D12GraphicsCommandList* commandList)
{
    __super::Initialize(ownerScene, ownerTechnique, commandList);
    PipelineStateStream pss;
    pss.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pss.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pss.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    (&pss.DepthStencilState)->DepthEnable = FALSE;
    pss.PrimitiveTopology                 = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pss.RTVFormats                        = {{DXGI_FORMAT_R32G32B32A32_FLOAT}, 1};
    _fxSSR.SetPipelineStateStream(pss);
    _pipelineState = Global::pipelineStateManager->GetPipelineState(pss);
}

void SSRPass::AddRenderPassDatas(std::string_view sceneName) {}

void SSRPass::Begin(ID3D12GraphicsCommandList* commandList) {}

void SSRPass::Draw(ID3D12GraphicsCommandList* commandList) 
{

}

void SSRPass::End(ID3D12GraphicsCommandList* commandList) {}
