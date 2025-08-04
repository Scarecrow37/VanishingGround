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