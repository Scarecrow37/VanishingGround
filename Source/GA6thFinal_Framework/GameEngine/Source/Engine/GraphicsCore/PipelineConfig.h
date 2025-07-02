#pragma once
class PipelineConfig
{
    PipelineConfig(const uint32_t maxTraceRecursiondDepth)
    {
        config.MaxTraceRecursionDepth = maxTraceRecursiondDepth;
        subObject.Type                = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG;
        subObject.pDesc               = &config;
    }
    D3D12_RAYTRACING_PIPELINE_CONFIG config{};
    D3D12_STATE_SUBOBJECT            subObject{};
};
