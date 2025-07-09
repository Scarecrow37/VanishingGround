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
    psodesc.RasterizerState                    = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
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

void UI3DPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_shader->GetRootSignature());

    UINT  currentBackBufferIndex = UmDevice.GetCurrentBackBufferIndex();
    auto  resource               = UmViewManager.GetShaderResourceHeap()->GetGPUDescriptorHandleForHeapStart();
    auto& frameResource          = _ownerScene->_frameResources[currentBackBufferIndex];

    frameResource->SetFrameResource(FrameResourceType::UI_TRANSFORM, _shader->GetRootParameterIndex("worldMatrices"), commandList);
    frameResource->SetFrameResource(FrameResourceType::UI_MATERIAL, _shader->GetRootParameterIndex("material"), commandList);

    commandList->SetGraphicsRootShaderResourceView(_shader->GetRootParameterIndex("IDs"), _instanceIDBuffer->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(_shader->GetRootParameterIndex("cameraData"), _ownerScene->_cameraBuffer->GetGPUVirtualAddress());
    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("textures"), resource);

    _halfQuad->Render(commandList, (UINT)_instanceIDs.size());
}