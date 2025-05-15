#include "pch.h"
#include "SkyBoxPass.h"
#include "SkyBox.h"

SkyBoxPass::SkyBoxPass() : _skyBox{std::make_unique<SkyBox>()} {}

SkyBoxPass::~SkyBoxPass() {}

void SkyBoxPass::Initialize(const D3D12_VIEWPORT& viewPort, const D3D12_RECT& sissorRect) 
{
    __super::Initialize(viewPort, sissorRect);
    InitShaderAndPSO();
    _skyBox->Initialize(); 
       
    File::Path fileName  = L"../../../Resource/Assets/skybox/kloppenheim_05_puresky_4k.hdr";
    File::Path assetPath = UmFileSystem.GetAssetPath();
    File::Path result    = assetPath / fileName;
    _skyBox->SetTexture(result.string());
}

void SkyBoxPass::Begin(ID3D12GraphicsCommandList* commandList) 
{

}

void SkyBoxPass::End(ID3D12GraphicsCommandList* commandList) {}

void SkyBoxPass::Draw(ID3D12GraphicsCommandList* commandList) {}

void SkyBoxPass::InitShaderAndPSO() {}
