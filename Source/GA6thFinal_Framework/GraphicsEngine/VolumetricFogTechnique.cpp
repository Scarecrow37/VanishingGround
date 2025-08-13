#include "pch.h"
#include "VolumetricFogTechnique.h"
#include "RenderPass.h"

VolumetricFogTechnique::VolumetricFogTechnique(){}

VolumetricFogTechnique::~VolumetricFogTechnique() {}

void VolumetricFogTechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{
    auto desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT, 240, 135, 6, 1, 1, 0,
                                             D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    _prevVoxelTexture->Initialize(desc, D3D12_UAV_DIMENSION_TEXTURE3D, D3D12_SRV_DIMENSION_TEXTURE3D);
    _currVoxelTexture->Initialize(desc, D3D12_UAV_DIMENSION_TEXTURE3D, D3D12_SRV_DIMENSION_TEXTURE3D);
}

void VolumetricFogTechnique::Execute(ID3D12GraphicsCommandList* commandList) 
{
    __super::Execute(commandList);
}
