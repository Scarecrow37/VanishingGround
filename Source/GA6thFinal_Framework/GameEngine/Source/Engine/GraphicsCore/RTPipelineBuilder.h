#pragma once
#include "DxilLibrary.h"
#include "ExportAssociation.h"
#include "GlobalRootSignature.h"
#include "HItProgram.h"
#include "LocalRootSignature.h"
#include "PipelineConfig.h"
#include "ShaderConfig.h"

struct ShaderEntry
{
    std::wstring filename;
    std::wstring entry;
    std::wstring target;
};

struct HitGroupDesc
{
    std::wstring name;
    std::wstring closestHit;
    std::wstring anyHit;
    std::wstring intersection;
};

class RTPipelineBuilder
{
public:
    void AddShader(const ShaderEntry& shader);
    void AddHitGroup(const HitGroupDesc& hitGroup);
    void SetRayGenRootSignature(const D3D12_ROOT_SIGNATURE_DESC& root);
    void SetHitGroupRootSignature(const D3D12_ROOT_SIGNATURE_DESC& root);

    //ComPtr<ID3D12StateObject> BuildPipelineStateObject(ID3D12Device5* device);

private:
    std::vector<ShaderEntry> _shaders;
    std::vector<HitGroupDesc> _hitGroups;

    D3D12_ROOT_SIGNATURE_DESC _rayGenRootDesc{};
    D3D12_ROOT_SIGNATURE_DESC _hitGroupRootDesc{};

    std::vector<std::wstring> _allExportNames;
};
