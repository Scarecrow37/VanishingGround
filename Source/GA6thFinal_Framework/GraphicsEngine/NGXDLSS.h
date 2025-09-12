#pragma once
#include "ThirdParty/include/nvsdk/nvsdk_ngx.h"
#include "ThirdParty/include/nvsdk/nvsdk_ngx_defs.h"
#include "ThirdParty/include/nvsdk/nvsdk_ngx_helpers.h"
#include "ThirdParty/include/nvsdk/nvsdk_ngx_params.h"

struct NGXInitDesc
{
    ID3D12Device* device = nullptr;

    // 로그/임시파일 경로 (쓰기 가능 경로). nullptr면 기본(nullptr 전달)
    const wchar_t* appDataPath = nullptr;

    // ProjectID 기반 초기화를 원할 때만 채우세요(권장: GUID 문자열)
    const char*          projectId     = nullptr;
    NVSDK_NGX_EngineType engineType    = NVSDK_NGX_ENGINE_TYPE_CUSTOM;
    const char*          engineVersion = "1.0";
};

struct NGXFrameDesc
{
    // Render resolution
    uint32_t renderW = 0;
    uint32_t renderH = 0;

    // Inputs at render resolution
    ID3D12Resource* color  = nullptr; // RGBA16F 권장, 선형
    ID3D12Resource* depth  = nullptr; // D32F
    ID3D12Resource* motion = nullptr; // R16G16_FLOAT, non-jitter, pixel units

    // Output at display resolution (UAV)
    ID3D12Resource* output = nullptr; // RGBA16F 권장

    // Jitter in pixels (render-res)
    float jitterX = 0.0f;
    float jitterY = 0.0f;

    // Optional per-frame
    bool            reset           = false; // camera-cut 등
    float           mvScaleX        = 1.0f;  // 픽셀 단위면 1.0
    float           mvScaleY        = 1.0f;
    ID3D12Resource* exposureTexture = nullptr; // 1x1 pre-exposure if used
    float           preExposure     = 1.0f;    // 0이면 helpers에서 1.0으로 보정됨
    float           exposureScale   = 1.0f;
    float           deltaTimeMs     = 16.6f;

    ID3D12GraphicsCommandList* cmd = nullptr;
};

enum class DlssPerfQuality
{
    Quality,          // -> MaxQuality
    Balanced,         // -> Balanced
    Performance,      // -> MaxPerf
    UltraQuality,     // -> UltraQuality
    UltraPerformance, // -> UltraPerformance
    DLAA              // -> DLAA (AA only, no upscale)
};

class NGXDLSS
{
public:
    bool Initialize(const NGXInitDesc& d);
    void Shutdown();

    // 출력 해상도 변경 시 호출(리소스는 엔진 쪽에서 재할당)
    void OnResize(uint32_t displayW, uint32_t displayH)
    {
        _displayW = displayW;
        _displayH = displayH;
    }

    // DLSS 권장 렌더 해상도/샤프니스 쿼리
    bool QueryOptimal(DlssPerfQuality mode, uint32_t displayW, uint32_t displayH, uint32_t& outRenderW,
                      uint32_t& outRenderH, float& outSharpness);

    void            SetMode(DlssPerfQuality m) { _mode = m; }
    DlssPerfQuality Mode() const { return _mode; }

    void SetEnabled(bool e) { _enabled = e; }
    bool Enabled() const { return _enabled; }

    // Create/Update DLSS feature for given resolutions and flags
    // depthInverted: 역-깊이 사용 시 true
    // hdr: HDR 파이프라인(선형 HDR + PreExposure/ExposureScale 제공) 시 true
    bool CreateOrUpdateFeature(ID3D12GraphicsCommandList* cmd, uint32_t renderW, uint32_t renderH, uint32_t outputW,
                               uint32_t outputH, bool depthInverted, bool hdr, bool enableOutputSubrects = false);

    bool Evaluate(const NGXFrameDesc& f);

private:
    NVSDK_NGX_PerfQuality_Value MapMode(DlssPerfQuality m) const;

    bool _initialized = false;
    bool _enabled     = true;

    ID3D12Device*        _device     = nullptr;
    NVSDK_NGX_Parameter* _params     = nullptr; // capability/working params
    NVSDK_NGX_Handle*    _dlssHandle = nullptr;

    // cached
    DlssPerfQuality _mode     = DlssPerfQuality::Balanced;
    uint32_t        _displayW = 0, _displayH = 0;

    uint32_t                    _createdRenderW = 0, _createdRenderH = 0;
    uint32_t                    _createdOutputW = 0, _createdOutputH = 0;
    int                         _createdFlags = NVSDK_NGX_DLSS_Feature_Flags_None;
    NVSDK_NGX_PerfQuality_Value _createdPerf  = NVSDK_NGX_PerfQuality_Value_Balanced;
};