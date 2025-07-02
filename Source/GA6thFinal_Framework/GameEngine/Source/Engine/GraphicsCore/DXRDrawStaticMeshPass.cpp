#include "pch.h"
#include "DXRDrawStaticMeshPass.h"
#include "RenderTarget.h"
#include "RenderScene.h"

DXRDrawStaticMeshPass::~DXRDrawStaticMeshPass() {}

void DXRDrawStaticMeshPass::Initialize() 
{
    __super::Initialize();
}

void DXRDrawStaticMeshPass::Begin(ID3D12GraphicsCommandList* commandList) 
{
    UmAccelerationStructureManager.RemoveUnUsedStaticMeshes(_ownerScene->_staticMesh);
}

void DXRDrawStaticMeshPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    for (auto* renderer : _ownerScene->_staticMesh)
    {
        UmAccelerationStructureManager.SubmitInstance(renderer);
    }
}

void DXRDrawStaticMeshPass::End(ID3D12GraphicsCommandList* commandList) 
{

    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void DXRDrawStaticMeshPass::CreateStaticAS(ID3D12GraphicsCommandList* commandList) 
{
}
