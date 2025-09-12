#include "pch.h"
#include "NGXDLSS.h"

static void NGX_LOG_FAIL(NVSDK_NGX_Result r, const char* where)
{
    if (NVSDK_NGX_FAILED(r))
    {
        wchar_t const* wmsg = GetNGXResultAsString ? GetNGXResultAsString(r) : L"";
        char           msgA[512];
        sprintf_s(msgA, "[NGX] %s failed: 0x%08X\n", where, (unsigned)r);
        OutputDebugStringA(msgA);
        if (wmsg && wmsg[0])
        {
            OutputDebugStringW(L"[NGX] Reason: ");
            OutputDebugStringW(wmsg);
            OutputDebugStringW(L"\n");
        }
    }
}

NVSDK_NGX_PerfQuality_Value NGXDLSS::MapMode(DlssPerfQuality m) const
{
    switch (m)
    {
    case DlssPerfQuality::Quality:
        return NVSDK_NGX_PerfQuality_Value_MaxQuality;
    case DlssPerfQuality::Balanced:
        return NVSDK_NGX_PerfQuality_Value_Balanced;
    case DlssPerfQuality::Performance:
        return NVSDK_NGX_PerfQuality_Value_MaxPerf;
    case DlssPerfQuality::UltraQuality:
        return NVSDK_NGX_PerfQuality_Value_UltraQuality;
    case DlssPerfQuality::UltraPerformance:
        return NVSDK_NGX_PerfQuality_Value_UltraPerformance;
    case DlssPerfQuality::DLAA:
        return NVSDK_NGX_PerfQuality_Value_DLAA;
    default:
        return NVSDK_NGX_PerfQuality_Value_Balanced;
    }
}

bool NGXDLSS::Initialize(const NGXInitDesc& d)
{
    if (_initialized)
        return true;
    _device = d.device;

    // 선택: ProjectID가 있으면 해당 API 사용
    NVSDK_NGX_Result r = NVSDK_NGX_Result_Success;

    NVSDK_NGX_FeatureCommonInfo featureInfo{};
    // 필요 시 플러그인 검색 경로/로깅 콜백 구성 가능 (기본값은 앱 폴더)

#if defined(NGX_SNIPPET_BUILD)
    // Snippet 빌드가 아닌 일반 SDK를 사용한다는 전제에서 이 경로는 보통 사용하지 않습니다.
    (void)featureInfo;
    (void)d;
#else
    if (d.projectId && d.projectId[0] != '\0')
    {
        r = NVSDK_NGX_D3D12_Init_with_ProjectID(d.projectId, d.engineType, d.engineVersion ? d.engineVersion : "1.0",
                                                d.appDataPath, _device, &featureInfo, NVSDK_NGX_Version_API);
    }
    else
    {
        // AppId가 없는 경우에도 0으로 호출 가능(문서상 ProjectID 권장)
        unsigned long long appId = 0ULL;
        r = NVSDK_NGX_D3D12_Init(appId, d.appDataPath, _device, &featureInfo, NVSDK_NGX_Version_API);
    }
#endif

    NGX_LOG_FAIL(r, "NVSDK_NGX_D3D12_Init");
    if (NVSDK_NGX_FAILED(r))
        return false;

    // Capability 파라미터 획득(최적 설정 콜백 등 포함)
    r = NVSDK_NGX_D3D12_GetCapabilityParameters(&_params);
    if (NVSDK_NGX_FAILED(r))
    {
        // 오래된 드라이버 호환: AllocateParameters 폴백(OptimalSettings 콜백은 없을 수 있음)
        r = NVSDK_NGX_D3D12_AllocateParameters(&_params);
        NGX_LOG_FAIL(r, "NVSDK_NGX_D3D12_AllocateParameters");
        if (NVSDK_NGX_FAILED(r))
            return false;
    }

    _initialized = true;
    return true;
}

void NGXDLSS::Shutdown()
{
    if (!_initialized)
        return;

    if (_dlssHandle)
    {
        NVSDK_NGX_Result r = NVSDK_NGX_D3D12_ReleaseFeature(_dlssHandle);
        NGX_LOG_FAIL(r, "NVSDK_NGX_D3D12_ReleaseFeature");
        _dlssHandle = nullptr;
    }

    if (_params)
    {
        NVSDK_NGX_Result r = NVSDK_NGX_D3D12_DestroyParameters(_params);
        NGX_LOG_FAIL(r, "NVSDK_NGX_D3D12_DestroyParameters");
        _params = nullptr;
    }

    if (_device)
    {
        NVSDK_NGX_Result r = NVSDK_NGX_D3D12_Shutdown1(_device);
        NGX_LOG_FAIL(r, "NVSDK_NGX_D3D12_Shutdown1");
    }

    _device      = nullptr;
    _initialized = false;
}

bool NGXDLSS::QueryOptimal(DlssPerfQuality mode, uint32_t displayW, uint32_t displayH, uint32_t& outRenderW,
                           uint32_t& outRenderH, float& outSharpness)
{
    if (!_initialized || !_params)
        return false;

    // 헬퍼 콜백 경유(능력 파라미터 필요)
    unsigned int optW = 0, optH = 0, maxW = 0, maxH = 0, minW = 0, minH = 0;
    float        sharp = 0.0f;

    NVSDK_NGX_Result r = NGX_DLSS_GET_OPTIMAL_SETTINGS(_params, displayW, displayH, MapMode(mode), &optW, &optH, &maxW,
                                                       &maxH, &minW, &minH, &sharp);

    if (NVSDK_NGX_FAILED(r))
    {
        // 폴백: 간단 스케일
        float scale = 0.58f;
        switch (mode)
        {
        case DlssPerfQuality::Quality:
            scale = 0.6667f;
            break;
        case DlssPerfQuality::Balanced:
            scale = 0.58f;
            break;
        case DlssPerfQuality::Performance:
            scale = 0.5f;
            break;
        case DlssPerfQuality::UltraQuality:
            scale = 0.77f;
            break;
        case DlssPerfQuality::UltraPerformance:
            scale = 0.3333f;
            break;
        case DlssPerfQuality::DLAA:
            scale = 1.0f;
            break; // 업스케일 없음
        }
        optW  = (unsigned)(displayW * scale) & ~1u;
        optH  = (unsigned)(displayH * scale) & ~1u;
        sharp = 0.0f;
    }

    outRenderW   = optW;
    outRenderH   = optH;
    outSharpness = sharp;
    return true;
}

bool NGXDLSS::CreateOrUpdateFeature(ID3D12GraphicsCommandList* cmd, uint32_t renderW, uint32_t renderH,
                                    uint32_t outputW, uint32_t outputH, bool depthInverted, bool hdr,
                                    bool enableOutputSubrects)
{
    if (!_initialized || !_params || !cmd)
        return false;

    const NVSDK_NGX_PerfQuality_Value perf = MapMode(_mode);

    int flags = NVSDK_NGX_DLSS_Feature_Flags_None;
    if (hdr)
        flags |= NVSDK_NGX_DLSS_Feature_Flags_IsHDR;
    if (depthInverted)
        flags |= NVSDK_NGX_DLSS_Feature_Flags_DepthInverted;
    // 추가 옵션이 필요하면 여기에 플래그 OR

    const bool needRecreate = (_dlssHandle == nullptr) || (_createdRenderW != renderW) ||
                              (_createdRenderH != renderH) || (_createdOutputW != outputW) ||
                              (_createdOutputH != outputH) || (_createdPerf != perf) || (_createdFlags != flags);

    if (!needRecreate)
        return true;

    if (_dlssHandle)
    {
        NVSDK_NGX_Result rRel = NVSDK_NGX_D3D12_ReleaseFeature(_dlssHandle);
        NGX_LOG_FAIL(rRel, "NVSDK_NGX_D3D12_ReleaseFeature");
        _dlssHandle = nullptr;
    }

    NVSDK_NGX_DLSS_Create_Params cp{};
    cp.Feature.InWidth            = renderW;
    cp.Feature.InHeight           = renderH;
    cp.Feature.InTargetWidth      = outputW;
    cp.Feature.InTargetHeight     = outputH;
    cp.Feature.InPerfQualityValue = perf;
    cp.InFeatureCreateFlags       = flags;
    cp.InEnableOutputSubrects     = enableOutputSubrects;

    // 단일 노드 환경(멀티어댑터 아니면 1/1)
    NVSDK_NGX_Result r = NGX_D3D12_CREATE_DLSS_EXT(cmd,
                                                   /*CreationNodeMask*/ 1u,
                                                   /*VisibilityNodeMask*/ 1u, &_dlssHandle, _params, &cp);

    NGX_LOG_FAIL(r, "NGX_D3D12_CREATE_DLSS_EXT");
    if (NVSDK_NGX_FAILED(r))
        return false;

    _createdRenderW = renderW;
    _createdRenderH = renderH;
    _createdOutputW = outputW;
    _createdOutputH = outputH;
    _createdPerf    = perf;
    _createdFlags   = flags;
    return true;
}

bool NGXDLSS::Evaluate(const NGXFrameDesc& f)
{
    if (!_initialized || !_enabled || !_dlssHandle || !_params)
        return false;
    if (!f.cmd || !f.color || !f.depth || !f.motion || !f.output)
        return false;

    NVSDK_NGX_D3D12_DLSS_Eval_Params ev{};
    ev.Feature.pInColor    = f.color;
    ev.Feature.pInOutput   = f.output;
    ev.Feature.InSharpness = 0.0f; // 기본: 내부 샤프니스

    ev.pInDepth         = f.depth;
    ev.pInMotionVectors = f.motion;

    ev.InJitterOffsetX = f.jitterX; // 픽셀 단위(렌더 해상도)
    ev.InJitterOffsetY = f.jitterY;

    ev.InRenderSubrectDimensions.Width  = f.renderW;
    ev.InRenderSubrectDimensions.Height = f.renderH;

    ev.InReset    = f.reset ? 1 : 0;
    ev.InMVScaleX = (f.mvScaleX == 0.0f ? 1.0f : f.mvScaleX);
    ev.InMVScaleY = (f.mvScaleY == 0.0f ? 1.0f : f.mvScaleY);

    ev.pInTransparencyMask     = nullptr;
    ev.pInExposureTexture      = f.exposureTexture;
    ev.pInBiasCurrentColorMask = nullptr;

    ev.InPreExposure   = f.preExposure;
    ev.InExposureScale = f.exposureScale;

    // 필요한 경우 추가 GBuffer/마스크를 ev.GBufferSurface.pInAttrib[...]에 채우세요.

    NVSDK_NGX_Result r = NGX_D3D12_EVALUATE_DLSS_EXT(f.cmd, _dlssHandle, _params, &ev);

    NGX_LOG_FAIL(r, "NGX_D3D12_EVALUATE_DLSS_EXT");
    return NVSDK_NGX_SUCCEED(r);
}