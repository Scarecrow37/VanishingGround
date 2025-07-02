#include "pch.h"
#include "RTPipelineBuilder.h"

void RTPipelineBuilder::AddShader(const ShaderEntry& shader) 
{
    _shaders.push_back(shader);
}

void RTPipelineBuilder::AddHitGroup(const HitGroupDesc& hitGroup)
{
    _hitGroups.push_back(hitGroup);
}

void RTPipelineBuilder::SetRayGenRootSignature(const D3D12_ROOT_SIGNATURE_DESC& root) 
{
    _rayGenRootDesc = root;
}

void RTPipelineBuilder::SetHitGroupRootSignature(const D3D12_ROOT_SIGNATURE_DESC& root) 
{
    _hitGroupRootDesc = root;
}
//
//ComPtr<ID3D12StateObject> RTPipelineBuilder::BuildPipelineStateObject(ID3D12Device5* device)
//{
//    return ComPtr<ID3D12StateObject>();
//}
