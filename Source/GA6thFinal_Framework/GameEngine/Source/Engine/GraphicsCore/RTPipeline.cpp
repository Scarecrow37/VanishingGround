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

    // cbv cameraData
    r.rootParams.resize(1);
    r.rootParams[0].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    r.rootParams[0].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;
    r.rootParams[0].Descriptor.ShaderRegister = 0; // b0
    r.rootParams[0].Descriptor.RegisterSpace  = 0;
    

    // 가속구조 srv (t0), 출력용 uav(u0) 테이블
    r.range.resize(2);
    r.range[0] = {D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, 0}; // t0
    r.range[1] = {D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0, 1}; // u0

    r.rootParams.emplace_back();
    r.rootParams[1].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    r.rootParams[1].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;
    r.rootParams[1].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(r.range.size());
    r.rootParams[1].DescriptorTable.pDescriptorRanges   = r.range.data();

    return r;
}

// 이거 작성.
RTPipeline::RootSignatureDesc RTPipeline::CreateHitRootDesc()
{
    RootSignatureDesc r;

    // CBV 5개 그대로(b0~b4)  ----
    r.rootParams.resize(5);
    for (int i = 0; i < 5; ++i)
    {
        r.rootParams[i].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
        r.rootParams[i].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;
        r.rootParams[i].Descriptor.ShaderRegister = i; // b0 ~ b4
        r.rootParams[i].Descriptor.RegisterSpace  = 0;
    }

    // SRV :  Indices(t1) / Vertices(t2) / Material(t3) /
    //               EnvCube(t4) / Textures[] (t5, unbounded) ----
    r.range.resize(2);
    r.range[0] = {D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, 1, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND}; // t0~t4
    r.range[1] = {D3D12_DESCRIPTOR_RANGE_TYPE_SRV, UINT_MAX, 5, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND};// t5~ ->textures

    // smapler
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
    r.desc = MakeRootSigDesc(r.rootParams, r.staticSampler, D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE);
    return r;
}

RTPipeline::RootSignatureDesc RTPipeline::CreateMissRootDesc()
{
    D3D12_DESCRIPTOR_RANGE    srvRange = {D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4, 0, 0}; // t4
    D3D12_STATIC_SAMPLER_DESC sampler  = {};
    sampler.Filter                     = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    sampler.AddressU = sampler.AddressV = sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.ShaderRegister                                 = 2; // s2
    sampler.RegisterSpace                                  = 0;
    sampler.ShaderVisibility                               = D3D12_SHADER_VISIBILITY_ALL;

    D3D12_ROOT_PARAMETER rootParam                = {};
    rootParam.ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParam.ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;
    rootParam.DescriptorTable.NumDescriptorRanges = 1;
    rootParam.DescriptorTable.pDescriptorRanges   = &srvRange;

    D3D12_ROOT_SIGNATURE_DESC desc = {};
    desc.NumParameters             = 1;
    desc.pParameters               = &rootParam;
    desc.NumStaticSamplers         = 1;
    desc.pStaticSamplers           = &sampler;
    desc.Flags                     = D3D12_ROOT_SIGNATURE_FLAG_NONE;

    RootSignatureDesc r;
    r.rootParams.resize(0);
    r.rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    r.rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    r.rootParams[0].DescriptorTable.NumDescriptorRanges = 1;
    r.rootParams[0].DescriptorTable.pDescriptorRanges = &srvRange;
    r.rootParams = rootParam;
}
