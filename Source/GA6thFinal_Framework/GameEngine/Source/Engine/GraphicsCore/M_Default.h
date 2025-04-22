#pragma once
#include "Material.h"

class M_Default : public Material
{
public:
    M_Default();
    virtual ~M_Default();

public:
    // Material을(를) 통해 상속됨
    void Initialize();
    void Render() override;

private:
    ComPtr<ID3D12PipelineState> _pipelineState;
    ComPtr<ID3D12RootSignature> _rootSignature;
    ComPtr<ID3D12GraphicsCommandList> _commandList;
};