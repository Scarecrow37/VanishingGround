#include "pch.h"
#include "UI2DPass.h"
#include "FrameResource.h"

UI2DPass::UI2DPass(const std::vector<UINT>& instanceIDs)
    : UIPassBase(instanceIDs)
{
}

UI2DPass::~UI2DPass() {}

void UI2DPass::Initialize(RenderScene* ownerScene)
{
    __super::Initialize(ownerScene);

    const auto& mode = UmDevice.GetMode();

    _2DCamera = std::make_unique<Camera>();
    _2DCamera->SetupOrthographic((float)mode.Width, (float)mode.Height, 0.1f, 100.f);

    _cameraBuffer = std::make_unique<ConstantBufferView>();
    _cameraBuffer->Initialize(sizeof(CameraData));

    CameraData cameraData{.View       = XMMatrixIdentity(),
                          .Projection = XMMatrixTranspose(_2DCamera->GetProjectionMatrix())};

    _cameraBuffer->UpdateBuffer(&cameraData);

    _shader = std::make_unique<ShaderBuilder>();
    _shader->BeginBuild();
    _shader->SetShader(L"../Shaders/vs_ui_fr.hlsl", ShaderBuilder::Type::VS);
    _shader->SetShader(L"../Shaders/ps_ui.hlsl", ShaderBuilder::Type::PS);
    _shader->EndBuild(ShaderBuilder::BindType::TABLE);

    ID3D12Device* device = UmDevice.GetDevice();

    D3D12_BLEND_DESC blendDesc                 = {};
    blendDesc.AlphaToCoverageEnable            = FALSE;
    blendDesc.IndependentBlendEnable           = FALSE;

    auto& rtDesc                               = blendDesc.RenderTarget[0];
    rtDesc.BlendEnable                         = TRUE;
    rtDesc.SrcBlend                            = D3D12_BLEND_SRC_ALPHA;
    rtDesc.DestBlend                           = D3D12_BLEND_INV_SRC_ALPHA;
    rtDesc.BlendOp                             = D3D12_BLEND_OP_ADD;
    rtDesc.SrcBlendAlpha                       = D3D12_BLEND_ZERO;
    rtDesc.DestBlendAlpha                      = D3D12_BLEND_ONE;
    rtDesc.BlendOpAlpha                        = D3D12_BLEND_OP_ADD;
    rtDesc.RenderTargetWriteMask               = D3D12_COLOR_WRITE_ENABLE_ALL;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psodesc = {};
    psodesc.RasterizerState                    = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psodesc.BlendState                         = blendDesc;
    psodesc.DepthStencilState                  = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psodesc.DSVFormat                          = _ownerScene->_depthStencilView->GetFormat();
    psodesc.SampleMask                         = UINT_MAX;
    psodesc.PrimitiveTopologyType              = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psodesc.InputLayout                        = _shader->GetInputLayout();
    psodesc.RTVFormats[0]                      = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psodesc.NumRenderTargets                   = 1;
    psodesc.pRootSignature                     = _shader->GetRootSignature();
    psodesc.SampleDesc                         = {1, 0};
    psodesc.VS                                 = _shader->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS                                 = _shader->GetShaderByteCode(ShaderBuilder::Type::PS);

    HRESULT hr = S_OK;
    hr         = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&_pipelineState));
    FAILED_CHECK_MESSAGE(hr, L"UI2DPass::Initialize device->CreateGraphicsPipelineState Failed");
}

void UI2DPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    auto& depthStencilView = _ownerScene->_depthStencilView;

    depthStencilView->TransitionResource(commandList, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    depthStencilView->ClearDepthStencilView(commandList);

    if constexpr (IS_EDITOR)
    {
        _finalRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
        commandList->OMSetRenderTargets(1, &_finalRenderTarget->GetRTVHandle(), FALSE, &depthStencilView->GetDSVHandle());
    }
    else
    {
        commandList->OMSetRenderTargets(1, &UmDevice.GetBackBufferHandle(), FALSE, &depthStencilView->GetDSVHandle());
    }

    commandList->RSSetViewports(1, &_finalRenderTarget->GetViewPort());
    commandList->RSSetScissorRects(1, &_finalRenderTarget->GetScissorRect());

    __super::UpdateBuffer(commandList);
}

void UI2DPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_shader->GetRootSignature());

    UINT  currentBackBufferIndex = UmDevice.GetCurrentBackBufferIndex();
    auto  resource               = UmViewManager.GetShaderResourceHeap()->GetGPUDescriptorHandleForHeapStart();
    auto& frameResource          = _ownerScene->_frameResources[currentBackBufferIndex];

    frameResource->SetFrameResource(FrameResourceType::UI_TRANSFORM, _shader->GetRootParameterIndex("worldMatrices"), commandList);
    frameResource->SetFrameResource(FrameResourceType::UI_MATERIAL, _shader->GetRootParameterIndex("material"), commandList);
    __super::SetResource(_shader->GetRootParameterIndex("IDs"), commandList);
    commandList->SetGraphicsRootConstantBufferView(_shader->GetRootParameterIndex("cameraData"), _cameraBuffer->GetGPUVirtualAddress());
    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("textures"), resource);

    _halfQuad->Render(commandList, (UINT)_instanceIDs.size());
}

void UI2DPass::End(ID3D12GraphicsCommandList* commandList)
{    
    _finalRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}