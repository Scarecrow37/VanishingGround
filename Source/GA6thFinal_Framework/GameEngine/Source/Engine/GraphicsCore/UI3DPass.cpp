#include "pch.h"
#include "UI3DPass.h"
#include "FrameResource.h"

UI3DPass::UI3DPass(const std::vector<UINT>& instanceIDs)
    : UIPassBase(instanceIDs)
{
}

UI3DPass::~UI3DPass() {}

void UI3DPass::Initialize(RenderScene* ownerScene)
{
    __super::Initialize(ownerScene);

    _shader = std::make_unique<ShaderBuilder>();
    _shader->BeginBuild();
    _shader->SetShader(L"../Shaders/vs_ui_fr.hlsl", ShaderBuilder::Type::VS);
    _shader->SetShader(L"../Shaders/ps_ui.hlsl", ShaderBuilder::Type::PS);
    _shader->EndBuild(ShaderBuilder::BindType::TABLE);

    ID3D12Device* device = UmDevice.GetDevice();

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
    psodesc.RasterizerState                    = CommonStates::CullNone;
    psodesc.BlendState                         = blendDesc;
    psodesc.DepthStencilState                  = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psodesc.DSVFormat                          = _ownerScene->_depthStencilView->GetFormat();
    psodesc.SampleMask                         = UINT_MAX;
    psodesc.PrimitiveTopologyType              = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psodesc.InputLayout                        = _shader->GetInputLayout();
    psodesc.NumRenderTargets                   = 1;
    psodesc.pRootSignature                     = _shader->GetRootSignature();
    psodesc.SampleDesc                         = {1, 0};
    psodesc.VS                                 = _shader->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS                                 = _shader->GetShaderByteCode(ShaderBuilder::Type::PS);

    if constexpr (IS_EDITOR)
    {
        psodesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
    }
    else
    {
        psodesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    }

    HRESULT hr = S_OK;
    hr         = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&_pipelineState));
    FAILED_CHECK_MESSAGE(hr, L"UI3DPass::Initialize device->CreateGraphicsPipelineState Failed");
}

void UI3DPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    const auto& mode = UmDevice.GetMode();

    _cameraData.View       = XMMatrixTranspose(_ownerScene->_camera->GetViewMatrix());
    _cameraData.Projection = XMMatrixTranspose(_ownerScene->_camera->GetProjectionMatrix());
    _cameraBuffer->UpdateBuffer(&_cameraData);

    __super::UpdateBuffer(commandList);

    __super::Begin(commandList);
}