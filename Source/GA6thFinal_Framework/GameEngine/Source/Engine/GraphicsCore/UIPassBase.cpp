#include "pch.h"
#include "UIPassBase.h"
#include "StructuredBuffer.h"
#include "Model.h"

UIPassBase::UIPassBase(const std::vector<UINT>& instanceIDs)
    : _instanceIDs(instanceIDs)
{
}

UIPassBase::~UIPassBase()
{
}

void UIPassBase::Initialize(RenderScene* ownerScene)
{
    __super::Initialize(ownerScene);

    _instanceIDBuffer = std::make_unique<StructuredBuffer>();
    _instanceIDBuffer->Initialize(sizeof(UINT), 1000);

    _quadModel = UmResourceManager.LoadResource<Model>(L"HalfQuad");
    _halfQuad  = _quadModel->GetMeshes().front().get();
}

void UIPassBase::UpdateBuffer(ID3D12GraphicsCommandList* commandList)
{
    _instanceIDBuffer->CopyStructuredBuffer(commandList, (void*)_instanceIDs.data(), (UINT)_instanceIDs.size());
}

void UIPassBase::SetResource(UINT rootParameterIndex, ID3D12GraphicsCommandList* commandList)
{
    commandList->SetGraphicsRootShaderResourceView(rootParameterIndex, _instanceIDBuffer->GetGPUVirtualAddress());
}