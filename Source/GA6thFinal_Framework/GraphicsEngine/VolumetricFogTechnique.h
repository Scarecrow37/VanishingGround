#pragma once
#include "RenderTechnique.h"

class UnorderedAccessView;
class VolumetricFogTechnique : public RenderTechnique
{
public:
    VolumetricFogTechnique();
    virtual ~VolumetricFogTechnique();
    UnorderedAccessView* GetPrevVoxelTexture() { return _prevVoxelTexture.get(); }
    UnorderedAccessView* GetCurrVoxelTexture() { return _currVoxelTexture.get(); }
    ConstantBufferView*  GetConstantBufferView() { return _constantBuffer.get(); }

public:
    void Initialize(ID3D12GraphicsCommandList* commandList) override;
    void Execute(ID3D12GraphicsCommandList* commandList) override;

private:
    void UpdateConstantBuffer();

private:
    std::shared_ptr<UnorderedAccessView> _prevVoxelTexture;
    std::shared_ptr<UnorderedAccessView> _currVoxelTexture;
    std::shared_ptr<ConstantBufferView>  _constantBuffer;
};
