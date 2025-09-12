#include "pch.h"
#include "UIPassBase.h"
#include "StructuredBuffer.h"
#include "Model.h"
#include "FrameResource.h"

UIPassBase::UIPassBase(const std::vector<UINT>& instanceIDs)
    : _instanceIDs(instanceIDs)
{
}

UIPassBase::~UIPassBase() = default;

void UIPassBase::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    RenderPass::Initialize(ownerScene, ownerTechnique, commandList);

    _instanceIDBuffer = std::make_unique<StructuredBuffer>();
    _instanceIDBuffer->Initialize(sizeof(UINT), 1000);

    _quadModel = Global::resourceManager->LoadResource<Model>(L"HalfQuad");
    _halfQuad  = _quadModel->GetMeshes().front().get();

    _cameraBuffer = std::make_unique<ConstantBufferView>();
    _cameraBuffer->Initialize(sizeof(CameraData));
}

void UIPassBase::Begin(ID3D12GraphicsCommandList* commandList)
{
    auto& depthStencilView = _ownerScene->_depthStencilView;
    depthStencilView->TransitionResource(commandList, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    
    _finalRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandList->OMSetRenderTargets(1, &_finalRenderTarget->GetRTVHandle(), FALSE, &depthStencilView->GetDSVHandle());

    commandList->RSSetViewports(1, &_finalRenderTarget->GetViewport());
    commandList->RSSetScissorRects(1, &_finalRenderTarget->GetScissorRect());
}

void UIPassBase::Draw(ID3D12GraphicsCommandList* commandList)
{
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_shader->GetRootSignature());

    UINT  currentBackBufferIndex = Global::device->GetCurrentBackBufferIndex();
    auto  resource               = Global::viewManager->GetShaderResourceHeap()->GetGPUDescriptorHandleForHeapStart();
    auto& frameResource          = _ownerScene->_frameResources[currentBackBufferIndex];

    frameResource->SetFrameResource(FrameResourceType::UI_TRANSFORM, _shader->GetRootParameterIndex("matrices"), commandList);
    frameResource->SetFrameResource(FrameResourceType::UI_MATERIAL, _shader->GetRootParameterIndex("material"), commandList);

    commandList->SetGraphicsRootShaderResourceView(_shader->GetRootParameterIndex("IDs"), _instanceIDBuffer->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(_shader->GetRootParameterIndex("cameraData"), _cameraBuffer->GetGPUVirtualAddress());
    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("textures"), resource);

    _halfQuad->Render(commandList, (UINT)_instanceIDs.size());
}

void UIPassBase::End(ID3D12GraphicsCommandList* commandList)
{    
    _finalRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void UIPassBase::UpdateBuffer(ID3D12GraphicsCommandList* commandList)
{
    _instanceIDBuffer->CopyStructuredBuffer(commandList, (void*)_instanceIDs.data(), (UINT)_instanceIDs.size());
}