#include "pch.h"
#include "VolumetricFogTechnique.h"
#include "FogLightInjectionPass.h"
#include "FogLightAccmulatePass.h"
#include "FogCompositePass.h"
#include "RenderPass.h"

VolumetricFogTechnique::VolumetricFogTechnique() = default;

VolumetricFogTechnique::~VolumetricFogTechnique() = default;

void VolumetricFogTechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{
    auto desc = CD3DX12_RESOURCE_DESC::Tex3D(DXGI_FORMAT_R32G32B32A32_FLOAT, 
                                             VOXEL_VOLUME_SIZEX, 
                                             VOXEL_VOLUME_SIZEY,
                                             VOXEL_VOLUME_SIZEZ, // 3D 깊이
                                             1,                  // mipLevels
                                             D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    // voxeltexture 만들기
    _tempVoxelInjectionTexture3D[0] = std::make_shared<UnorderedAccessView>();
    _tempVoxelInjectionTexture3D[1] = std::make_shared<UnorderedAccessView>();
    _finalVoxelAccumulationTexture3D = std::make_shared<UnorderedAccessView>();
    _tempVoxelInjectionTexture3D[0]->InitializeAsTexture(desc, UnorderedAccessView::UAVSliceType::PER_MIP, true, D3D12_SRV_DIMENSION_TEXTURE3D);
    _tempVoxelInjectionTexture3D[1]->InitializeAsTexture(desc, UnorderedAccessView::UAVSliceType::PER_MIP, true, D3D12_SRV_DIMENSION_TEXTURE3D);
    _finalVoxelAccumulationTexture3D->InitializeAsTexture(desc, UnorderedAccessView::UAVSliceType::PER_MIP, true, D3D12_SRV_DIMENSION_TEXTURE3D);
    // 상수 버퍼 init
    _constantBuffer = std::make_shared<ConstantBufferView>();
    UINT size       = (sizeof(VolumetricFogData) + 255) & ~255;
    _constantBuffer->Initialize(size);

    _volumetricFogBuffer = std::make_shared<ConstantBufferView>();
    size                 = (sizeof(VolumetricFogCompositeData) + 255) & ~255;
    _volumetricFogBuffer->Initialize(size);

    std::unique_ptr<RenderPass> pass;
    pass = std::make_unique<FogLightInjectionPass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));

    pass = std::make_unique<FogLightAccmulatePass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));

    pass = std::make_unique<FogCompositePass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));
}

void VolumetricFogTechnique::Execute(ID3D12GraphicsCommandList* commandList) 
{   
    UpdateConstantBuffer();
    RenderTechnique::Execute(commandList);

    // 0->1 or 1->0
    _readIndex = !_readIndex;
}

void VolumetricFogTechnique::UpdateConstantBuffer()
{
    const auto& volumetricFogProperty = std::any_cast<const VolumetricFogProperty&>(Global::renderPassDatas->GetRenderPassProperty("VolumetricFogData"));

    XMMATRIX view = _ownerScene->_camera->GetViewMatrix();
    XMMATRIX proj = _ownerScene->_camera->GetProjectionMatrix();
    XMMATRIX viewProj = XMMatrixMultiply(view, proj);
    XMMATRIX invViewProj = XMMatrixInverse(nullptr, viewProj);

    // 상수 버퍼 관련 update
    VolumetricFogData fogData;
    fogData.FogAnisotropy                    = volumetricFogProperty.FogAnisotropy;
    fogData.LightShaftAnisotropy             = volumetricFogProperty.LightShaftAnisotropy;
    fogData.CameraNearFar_PreviousFrameBlend = Vector4(
        volumetricFogProperty.CustomNear, volumetricFogProperty.CustomFar, volumetricFogProperty.BlendWithPrevFrame, 1);
    fogData.Density               = volumetricFogProperty.Density;
    fogData.PreViewProjection     = XMMatrixTranspose(_prevViewProjection);
    fogData.InverseViewProjection = XMMatrixTranspose(invViewProj);
    fogData.Strength              = volumetricFogProperty.Strength;
    fogData.ThicknessFactor       = 0.01f;
    fogData.VolumeSize            = Vector4(VOXEL_VOLUME_SIZEX, VOXEL_VOLUME_SIZEY, VOXEL_VOLUME_SIZEZ, 0);
    fogData.FogIntensity          = volumetricFogProperty.FogIntensity;
    fogData.LightShaftIntensity   = volumetricFogProperty.LightShaftIntensity;
    fogData.FogColor              = Vector4(volumetricFogProperty.FogColor[0], volumetricFogProperty.FogColor[1],
                                            volumetricFogProperty.FogColor[2], 1);
    _constantBuffer->UpdateBuffer(&fogData);


    VolumetricFogCompositeData compositeData;
    compositeData.BlendWithScene = volumetricFogProperty.BlendWithScene;
    compositeData.CameraNearFar  = Vector4(volumetricFogProperty.CustomNear, volumetricFogProperty.CustomFar, (float)_ownerScene->_currentFrameIndex, 1.f);
    compositeData.ViewProj       = XMMatrixTranspose(viewProj);
    compositeData.VoxelSize      = fogData.VolumeSize;
    compositeData.InverseViewProjection = XMMatrixTranspose(invViewProj);
    _volumetricFogBuffer->UpdateBuffer(&compositeData);
    _prevViewProjection = viewProj;
}
