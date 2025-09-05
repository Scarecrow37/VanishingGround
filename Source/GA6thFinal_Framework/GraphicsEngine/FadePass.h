#pragma once
#include "RenderPass.h"

struct timestep
{
    float time = 0;
    Vector4 start{0, 0, 0, 0};
    Vector4 end{0, 0, 0, 0};
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

    void Fade(Vector4& start, Vector4& end, float time);

private:
    void InitializeShaderAndPSO();

    bool                           _fadeFlag = false;
    Vector4                        _fadeStart{0, 0, 0, 0};
    Vector4                        _fadeEnd{0, 0, 0, 0};
    float                          _fadeDuration = 1.f;
    float                          _fadeTimer;
    float                          _fadeStep;
    FX<GE::VS::QUAD, GE::PS::FADE> _fx;
    // std::map<std::string, class Texture*> _fadeMaskTextures;
    

};
