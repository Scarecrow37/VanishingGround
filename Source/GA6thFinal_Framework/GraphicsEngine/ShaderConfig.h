#pragma once
struct ShaderConfig
{
    ShaderConfig(const uint32_t maxAttributeSizeInBytes, const uint32_t maxPayloadSizeInBytes)
    {
        shaderConfig.MaxAttributeSizeInBytes = maxAttributeSizeInBytes;
        shaderConfig.MaxPayloadSizeInBytes   = maxPayloadSizeInBytes;

        subObject.Type  = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG;
        subObject.pDesc = &shaderConfig;
    }

    D3D12_RAYTRACING_SHADER_CONFIG shaderConfig{};
    D3D12_STATE_SUBOBJECT          subObject{};
};