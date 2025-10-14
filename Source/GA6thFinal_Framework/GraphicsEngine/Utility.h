#pragma once
#include "Structs.h"
#include <functional>
#include <string.h>

template <class T>
inline void hash_combine(size_t& seed, const T& v) {
    seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

inline bool operator==(const PipelineStateStream& lhs, const PipelineStateStream& rhs)
{
    if (static_cast<ID3D12RootSignature*>(lhs.RootSignature) != static_cast<ID3D12RootSignature*>(rhs.RootSignature)) return false;
    if (static_cast<D3D12_PRIMITIVE_TOPOLOGY_TYPE>(lhs.PrimitiveTopology) != static_cast<D3D12_PRIMITIVE_TOPOLOGY_TYPE>(rhs.PrimitiveTopology)) return false;
    if (static_cast<DXGI_FORMAT>(lhs.DSVFormat) != static_cast<DXGI_FORMAT>(rhs.DSVFormat)) return false;

    D3D12_SHADER_BYTECODE lhs_vs = lhs.VS, rhs_vs = rhs.VS;
    if (lhs_vs.BytecodeLength != rhs_vs.BytecodeLength || (lhs_vs.BytecodeLength > 0 && memcmp(lhs_vs.pShaderBytecode, rhs_vs.pShaderBytecode, lhs_vs.BytecodeLength) != 0)) return false;

    D3D12_SHADER_BYTECODE lhs_ps = lhs.PS, rhs_ps = rhs.PS;
    if (lhs_ps.BytecodeLength != rhs_ps.BytecodeLength || (lhs_ps.BytecodeLength > 0 && memcmp(lhs_ps.pShaderBytecode, rhs_ps.pShaderBytecode, lhs_ps.BytecodeLength) != 0)) return false;

    D3D12_SHADER_BYTECODE lhs_gs = lhs.GS, rhs_gs = rhs.GS;
    if (lhs_gs.BytecodeLength != rhs_gs.BytecodeLength || (lhs_gs.BytecodeLength > 0 && memcmp(lhs_gs.pShaderBytecode, rhs_gs.pShaderBytecode, lhs_gs.BytecodeLength) != 0)) return false;

    D3D12_BLEND_DESC lb = lhs.BlendState, rb = rhs.BlendState;   
    if (std::memcmp(&lb, &rb, sizeof(D3D12_BLEND_DESC)) != 0) return false;

    D3D12_RASTERIZER_DESC lr = lhs.RasterizerState, rr = rhs.RasterizerState;
    if (std::memcmp(&lr, &rr, sizeof(D3D12_RASTERIZER_DESC)) != 0) return false;

    D3D12_DEPTH_STENCIL_DESC ld = lhs.DepthStencilState, rd = rhs.DepthStencilState;
    if (ld.DepthEnable != rd.DepthEnable || ld.DepthWriteMask != rd.DepthWriteMask || ld.DepthFunc != rd.DepthFunc ||
        ld.StencilEnable != rd.StencilEnable || ld.StencilReadMask != rd.StencilReadMask || ld.StencilWriteMask != rd.StencilWriteMask ||
        memcmp(&ld.FrontFace, &rd.FrontFace, sizeof(D3D12_DEPTH_STENCILOP_DESC)) != 0 ||
        memcmp(&ld.BackFace, &rd.BackFace, sizeof(D3D12_DEPTH_STENCILOP_DESC)) != 0) return false;

    D3D12_RT_FORMAT_ARRAY lrv = lhs.RTVFormats, rrv = rhs.RTVFormats;
    if (std::memcmp(&lrv, &rrv, sizeof(D3D12_RT_FORMAT_ARRAY)) != 0) return false;

    return true;
}

inline bool operator==(const ComputePipelineStateStream& lhs, const ComputePipelineStateStream& rhs)
{
    if (static_cast<ID3D12RootSignature*>(lhs.RootSignature) != static_cast<ID3D12RootSignature*>(rhs.RootSignature)) return false;
    D3D12_SHADER_BYTECODE lhs_cs = lhs.CS, rhs_cs = rhs.CS;
    if (lhs_cs.BytecodeLength != rhs_cs.BytecodeLength || (lhs_cs.BytecodeLength > 0 && memcmp(lhs_cs.pShaderBytecode, rhs_cs.pShaderBytecode, lhs_cs.BytecodeLength) != 0)) return false;
    return true;
}

namespace std
{
    template<>
    struct hash<PipelineStateStream>
    {
        size_t operator()(const PipelineStateStream& s) const 
        {
            size_t seed = 0;

            hash_combine(seed, static_cast<ID3D12RootSignature*>(s.RootSignature));
            hash_combine(seed, static_cast<D3D12_PRIMITIVE_TOPOLOGY_TYPE>(s.PrimitiveTopology));
            hash_combine(seed, static_cast<DXGI_FORMAT>(s.DSVFormat));

            D3D12_SHADER_BYTECODE vs = s.VS, ps = s.PS, gs = s.GS;
            if (vs.pShaderBytecode) hash_combine(seed, vs.pShaderBytecode);
            if (ps.pShaderBytecode) hash_combine(seed, ps.pShaderBytecode);
            if (gs.pShaderBytecode) hash_combine(seed, gs.pShaderBytecode);

            // --- 4. 나머지 POD 구조체들을 멤버별로 해싱 ---
            D3D12_RASTERIZER_DESC r = s.RasterizerState;
            hash_combine(seed, r.FillMode);
            hash_combine(seed, r.CullMode);
            hash_combine(seed, r.FrontCounterClockwise);
            hash_combine(seed, r.DepthBias);
            hash_combine(seed, r.DepthBiasClamp);
            hash_combine(seed, r.SlopeScaledDepthBias);
            hash_combine(seed, r.DepthClipEnable);
            hash_combine(seed, r.MultisampleEnable);
            hash_combine(seed, r.AntialiasedLineEnable);
            hash_combine(seed, r.ForcedSampleCount);
            hash_combine(seed, r.ConservativeRaster);

            D3D12_BLEND_DESC b = s.BlendState;
            hash_combine(seed, b.AlphaToCoverageEnable);
            hash_combine(seed, b.IndependentBlendEnable);
            for(int i=0; i<8; ++i) {
                const auto& rt_b = b.RenderTarget[i];
                hash_combine(seed, rt_b.BlendEnable); hash_combine(seed, rt_b.LogicOpEnable); hash_combine(seed, rt_b.SrcBlend);
                hash_combine(seed, rt_b.DestBlend); hash_combine(seed, rt_b.BlendOp); hash_combine(seed, rt_b.SrcBlendAlpha);
                hash_combine(seed, rt_b.DestBlendAlpha); hash_combine(seed, rt_b.BlendOpAlpha); hash_combine(seed, rt_b.LogicOp);
                hash_combine(seed, rt_b.RenderTargetWriteMask);
            }

            D3D12_DEPTH_STENCIL_DESC d = s.DepthStencilState;
            hash_combine(seed, d.DepthEnable);
            hash_combine(seed, d.DepthWriteMask);
            hash_combine(seed, d.DepthFunc);
            hash_combine(seed, d.StencilEnable);
            hash_combine(seed, d.StencilReadMask);
            hash_combine(seed, d.StencilWriteMask);

            const char* front_p = reinterpret_cast<const char*>(&d.FrontFace);
            for(size_t i=0; i<sizeof(d.FrontFace); ++i) hash_combine(seed, front_p[i]);
            const char* back_p = reinterpret_cast<const char*>(&d.BackFace);
            for(size_t i=0; i<sizeof(d.BackFace); ++i) hash_combine(seed, back_p[i]);

            D3D12_RT_FORMAT_ARRAY rtv = s.RTVFormats;
            hash_combine(seed, rtv.NumRenderTargets);
            for(UINT i=0; i<rtv.NumRenderTargets; ++i) hash_combine(seed, rtv.RTFormats[i]);

            return seed;
        }
    };

    template <>
    struct hash<ComputePipelineStateStream>
    {
        size_t operator()(const ComputePipelineStateStream& s) const 
        {
            size_t seed = 0;
            hash_combine(seed, static_cast<ID3D12RootSignature*>(s.RootSignature));
            D3D12_SHADER_BYTECODE cs = s.CS;
            if (cs.pShaderBytecode) hash_combine(seed, cs.pShaderBytecode);
            return seed;
        }
    };
}