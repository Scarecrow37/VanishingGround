#pragma once
struct GlobalRootSignature
{
    GlobalRootSignature(const D3D12_ROOT_SIGNATURE_DESC& desc)
    {
        rootSignature = UmDevice.CreateRootSignature(desc);
        pInterface      = rootSignature.Get();
        subObject.pDesc = &pInterface;
        subObject.Type  = D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE;
    }
    ComPtr<ID3D12RootSignature> rootSignature;
    ID3D12RootSignature* pInterface = nullptr;
    D3D12_STATE_SUBOBJECT       subObject{};
};
