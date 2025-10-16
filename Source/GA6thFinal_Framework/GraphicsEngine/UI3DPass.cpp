#include "pch.h"
#include "UI3DPass.h"
#include "UITechnique.h"

UI3DPass::UI3DPass(const std::vector<UINT>* instanceIDs)
    : UIPassBase(instanceIDs)
{
}

UI3DPass::~UI3DPass() = default;

void UI3DPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    UIPassBase::Initialize(ownerScene, ownerTechnique, commandList);

    _cameraBuffer = static_cast<UITechnique*>(_ownerTechnique)->GetCameraBuffer(MODE_3D);
}