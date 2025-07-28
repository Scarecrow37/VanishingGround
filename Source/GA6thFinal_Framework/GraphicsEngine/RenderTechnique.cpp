#include "pch.h"
#include "RenderTechnique.h"
#include "RenderPass.h"

RenderTechnique::RenderTechnique()
{
    _renderPasses.reserve(10);
}

void RenderTechnique::AddRenderPass(std::unique_ptr<RenderPass> pass)
{
    _renderPasses.push_back(std::move(pass));
}

void RenderTechnique::Update(ID3D12GraphicsCommandList* commandList)
{
    for (auto& pass : _renderPasses)
    {
        pass->Update(commandList);
    }
}

void RenderTechnique::Execute(ID3D12GraphicsCommandList* commandList)
{
    for (auto& pass : _renderPasses)
    {
        pass->Begin(commandList);
        pass->Draw(commandList);
        pass->End(commandList);
    }
}