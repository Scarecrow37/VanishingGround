//#pragma once
//#include "Material.h"
//
//class M_PBR : public Material
//{
//public:
//    M_PBR();
//    virtual ~M_PBR();
//
//public:
//    // Material을(를) 통해 상속됨
//    void Initialize(const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& handles) override;
//    void Instance(D3D12_GPU_VIRTUAL_ADDRESS camera) override;
//    void SetupState(ComPtr<ID3D12GraphicsCommandList> commandList, D3D12_GPU_VIRTUAL_ADDRESS camera) override;
//
//private:
//    ComPtr<ID3D12PipelineState>       _pipelineState;
//    ComPtr<ID3D12GraphicsCommandList> _commandList;
//    std::shared_ptr<Shader>           _shader;
//};