#include "pch.h"
#include "DeferredPBRLitPass.h"
#include "Shader.h"

DeferredPBRLitPass::~DeferredPBRLitPass() {}

void DeferredPBRLitPass::Initialize(const D3D12_VIEWPORT& viewPort, const D3D12_RECT& sissorRect)
{
    __super::Initialize(viewPort, sissorRect);
}

void DeferredPBRLitPass::Begin(ID3D12GraphicsCommandList* commandList) {}

void DeferredPBRLitPass::End(ID3D12GraphicsCommandList* commandList) {}

void DeferredPBRLitPass::Draw(ID3D12GraphicsCommandList* commandList) {}

void DeferredPBRLitPass::InitShaderAndPSO()
{
    _shader = std::make_shared<Shader>();
    _shader->BeginBuild();
    //_shader->
}
