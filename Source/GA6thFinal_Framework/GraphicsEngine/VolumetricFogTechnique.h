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

public :
    void Initialize(ID3D12GraphicsCommandList* commandList) override;
    void Execute(ID3D12GraphicsCommandList* commandList) override;

private:
    std::unique_ptr<UnorderedAccessView> _prevVoxelTexture;
    std::unique_ptr<UnorderedAccessView> _currVoxelTexture;
};
