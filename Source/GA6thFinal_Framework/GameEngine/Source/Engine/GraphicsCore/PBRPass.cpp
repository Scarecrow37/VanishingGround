#include "pch.h"
#include "PBRPass.h"
#include "FrameResource.h"
#include "RenderScene.h"
#include "Shader.h"
#include "TempObject.h" 
#include "Model.h"
#include "UmScripts.h"
#include "BaseMesh.h"

PBRPass::~PBRPass() {}

void PBRPass::Initialize(const D3D12_VIEWPORT& viewPort, const D3D12_RECT& sissorRect)
{
    __super::Initialize(viewPort, sissorRect);
}

void PBRPass::Begin(ComPtr<ID3D12GraphicsCommandList> commandList)
{
    __super::Begin(commandList);
}

void PBRPass::End(ComPtr<ID3D12GraphicsCommandList> commandList)
{
    __super::End(commandList);
}

void PBRPass::Draw(ComPtr<ID3D12GraphicsCommandList> commandList)
{
    UINT                         currentBackBufferIndex = UmDevice.GetCurrentBackBufferIndex();
    ComPtr<ID3D12DescriptorHeap> dh    = _ownerScene->_frameResources[currentBackBufferIndex]->GetDescriptorHeap();
    ID3D12DescriptorHeap*        hps[] = {
        dh.Get(),
    };

    // 디스크립터-힙 설정.
    commandList->SetDescriptorHeaps(_countof(hps), hps);

    // 디스크립터-힙에서 첫번째 디스크립터 (배열)주소 획득.
    D3D12_GPU_DESCRIPTOR_HANDLE textures = dh->GetGPUDescriptorHandleForHeapStart();

    // ID3DR
    //  현재 타겟의 카메라 버퍼 설정
    commandList->SetGraphicsRootConstantBufferView(_shader->GetRootSignatureIndex("cameraData"),
                                                   _ownerScene->_cameraBuffer->GetGPUVirtualAddress());

    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootSignatureIndex("objectData"), textures);
    textures.ptr += UmDevice.GetCBVSRVUAVDescriptorSize();

    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootSignatureIndex("material"), textures);
    textures.ptr += UmDevice.GetCBVSRVUAVDescriptorSize();

    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootSignatureIndex("textures"), textures);

    UINT ID = 0;
    for (auto& component : _ownerScene->_renderQueue)
    {
        const auto& model = component->GetModel();

        for (auto& mesh : model->GetMeshes())
        {
            commandList->SetGraphicsRoot32BitConstant(_shader->GetRootSignatureIndex("bit32_object"), ID++, 0);
            mesh->Render(commandList.Get());
        }
    }
}
