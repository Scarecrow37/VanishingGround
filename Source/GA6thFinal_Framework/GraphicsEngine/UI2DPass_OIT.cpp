#include "pch.h"
#include "UI2DPass_OIT.h"
#include "UITechnique.h"

UI2DPass_OIT::UI2DPass_OIT(const std::vector<UINT>* instanceIDs)
    : UIPassBase_OIT(instanceIDs)
{
}

UI2DPass_OIT::~UI2DPass_OIT() = default;

void UI2DPass_OIT::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    UIPassBase_OIT::Initialize(ownerScene, ownerTechnique, commandList);    
    
    _cameraBuffer = static_cast<UITechnique*>(_ownerTechnique)->GetCameraBuffer(MODE_2D);
}