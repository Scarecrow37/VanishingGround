#pragma once

//class Shader;
//class Material
//{
//public:
//    Material();
//    virtual ~Material();
//
//public:
//    virtual void Initialize(const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& handles)                         = 0;
//    virtual void Instance(D3D12_GPU_VIRTUAL_ADDRESS camera)                                                  = 0;
//    virtual void SetupState(ComPtr<ID3D12GraphicsCommandList> commandList, D3D12_GPU_VIRTUAL_ADDRESS camera) = 0;
//
//protected:
//    void CopyDescriptors(const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& handles);
//
//protected:
//    ComPtr<ID3D12GraphicsCommandList> _commandList;
//    ComPtr<ID3D12CommandAllocator>    _commandAllocator;
//    ComPtr<ID3D12DescriptorHeap>      _descriptor;
//};