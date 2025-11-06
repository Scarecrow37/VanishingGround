#pragma once
#include "DxilLibrary.h"

class RTPipeline
{
public:
    RTPipeline() = default;

    struct RootSignatureDesc
    {
        D3D12_ROOT_SIGNATURE_DESC              desc{};
        std::vector<D3D12_DESCRIPTOR_RANGE>    range;
        std::vector<D3D12_ROOT_PARAMETER>      rootParams;
        std::vector<D3D12_STATIC_SAMPLER_DESC> staticSampler;
    };

    static const WCHAR* RayGenShader;
    static const WCHAR* MissShader;
    static const WCHAR* ClosestHitShader;
    static const WCHAR* HitGroup;
    static const WCHAR* ShadowAnyHitShader;
    static const WCHAR* ShadowMissShader;
    static const WCHAR* ShadowHitGroup;

    static D3D12_ROOT_SIGNATURE_DESC MakeRootSigDesc(const std::vector<D3D12_ROOT_PARAMETER>&      params,
                                                     const std::vector<D3D12_STATIC_SAMPLER_DESC>& samplers,
                                                     D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE);
    static DxilLibrary               CreateDxilLibrary();

//#ifdef NDEBUG
    static DxilLibrary CreateDxilLibraryFromBuiltIn();
//#endif

    static RootSignatureDesc         CreateRayGenRootDesc();
    static RootSignatureDesc         CreateHitRootDesc();
    static RootSignatureDesc         CreateMissRootDesc();
    static RootSignatureDesc         CreateGlobalRootDesc();
};
