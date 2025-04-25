#include "pch.h"
#include "RenderTechnique.h"
#include "RenderPass.h"

RenderTechnique::RenderTechnique()
{
    _renderPasses.reserve(10);
}

void RenderTechnique::AddRenderPass(std::shared_ptr<RenderPass> pass)
{
    _renderPasses.push_back(pass);
}

void RenderTechnique::Initialize() {}

void RenderTechnique::Execute(ID3D12GraphicsCommandList* commadList)
{
    for (auto& pass : _renderPasses)
    {
        pass->Begin(commadList);
        pass->Draw(commadList);
        pass->End(commadList);
    }
}
