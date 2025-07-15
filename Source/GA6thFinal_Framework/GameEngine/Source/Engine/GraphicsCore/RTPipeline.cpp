#include "pch.h"
#include "RTPipeline.h"
#include "d3dUtil.h"
#include "dxcapi.use.h"
#include "ShaderBuilder.h"

const WCHAR* RTPipeline::RayGenShader     = L"RayGen";
const WCHAR* RTPipeline::MissShader       = L"Miss";
const WCHAR* RTPipeline::ClosestHitShader = L"ClosestHit";
const WCHAR* RTPipeline::HitGroup         = L"HitGroup";

static dxc::DxcDllSupport gDxcDllHelper;

D3D12_ROOT_SIGNATURE_DESC RTPipeline::MakeRootSigDesc(const std::vector<D3D12_ROOT_PARAMETER>&      params,
                                                      const std::vector<D3D12_STATIC_SAMPLER_DESC>& samplers,
                                                      D3D12_ROOT_SIGNATURE_FLAGS flags)
{
    D3D12_ROOT_SIGNATURE_DESC desc{};
    desc.NumParameters     = (UINT)params.size();
    desc.pParameters       = params.data();
    desc.NumStaticSamplers = (UINT)samplers.size();
    desc.pStaticSamplers   = samplers.data();
    desc.Flags             = flags;
    return desc;
}

DxilLibrary RTPipeline::CreateDxilLibrary()
{
    // compile shader
    const ComPtr<IDxcBlob> rayGenshader  = d3dUtil::CompileShaderLibrary(L"../Shaders/RTShaders.hlsl", L"lib_6_3");
    const WCHAR*           entryPoints[] = {RayGenShader, MissShader, ClosestHitShader};
    return DxilLibrary(rayGenshader, entryPoints, ARRAYSIZE(entryPoints));
}


// 이거는 완
RTPipeline::RootSignatureDesc RTPipeline::CreateRayGenRootDesc()
{
    RootSignatureDesc r;
    r.range.resize(2);
    /* ───────── 테이블 #1 : SRV t0 한 개 ───────── */
    D3D12_DESCRIPTOR_RANGE srvRange{};
    srvRange.RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    srvRange.NumDescriptors                    = 1;
    srvRange.BaseShaderRegister                = 0; // t0
    srvRange.RegisterSpace                     = 0;
    srvRange.OffsetInDescriptorsFromTableStart = 0;
    r.range[0] = srvRange;

    D3D12_ROOT_PARAMETER srvTable{};
    srvTable.ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    srvTable.ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;
    srvTable.DescriptorTable.NumDescriptorRanges = 1;
    srvTable.DescriptorTable.pDescriptorRanges   = &r.range[0];
    r.rootParams.push_back(srvTable); // RootParam #1

    /* ───────── 테이블 #2 : UAV u0 한 개 ───────── */
    D3D12_DESCRIPTOR_RANGE uavRange{};
    uavRange.RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
    uavRange.NumDescriptors                    = 1;
    uavRange.BaseShaderRegister                = 0; // u0
    uavRange.RegisterSpace                     = 0;
    uavRange.OffsetInDescriptorsFromTableStart = 0;
    r.range[1]                                 = uavRange;

    D3D12_ROOT_PARAMETER uavTable{};
    uavTable.ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    uavTable.ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;
    uavTable.DescriptorTable.NumDescriptorRanges = 1;
    uavTable.DescriptorTable.pDescriptorRanges   = &r.range[1];
    r.rootParams.push_back(uavTable); // RootParam #2

    /* ───────── 나머지 설정 ───────── */
    r.desc.NumParameters = static_cast<UINT>(r.rootParams.size());
    r.desc.pParameters   = r.rootParams.data();
    r.desc.Flags         = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
    return r;
}

RTPipeline::RootSignatureDesc RTPipeline::CreateHitRootDesc()
{
    RootSignatureDesc r;
    r.range.resize(5);
    /* ---------- 1 SRV t0 (TLAS)  ---------- */
    D3D12_DESCRIPTOR_RANGE tlasRange{};
    tlasRange.RangeType          = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    tlasRange.NumDescriptors     = 1;
    tlasRange.BaseShaderRegister = 0; // t0
    r.range[0] = tlasRange;

    D3D12_ROOT_PARAMETER tlasTable{};
    tlasTable.ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    tlasTable.ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;
    tlasTable.DescriptorTable.NumDescriptorRanges = 1;
    tlasTable.DescriptorTable.pDescriptorRanges   = &r.range[0];
    r.rootParams.push_back(tlasTable); // RootParam #4
  
    /* ---------- 2 cube texture t4---------- */
    D3D12_DESCRIPTOR_RANGE cubeMapRange{};
    cubeMapRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    cubeMapRange.NumDescriptors = 1;
    cubeMapRange.BaseShaderRegister = 5;
    r.range[1]                      = cubeMapRange;

    D3D12_ROOT_PARAMETER cubeMapTable{};
    cubeMapTable.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    cubeMapTable.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    cubeMapTable.DescriptorTable.NumDescriptorRanges = 1;
    cubeMapTable.DescriptorTable.pDescriptorRanges   = &r.range[1];
    r.rootParams.push_back(cubeMapTable);

    /* ---------- 3 Vertices[2000]  ---------- */
    D3D12_DESCRIPTOR_RANGE verticesRange{};
    verticesRange.RangeType          = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    verticesRange.NumDescriptors     = 2000; // t5~t2005
    verticesRange.BaseShaderRegister = 6;
    r.range[2]                       = verticesRange;
   
    D3D12_ROOT_PARAMETER verticesTable{};
    verticesTable.ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    verticesTable.ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;
    verticesTable.DescriptorTable.NumDescriptorRanges = 1;
    verticesTable.DescriptorTable.pDescriptorRanges   = &r.range[2];
    r.rootParams.push_back(verticesTable); // #9

    /* ---------- 4 Indices[2000]  ---------- */
    D3D12_DESCRIPTOR_RANGE indicesRange{};
    indicesRange.RangeType          = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    indicesRange.NumDescriptors     = 2000; // t2006~t4005
    indicesRange.BaseShaderRegister = 2006;
    r.range[3] = indicesRange;

    D3D12_ROOT_PARAMETER indicesTable{};
    indicesTable.ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    indicesTable.ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;
    indicesTable.DescriptorTable.NumDescriptorRanges = 1;
    indicesTable.DescriptorTable.pDescriptorRanges   = &r.range[3];
    r.rootParams.push_back(indicesTable); // #10

    /* ---------- 5 textures[]  (unbounded) ---------- */
    D3D12_DESCRIPTOR_RANGE texRange{};
    texRange.RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    texRange.NumDescriptors                    = 1000; // 무제한
    texRange.BaseShaderRegister                = 4006;     // t4005~
    texRange.RegisterSpace                     = 0;
    texRange.OffsetInDescriptorsFromTableStart = 0;
    r.range[4] = texRange;

    D3D12_ROOT_PARAMETER texTable{};
    texTable.ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    texTable.ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;
    texTable.DescriptorTable.NumDescriptorRanges = 1;
    texTable.DescriptorTable.pDescriptorRanges   = &r.range[4];
    r.rootParams.push_back(texTable); // #11

    /* ---------- 6 루트 시그니처 만들기 ---------- */
    r.desc.NumParameters     = static_cast<UINT>(r.rootParams.size()); // 8개
    r.desc.pParameters       = r.rootParams.data();
    r.desc.NumStaticSamplers = 0;
    r.desc.pStaticSamplers   = nullptr;
    r.desc.Flags             = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
    return r;
}
RTPipeline::RootSignatureDesc RTPipeline::CreateMissRootDesc()
{
    RootSignatureDesc r;

    r.range.resize(1);
    r.range[0] = {
        D3D12_DESCRIPTOR_RANGE_TYPE_SRV, // RangeType
        1,                               // NumDescriptors
        5,                               // BaseShaderRegister = t5
        0,                               // RegisterSpace
        0                                // OffsetInDescriptorsFromTableStart
    };
    r.rootParams.resize(1);
    r.rootParams[0].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    r.rootParams[0].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;
    r.rootParams[0].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(r.range.size());
    r.rootParams[0].DescriptorTable.pDescriptorRanges   = r.range.data();

    r.desc.NumParameters                                = static_cast<UINT>(r.rootParams.size());
    r.desc.pParameters                                  = r.rootParams.data();
    r.desc.NumStaticSamplers                            = 0;
    r.desc.pStaticSamplers                              = nullptr;
    r.desc.Flags                                        = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
    return r;
}

RTPipeline::RootSignatureDesc RTPipeline::CreateGlobalRootDesc()
{
    RootSignatureDesc r;
    // 상수버퍼는 global root signature로 사용하여 local root를 사용하지 않게금 함.
    // 굳이 local 로 분리할 필요가 없기 떄문.
    r.rootParams.resize(7);
    // b0 camera data
    r.rootParams[0].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    r.rootParams[0].Descriptor.ShaderRegister = 0;
    r.rootParams[0].Descriptor.RegisterSpace  = 0;
    r.rootParams[0].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;
    // b1 light data
    r.rootParams[1].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    r.rootParams[1].Descriptor.ShaderRegister = 1;
    r.rootParams[1].Descriptor.RegisterSpace  = 0;
    r.rootParams[1].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;
    // b2 bit32_3_numLight
    r.rootParams[2].ParameterType            = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    r.rootParams[2].Constants.Num32BitValues = 3;
    r.rootParams[2].Constants.ShaderRegister = 2;
    r.rootParams[2].Constants.RegisterSpace  = 0;
    r.rootParams[2].ShaderVisibility         = D3D12_SHADER_VISIBILITY_ALL;

    // 동일하게 structured buffer들을 gloabl root signature 를 사용.
    // vertex buffer id
    r.rootParams[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
    r.rootParams[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    r.rootParams[3].Descriptor.ShaderRegister = 1;
    r.rootParams[3].Descriptor.RegisterSpace  = 0;
    // index buffer id
    r.rootParams[4].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_SRV;
    r.rootParams[4].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;
    r.rootParams[4].Descriptor.ShaderRegister = 2;
    r.rootParams[4].Descriptor.RegisterSpace  = 0;
    // material
    r.rootParams[5].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_SRV;
    r.rootParams[5].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;
    r.rootParams[5].Descriptor.ShaderRegister = 3;
    r.rootParams[5].Descriptor.RegisterSpace  = 0;
    // mesh instance id
    r.rootParams[6].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_SRV;
    r.rootParams[6].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;
    r.rootParams[6].Descriptor.ShaderRegister = 4;
    r.rootParams[6].Descriptor.RegisterSpace  = 0;

    r.staticSampler.resize(7);
    ShaderBuilder builder;
    builder.CreateStaticSampler(D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_WRAP, 0, r.staticSampler[0]);
    builder.CreateStaticSampler(D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, 1,
                                r.staticSampler[1]);
    builder.CreateStaticSampler(D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_WRAP, 2,
                                r.staticSampler[2]);
    builder.CreateStaticSampler(D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, 3,
                                r.staticSampler[3]);
    builder.CreateStaticSampler(D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 4,
                                r.staticSampler[4]);
    builder.CreateStaticSampler(D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_WRAP, 5, r.staticSampler[5]);
    builder.CreateStaticSampler(D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, 6, r.staticSampler[6]);
    r.desc = MakeRootSigDesc(r.rootParams, r.staticSampler, D3D12_ROOT_SIGNATURE_FLAG_NONE);
    return r;
}