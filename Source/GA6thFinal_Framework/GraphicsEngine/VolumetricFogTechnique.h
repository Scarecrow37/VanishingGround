#pragma once
#include "RenderTechnique.h"

class UnorderedAccessView;
class VolumetricFogTechnique : public RenderTechnique
{
public:
    VolumetricFogTechnique();
    virtual ~VolumetricFogTechnique();

public:
    ConstantBufferView* GetConstantBufferView() { return _constantBuffer.get(); }
    ConstantBufferView* GetVolumetricFogBufferView() { return _volumetricFogBuffer.get(); }

public:
    void Initialize(ID3D12GraphicsCommandList* commandList) override;
    void Execute(ID3D12GraphicsCommandList* commandList) override;

public:
    std::shared_ptr<UnorderedAccessView> _tempVoxelInjectionTexture3D[2];
    std::shared_ptr<UnorderedAccessView> _finalVoxelAccumulationTexture3D;
    bool                                 _readIndex = false;

private:
    void UpdateConstantBuffer();

private:
    std::shared_ptr<ConstantBufferView>  _constantBuffer;
    std::shared_ptr<ConstantBufferView>  _volumetricFogBuffer;
    XMMATRIX                             _prevViewProjection{XMMatrixIdentity()};
};
