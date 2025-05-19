#include "pch.h"
//#include "Material.h"
//
//Material::Material()
//{
//    HRESULT hr = UmDevice.CreateCommandList(_commandAllocator, _commandList, COMMAND_TYPE::BUNDLE);
//    FAILED_CHECK_BREAK(hr);
//}
//
//Material::~Material()
//{
//}
//
//void Material::CopyDescriptors(const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& handles)
//{
//    ComPtr<ID3D12Device> device = UmDevice.GetDevice();
//
//    UINT count = static_cast<UINT>(handles.size());
//
//    UINT destDescriptorCount = 0;
//    UINT srcDescriptorCount  = 0;
//
//    auto cpuHandle = _descriptor->GetCPUDescriptorHandleForHeapStart();
//    device->CopyDescriptors(count, &cpuHandle, nullptr, count, handles.data(), nullptr, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
//}