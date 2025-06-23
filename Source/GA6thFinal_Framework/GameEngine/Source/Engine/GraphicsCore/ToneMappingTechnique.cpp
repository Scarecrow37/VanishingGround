#include "pch.h"
//#include "ToneMappingTechnique.h"
//
//ToneMappingTechnique::ToneMappingTechnique() {}
//
//ToneMappingTechnique::~ToneMappingTechnique() {}
//
//void ToneMappingTechnique::Initialize(ID3D12GraphicsCommandList* commandList)
//{
//    const auto&    mode = UmDevice.GetMode();
//    D3D12_VIEWPORT viewport{.Width = (FLOAT)mode.Width, .Height = (FLOAT)mode.Height, .MinDepth = 0.f, .MaxDepth = 1.f};
//    D3D12_RECT     scissor{.right = (LONG)mode.Width, .bottom = (LONG)mode.Height};
//
//    //std::unique_ptr<RenderPass> pass;
//}
//
//void ToneMappingTechnique::Execute(ID3D12GraphicsCommandList* commandList)
//{
//    __super::Execute(commandList);
//}