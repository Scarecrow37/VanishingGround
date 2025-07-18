#pragma once
struct PipelineConfig
{
    PipelineConfig(const uint32_t maxTraceRecursionDepth)
    {
        config.MaxTraceRecursionDepth = maxTraceRecursionDepth;

        subObject.Type  = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG;
        subObject.pDesc = &config;
    }
    D3D12_RAYTRACING_PIPELINE_CONFIG config{};
    D3D12_STATE_SUBOBJECT            subObject{};
};