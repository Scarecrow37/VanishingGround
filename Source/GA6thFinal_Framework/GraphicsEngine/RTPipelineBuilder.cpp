#include "pch.h"
#include "RTPipelineBuilder.h"
#include "d3dUtil.h"

void RTPipelineBuilder::AddShader(const ShaderEntry& shader) 
{
    _shaders.push_back(shader);
}

void RTPipelineBuilder::AddHitGroup(const HitGroupDesc& hitGroup)
{
    _hitGroups.push_back(hitGroup);
}

void RTPipelineBuilder::SetRayGenRootSignature(const D3D12_ROOT_SIGNATURE_DESC& root) 
{
    _rayGenRootDesc = root;
}

void RTPipelineBuilder::SetHitGroupRootSignature(const D3D12_ROOT_SIGNATURE_DESC& root) 
{
    _hitGroupRootDesc = root;
}

ComPtr<ID3D12StateObject> RTPipelineBuilder::BuildPipelineStateObject(ID3D12Device5* device)
{
    std::vector<D3D12_STATE_SUBOBJECT> subobjects;
    std::vector<DxilLibrary>           libraries;
    std::vector<ExportAssociation>     associations;
    std::vector<HitProgram>            hitGroups;

    for (const auto& shader : _shaders)
    {
        const WCHAR* entryNames[] = {shader.entry.c_str()};
        _allExportNames.push_back(shader.entry.c_str());
        auto         blob         = d3dUtil::CompileShaderLibrary(shader.filename.c_str(), shader.target.c_str());
        libraries.emplace_back(blob, entryNames, 1);
        subobjects.push_back(libraries.back().stateSubObject);
    }

    for (const auto& hit : _hitGroups)
    {
        hitGroups.emplace_back(hit.anyHit.c_str(), hit.closestHit.c_str(), hit.name);
        subobjects.push_back(hitGroups.back().subObject);
    }

    GlobalRootSignature globalRoot({});
    subobjects.push_back(globalRoot.subObject);

    LocalRootSignature rgsRoot(_rayGenRootDesc);
    subobjects.push_back(rgsRoot.subObject);
    ExportAssociation rgsAssoc(&_allExportNames[0], 1, &subobjects[subobjects.size() - 1]);
    subobjects.push_back(rgsAssoc.subObject);

    LocalRootSignature hitRoot(_hitGroupRootDesc);
    subobjects.push_back(hitRoot.subObject);
    ExportAssociation hitAssoc(&_allExportNames[2], 1, &subobjects[subobjects.size() - 1]);
    subobjects.push_back(hitAssoc.subObject);

    _missRootDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
    LocalRootSignature missRoot(_missRootDesc);
    subobjects.push_back(missRoot.subObject);
    const WCHAR*      missShaders[] = {_allExportNames[1]};
    ExportAssociation missAssoc(missShaders, 1 , &subobjects[subobjects.size() - 1]);
    subobjects.push_back(missAssoc.subObject);

    ShaderConfig shaderCfg(sizeof(float) * 2, sizeof(float) * (4 + 1 + 1));
    subobjects.push_back(shaderCfg.subObject);
    std::vector<const WCHAR*> exportPtrs;
    for (const auto& name : _allExportNames)
        exportPtrs.push_back(name);
    ExportAssociation configAssoc(exportPtrs.data(), (uint32_t)exportPtrs.size(), &subobjects[subobjects.size() - 1]);
    subobjects.push_back(configAssoc.subObject);

    PipelineConfig pipelineCfg(8);
    subobjects.push_back(pipelineCfg.subObject);

    D3D12_STATE_OBJECT_DESC desc = {};
    desc.Type                    = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
    desc.NumSubobjects           = static_cast<UINT>(subobjects.size());
    desc.pSubobjects             = subobjects.data();

    ComPtr<ID3D12StateObject> stateObject;
    HRESULT                   hr = device->CreateStateObject(&desc, IID_PPV_ARGS(stateObject.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr, L"RTPipelineBuilder::BuildPipelineStateObject() failed createstateobject");
    return stateObject;
}
