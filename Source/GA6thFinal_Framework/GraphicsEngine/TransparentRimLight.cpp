#include "pch.h"
#include "TransparentRimLight.h"
#include "MeshRenderer.h"

TransparentRimLight::TransparentRimLight() = default;

TransparentRimLight::~TransparentRimLight() = default;

UINT TransparentRimLight::GetRootParameterIndex(MeshType meshType, std::string_view name) const
{
    if (meshType == MeshType::STATIC_MESH)
    {
        return _fxStatic.GetRootParameterIndex(name);
    }
    else
    {
        return _fxSkeletal.GetRootParameterIndex(name);
    }
}

void TransparentRimLight::Initialize()
{
    D3D12_RENDER_TARGET_BLEND_DESC rtDesc{};
    rtDesc.SrcBlend              = D3D12_BLEND_ONE;
    rtDesc.DestBlend             = D3D12_BLEND_ZERO;
    rtDesc.BlendOp               = D3D12_BLEND_OP_ADD;
    rtDesc.SrcBlendAlpha         = D3D12_BLEND_ONE;
    rtDesc.DestBlendAlpha        = D3D12_BLEND_ZERO;
    rtDesc.BlendOpAlpha          = D3D12_BLEND_OP_ADD;
    rtDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    PipelineStateStream pss;
    pss.BlendState                            = CD3DX12_BLEND_DESC(CommonStates::NonPremultiplied);
    (&pss.BlendState)->AlphaToCoverageEnable  = FALSE;
    (&pss.BlendState)->IndependentBlendEnable = TRUE;
    (&pss.BlendState)->RenderTarget[1]        = rtDesc;
    (&pss.BlendState)->RenderTarget[2]        = rtDesc;
    (&pss.BlendState)->RenderTarget[3]        = rtDesc;
    pss.RasterizerState                       = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pss.DepthStencilState                     = CD3DX12_DEPTH_STENCIL_DESC(CommonStates::DepthRead);
    pss.DSVFormat                             = DXGI_FORMAT_D32_FLOAT;
    pss.PrimitiveTopology                     = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pss.RTVFormats                            = {{DXGI_FORMAT_R32G32B32A32_FLOAT, // Result
                                                  DXGI_FORMAT_R32G32B32A32_FLOAT, // Normal
                                                  DXGI_FORMAT_R32_UINT},          // CustomDepth
                                                 3};

    _fxStatic.SetPipelineStateStream(pss);
    _pipelineState[0] = Global::pipelineStateManager->GetPipelineState(pss);

    _fxSkeletal.SetPipelineStateStream(pss);
    _pipelineState[1] = Global::pipelineStateManager->GetPipelineState(pss);
}

void TransparentRimLight::SetMaterial(ID3D12GraphicsCommandList* commandList, MeshRenderer* renderer)
{
    const auto& data = std::any_cast<const TransparentRimLightMaterial&>(renderer->GetCustomMaterialData());

    MeshType type = renderer->GetType();

    commandList->SetPipelineState(_pipelineState[type].Get());

    switch (type)
    {
    case STATIC_MESH:
        commandList->SetGraphicsRootSignature(_fxStatic.GetRootSignature());
        commandList->SetGraphicsRoot32BitConstants(_fxStatic.GetRootParameterIndex("bit32_5_rimData"), 5, &data, 0);
        break;
    case SKELETAL_MESH:
        commandList->SetGraphicsRootSignature(_fxSkeletal.GetRootSignature());
        commandList->SetGraphicsRoot32BitConstants(_fxSkeletal.GetRootParameterIndex("bit32_5_rimData"), 5, &data, 0);
        break;
    }    
}