#include "pch.h"
#include "UI25DPass.h"
#include "FrameResource.h"

UI25DPass::UI25DPass(const std::vector<UINT>& instanceIDs)
    : UIPassBase(instanceIDs)
{
}

UI25DPass::~UI25DPass() {}

void UI25DPass::Initialize(RenderScene* ownerScene, ID3D12GraphicsCommandList* commandList)
{
    __super::Initialize(ownerScene, commandList);

    _cameraData.View = XMMatrixTranspose(XMMatrixLookAtLH({0.f, 0.f, -1.f}, {0.f, 0.f, 1.f}, {0.f, 1.f, 0.f}));

    _shader = std::make_unique<ShaderBuilder>();
    _shader->BeginBuild();
    _shader->SetShader(L"../Shaders/vs_ui_fr.hlsl", ShaderBuilder::Type::VS);
    _shader->SetShader(L"../Shaders/ps_ui.hlsl", ShaderBuilder::Type::PS);
    _shader->EndBuild(ShaderBuilder::BindType::TABLE);

    ID3D12Device* device = Global::device->GetDevice();

    D3D12_BLEND_DESC blendDesc       = {};
    blendDesc.AlphaToCoverageEnable  = FALSE;
    blendDesc.IndependentBlendEnable = FALSE;

    auto& rtDesc                 = blendDesc.RenderTarget[0];
    rtDesc.BlendEnable           = TRUE;
    rtDesc.SrcBlend              = D3D12_BLEND_SRC_ALPHA;
    rtDesc.DestBlend             = D3D12_BLEND_INV_SRC_ALPHA;
    rtDesc.BlendOp               = D3D12_BLEND_OP_ADD;
    rtDesc.SrcBlendAlpha         = D3D12_BLEND_ZERO;
    rtDesc.DestBlendAlpha        = D3D12_BLEND_ONE;
    rtDesc.BlendOpAlpha          = D3D12_BLEND_OP_ADD;
    rtDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psodesc = {};
    psodesc.RasterizerState                    = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psodesc.BlendState                         = blendDesc;
    psodesc.DepthStencilState                  = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psodesc.DSVFormat                          = _ownerScene->_depthStencilView->GetFormat();
    psodesc.SampleMask                         = UINT_MAX;
    psodesc.PrimitiveTopologyType              = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psodesc.InputLayout                        = _shader->GetInputLayout();
    psodesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psodesc.NumRenderTargets                   = 1;
    psodesc.pRootSignature                     = _shader->GetRootSignature();
    psodesc.SampleDesc                         = {1, 0};
    psodesc.VS                                 = _shader->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS                                 = _shader->GetShaderByteCode(ShaderBuilder::Type::PS);    

    HRESULT hr = S_OK;
    hr         = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&_pipelineState));
    FAILED_CHECK_MESSAGE(hr, L"UI25DPass::Initialize device->CreateGraphicsPipelineState Failed");
}

void UI25DPass::Begin(ID3D12GraphicsCommandList* commandList)
{    
    _cameraData.Projection = XMMatrixTranspose(_ownerScene->_camera->GetProjectionMatrix());
    _cameraBuffer->UpdateBuffer(&_cameraData);
    
    __super::UpdateBuffer(commandList);

    __super::Begin(commandList);
}