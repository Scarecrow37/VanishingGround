#pragma once
#include "RenderPass.h"
class FadePass : public RenderPass
{
public:
    FadePass();
    virtual ~FadePass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique,
                    ID3D12GraphicsCommandList* commandList) override;
    void Update(ID3D12GraphicsCommandList* commadList, const float deltaTime) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;


private:
    void InitializeShaderAndPSO();

    bool                           _fadeFlag = false;
    Vector4                        _fadeColor{0, 0, 0, 0};
    FX<GE::VS::QUAD, GE::PS::FADE> _fx;
    // std::map<std::string, class Texture*> _fadeMaskTextures;
    

};
