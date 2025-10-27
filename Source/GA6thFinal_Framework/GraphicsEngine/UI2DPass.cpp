#include "pch.h"
#include "UI2DPass.h"
#include "UITechnique.h"

UI2DPass::UI2DPass(const std::vector<UINT>* instanceIDs)
    : UIPassBase(instanceIDs)
{
}

UI2DPass::~UI2DPass() = default;

void UI2DPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    UIPassBase::Initialize(ownerScene, ownerTechnique, commandList);        

    _cameraBuffer = static_cast<UITechnique*>(_ownerTechnique)->GetCameraBuffer(MODE_2D);
}