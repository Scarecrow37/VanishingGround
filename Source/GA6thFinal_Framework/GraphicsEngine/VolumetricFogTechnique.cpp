#include "pch.h"
#include "VolumetricFogTechnique.h"
#include "LightInjectionPass.h"
#include "RenderPass.h"

VolumetricFogTechnique::VolumetricFogTechnique(){}

VolumetricFogTechnique::~VolumetricFogTechnique() {}

void VolumetricFogTechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{
    auto desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT, 240, 135, 6, 1, 1, 0,
                                             D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    // voxeltexture 만들기
    _prevVoxelTexture = std::make_shared<UnorderedAccessView>();
    _currVoxelTexture = std::make_shared<UnorderedAccessView>();
    _prevVoxelTexture->Initialize(desc, D3D12_UAV_DIMENSION_TEXTURE3D, D3D12_SRV_DIMENSION_TEXTURE3D);
    _currVoxelTexture->Initialize(desc, D3D12_UAV_DIMENSION_TEXTURE3D, D3D12_SRV_DIMENSION_TEXTURE3D);

    // 상수 버퍼 init
    _constantBuffer = std::make_shared<ConstantBufferView>();
    UINT size       = (sizeof(VolumetricFogData) + 255) & ~255;
    _constantBuffer->Initialize(size);

    std::unique_ptr<RenderPass> pass = std::make_unique<LightInjectionPass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));
}

void VolumetricFogTechnique::Execute(ID3D12GraphicsCommandList* commandList) 
{
    UpdateConstantBuffer();
    __super::Execute(commandList);
}

void VolumetricFogTechnique::UpdateConstantBuffer()
{
    const auto& volumetricFogProperty =
        std::any_cast<const VolumetricFogProperty&>(_ownerScene->GetRenderPassProperty("VolumetricFogData"));

    // 상수 버퍼 관련 update
    VolumetricFogData fogData;
    fogData.Anisotropy = volumetricFogProperty.Anisotropy;
    fogData.CameraNearFar_FrameIndex_PreviousFrameBlend =
        Vector4(volumetricFogProperty.CustomNear, volumetricFogProperty.CustomFar, _ownerScene->_currentFrameIndex,
                volumetricFogProperty.BlendWithPrevFrame);
    fogData.Density           = volumetricFogProperty.Density;
    fogData.PreViewProjection = _ownerScene->_camera->GetPrevViewProjectionMatrix();
    fogData.Strength          = volumetricFogProperty.Strength;
    fogData.ThicknessFactor   = 0.01f;
    fogData.VolumeSize        = Vector4(VOXEL_VOLUME_SIZEX, VOXEL_VOLUME_SIZEY, VOXEL_VOLUME_SIZEX, 0);
    _constantBuffer->UpdateBuffer(&fogData);
}
