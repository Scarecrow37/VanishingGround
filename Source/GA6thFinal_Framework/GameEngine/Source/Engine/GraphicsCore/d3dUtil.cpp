#include "pch.h"
#include "d3dUtil.h"

Microsoft::WRL::ComPtr<ID3D12Resource> d3dUtil::CreateDefaultBuffer(
    ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const void* initData, UINT64 byteSize,

    Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer)
{
    ComPtr<ID3D12Resource>  defaultBuffer;
    CD3DX12_HEAP_PROPERTIES heapPropertyDefault(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_HEAP_PROPERTIES heapPropertyUPLOAD(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC   defaultBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(byteSize);
    CD3DX12_RESOURCE_DESC   uploadBufferDesc  = CD3DX12_RESOURCE_DESC::Buffer(byteSize);
    // 실제 기본 버퍼 자원을 생성한다
    FAILED_CHECK_BREAK(device->CreateCommittedResource(&heapPropertyDefault, D3D12_HEAP_FLAG_NONE, &defaultBufferDesc,
                                                  D3D12_RESOURCE_STATE_COMMON, nullptr,
                                                  IID_PPV_ARGS(defaultBuffer.GetAddressOf())));

    // CPU 메모리의 자료를 기본 버퍼에 복사
    // 임시 업로드 힙을 생성
    FAILED_CHECK_BREAK(device->CreateCommittedResource(&heapPropertyUPLOAD, D3D12_HEAP_FLAG_NONE, &uploadBufferDesc,
                                                  D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                  IID_PPV_ARGS(uploadBuffer.GetAddressOf())));

    // 기본 버퍼에 복사할 자료를 서술
    D3D12_SUBRESOURCE_DATA subResourceData = {};
    subResourceData.pData                  = initData;
    subResourceData.RowPitch               = byteSize;
    subResourceData.SlicePitch             = subResourceData.RowPitch;

    // 기본 버퍼 자원으로의 자료 복사를 요청
    // 개략적으로 말하자면, 보조함수 UpdateSubResources는 CPU 메모리를
    // 임시 업로드 힙에 복사하고, ID3D12CommandList::CopySubresourceRegion을
    // 이용해서 임시 업로드 힙의 자료를 mBuffer에 복사.
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        defaultBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);

    cmdList->ResourceBarrier(1, &barrier);
    UpdateSubresources<1>(cmdList, defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);
    barrier = CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
                                                   D3D12_RESOURCE_STATE_GENERIC_READ);
    cmdList->ResourceBarrier(1, &barrier);

    // Note: uploadBuffer has to be kept alive after the above function calls
    // because the command list has not been executed yet that performs the actual
    // copy. The caller can Release the uploadBuffer after it knows the copy has
    // been executed.

    return defaultBuffer;
}