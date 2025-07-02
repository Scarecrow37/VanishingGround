#pragma once
struct HItProgram
{
    HItProgram(const LPCWSTR ahsExport, const LPCWSTR chsExport, const std::wstring& name)
    {
        desc                        = {};
        desc.AnyHitShaderImport     = ahsExport;
        desc.ClosestHitShaderImport = chsExport;
        desc.HitGroupExport         = exportName.c_str();

        subObject.Type  = D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP;
        subObject.pDesc = &desc;
    }
    std::wstring          exportName;
    D3D12_HIT_GROUP_DESC  desc;
    D3D12_STATE_SUBOBJECT subObject;
};
