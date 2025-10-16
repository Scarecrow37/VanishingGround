#include "pch.h"
#include "UI25DPass_OIT.h"
#include "UITechnique.h"

UI25DPass_OIT::UI25DPass_OIT(const std::vector<UINT>* instanceIDs)
    : UIPassBase_OIT(instanceIDs)
{
}

UI25DPass_OIT::~UI25DPass_OIT() = default;

void UI25DPass_OIT::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    UIPassBase_OIT::Initialize(ownerScene, ownerTechnique, commandList);

    _cameraBuffer = static_cast<UITechnique*>(_ownerTechnique)->GetCameraBuffer(MODE_25D);
}