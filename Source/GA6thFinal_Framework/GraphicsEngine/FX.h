#pragma once
#include "ShaderTypes.mappings.generated.h"

namespace Global
{
    extern std::unordered_map<std::wstring, std::wstring> shaderPathMappings;
}

template <GE::VS vs, GE::PS ps = GE::PS::NONE, GE::GS gs = GE::GS::NONE>
class FX
{
public:
    FX()  = default;
    ~FX() = default;

public:
    UINT GetRootParameterIndex(std::string_view tag) const { return _shader.GetRootParameterIndex(tag); }
    ID3D12RootSignature* GetRootSignature() const { return _shader.GetRootSignature(); }

public:
    void SetPipelineStateStream(PipelineStateStream& pss)
    {
        if (nullptr == _shader.GetRootSignature())
        {
            _shader.BeginBuild();

        #ifdef _DEBUG
            const auto& vsFileName = GE::enumToVSFileNameMap.at(vs);
            _shader.SetShader(Global::shaderPathMappings[vsFileName], ShaderBuilder::Type::VS);

            if constexpr (GE::PS::NONE != ps)
            {
                const auto& psFileName = GE::enumToPSFileNameMap.at(ps);
                _shader.SetShader(Global::shaderPathMappings[psFileName], ShaderBuilder::Type::PS);
            }
            if constexpr (GE::GS::NONE != gs)
            {
                const auto& gsFileName = GE::enumToGSFileNameMap.at(gs);
                _shader.SetShader(Global::shaderPathMappings[gsFileName], ShaderBuilder::Type::GS);
            }
        #else
            const auto& vsFileName = GE::enumToVSGlobalNameMap.at(vs);
            _shader.SetShader(vsFileName, ShaderBuilder::Type::VS);

            if constexpr (GE::PS::NONE != ps)
            {
                const auto& psFileName = GE::enumToPSGlobalNameMap.at(ps);
                _shader.SetShader(psFileName, ShaderBuilder::Type::PS);
            }
            if constexpr (GE::GS::NONE != gs)
            {
                const auto& gsFileName = GE::enumToGSGlobalNameMap.at(gs);
                _shader.SetShader(gsFileName, ShaderBuilder::Type::GS);
            }
        #endif
            _shader.EndBuild();
        }

        pss.RootSignature = _shader.GetRootSignature();
        pss.InputLayout   = _shader.GetInputLayout();

        pss.VS = _shader.GetShaderByteCode(ShaderBuilder::Type::VS);
        if constexpr (GE::PS::NONE != ps)
        {
            pss.PS = _shader.GetShaderByteCode(ShaderBuilder::Type::PS);
        }
        if constexpr (GE::GS::NONE != gs)
        {
            pss.GS = _shader.GetShaderByteCode(ShaderBuilder::Type::GS);
        }
    }

private:
    inline static ShaderBuilder _shader;
};

template <GE::CS ccs>
class ComputeFX
{
public:
    ComputeFX()  = default;
    ~ComputeFX() = default;

public:
    UINT GetRootParameterIndex(std::string_view tag) const { return _shader.GetRootParameterIndex(tag); }
    ID3D12RootSignature* GetRootSignature() const { return _shader.GetRootSignature(); }

public:
    void SetPipelineStateStream(ComputePipelineStateStream& pss)
    {
        if (nullptr == _shader.GetRootSignature())
        {
            _shader.BeginBuild();

        #ifdef _DEBUG
            const auto& csFileName = GE::enumToCSFileNameMap.at(ccs);
            _shader.SetShader(Global::shaderPathMappings[csFileName], ShaderBuilder::Type::CS);
        #else
            const auto& csFileName = GE::enumToCSGlobalNameMap.at(ccs);
            _shader.SetShader(csFileName, ShaderBuilder::Type::CS);
        #endif

            _shader.EndBuild();
        }

        pss.RootSignature = _shader.GetRootSignature();
        pss.CS = _shader.GetShaderByteCode(ShaderBuilder::Type::CS);
    }

private:
    inline static ShaderBuilder _shader;
};