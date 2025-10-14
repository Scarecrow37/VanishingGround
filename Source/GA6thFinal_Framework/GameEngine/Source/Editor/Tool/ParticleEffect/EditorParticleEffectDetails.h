#pragma once
class EditorParticleEffectDetails : public EditorTool
{
public:
    EditorParticleEffectDetails();
    virtual ~EditorParticleEffectDetails();

    void OnTickGui() override;
    void OnStartGui() override;
    void OnEndGui() override;

    void SetCurrentEffect(class ParticleEffect* curEffect);
    void SetCurrentEmitter(class ParticleEmitter* curEmitter);

private:
    void OnPreFrameBegin() override;
    void OnPostFrameBegin() override;
    void OnFrameClipped() override;
    void OnFrameEnd() override;
    void OnFrameFocusEnter() override;
    void OnFrameFocusStay() override;
    void OnFrameFocusExit() override;
    void OnFrameRender() override;
    void OnFramePopupOpened() override;
    void ProcessPopupFrame() override;
    void ProcessFocusFrame() override;
    void ProcessRenderFrame() override;

    void ShowEmitterDetails();
    void ShowEffectDetails();

    // ──────────────────────────────────────────────────────────────────────────
    // UI Ranges / Sizes (에디터용 상수) — 필요시 프로젝트 전역 설정으로 뺄 수도 있음
    // ──────────────────────────────────────────────────────────────────────────
private:
    //// 공통 미리보기 버튼 크기
    //float _uiTexturePreviewWidth  = 100.0f;
    //float _uiTexturePreviewHeight = 100.0f;

    //// 위치/스케일 등 기본 슬라이더 범위
    //float _uiShapeMin = -10.0f;
    //float _uiShapeMax = 10.0f;

    //// 회전(도 단위) 범위
    //float _uiRotDegMin = -180.0f;
    //float _uiRotDegMax = 180.0f;

    //// 알파 범위
    //float _uiAlphaMin = 0.0f;
    //float _uiAlphaMax = 1.0f;

    //// 라이트 인텐시티/레인지 범위
    //float _uiIntensityMin = 0.0f;
    //float _uiIntensityMax = 10.0f;
    //float _uiRangeMin     = 0.0f;
    //float _uiRangeMax     = 100.0f;

    //// 속도 스케일 범위
    //float _uiVelScaleMin = -1000.0f;
    //float _uiVelScaleMax = 1000.0f;

    //// 토러스 내/외경 보정 마진
    //float _uiTorusInnerMargin = 0.1f;

private:
    class ParticleEffect*  _curEffect  = nullptr;
    class ParticleEmitter* _curEmitter = nullptr;
};
