#pragma once
#include "RenderPass.h"

struct timestep
{
    Vector4 color{0, 0, 0, 0};
};

class FadePass : public RenderPass
{
public:
    FadePass();
    virtual ~FadePass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique,
                    ID3D12GraphicsCommandList* commandList) override;
    void Update(ID3D12GraphicsCommandList* commadList) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;


private:
    void InitializeShaderAndPSO();

    bool                           _fadeFlag = false;
    Vector4                        _fadeStart{0, 0, 0, 0};
    Vector4                        _fadeEnd{0, 0, 0, 0};
    Vector4                        _fadeColor{0, 0, 0, 0};
    float                          _fadeDuration = 1.f;
    float                          _fadeTimer;
    float                          _fadeMaintain;
    FX<GE::VS::QUAD, GE::PS::FADE> _fx;
    // std::map<std::string, class Texture*> _fadeMaskTextures;
    

};
