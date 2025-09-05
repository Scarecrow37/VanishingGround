#pragma once
#include "RenderTechnique.h"
class SceneTransitionTechnique : public RenderTechnique
{
public:
    SceneTransitionTechnique();
    virtual ~SceneTransitionTechnique();

public:
    void Initialize(ID3D12GraphicsCommandList* commandList) override;


};
