#include "pch.h"
#include "DXRDrawStaticMeshPass.h"
#include "RenderTarget.h"
DXRDrawStaticMeshPass::~DXRDrawStaticMeshPass() {}

void DXRDrawStaticMeshPass::Initialize(const D3D12_VIEWPORT& viewPort, const D3D12_RECT& sissorRect) 
{
    __super::Initialize(viewPort, sissorRect);
}

void DXRDrawStaticMeshPass::Begin(ID3D12GraphicsCommandList* commandList) {}

void DXRDrawStaticMeshPass::Draw(ID3D12GraphicsCommandList* commandList) {}

void DXRDrawStaticMeshPass::End(ID3D12GraphicsCommandList* commandList) 
{
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void DXRDrawStaticMeshPass::CreateStaticAS(ID3D12GraphicsCommandList* commandList) 
{
}
