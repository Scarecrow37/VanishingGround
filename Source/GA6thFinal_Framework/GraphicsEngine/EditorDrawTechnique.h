#pragma once
#include "RenderTechnique.h"

class EditorDrawTechnique : public RenderTechnique
{
public:
    EditorDrawTechnique();
    virtual ~EditorDrawTechnique();

public:
    void Initialize(ID3D12GraphicsCommandList* commandList) override;
};
