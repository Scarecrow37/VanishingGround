#pragma once
#include "DLLExportDefine.h"
#include "UmFramework.h"
#include "CameraComponent.h"
class UmCineMotion : public CameraComponent
{
    USING_PROPERTY(UmCineMotion)
public:
    REFLECT_PROPERTY(RailSpeed, ShakeIntensity, ShakeDuration, ShakeFrequency, Current)
    GETTER(float, RailSpeed) { return ReflectFields->RailSpeed; }
    SETTER(float, RailSpeed) { ReflectFields->RailSpeed = value; }
    PROPERTY(RailSpeed)
    GETTER(float, ShakeIntensity) { return _shakeIntensity; }
    SETTER(float, ShakeIntensity) { _shakeIntensity = value; }
    PROPERTY(ShakeIntensity)
    GETTER(float, ShakeDuration) { return _shakeDuration; }
    SETTER(float, ShakeDuration) { _shakeDuration = value; }
    PROPERTY(ShakeDuration)
    GETTER(float, ShakeFrequency) { return _shakeFrequency; }
    SETTER(float, ShakeFrequency) { _shakeFrequency = value; }
    PROPERTY(ShakeFrequency)
    GETTER(float, Current) { return _currentStep; }
    SETTER(float, Current)
    {
        _currentStep = std::clamp(value, 0.f, 1.f);
        _railFlag    = true;
        _pauseFlag   = true;
    }
    PROPERTY(Current)

    GETTER_ONLY(float, Duration) { return ReflectFields->RailLength / (ReflectFields->RailSpeed * _railSpeedScale); }
    PROPERTY(Duration)


public:
    UmCineMotion();
    ~UmCineMotion() override;

protected:
    REFLECT_FIELDS_BEGIN(CameraComponent)
    float                RailSpeed  = 1.f;
    float                RailLength = 0.f;
    std::vector<float>   PositionXTethers;
    std::vector<float>   PositionYTethers;
    std::vector<float>   PositionZTethers;
    std::vector<float>   RotationXTethers;
    std::vector<float>   RotationYTethers;
    std::vector<float>   RotationZTethers;
    std::vector<float>   RotationWTethers;
    std::vector<float>   TimestepTethers;
    UINT                 EaseType      = 0;
    UINT                 EaseFuncType  = 0;
    float                EaseThreshold = 0.5f;
    REFLECT_FIELDS_END(UmCineMotion)

    void OnDrawDebug() override;
    void OnDrawDebugSelected() override;
    void Start() override;
    void Update() override;
    void ImGuiDrawPropertysEvent() override;
    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;

public:
    void AddTether();
    void UndoTether();
    void ClearTethers();

    void StartRail(bool isReverse);
    void PauseRail();
    void StopRail();
    void ResetRail(bool toBegin);

    void BeginShake(float duration, float intensity, float frequency);
    void BeginFeedBackShake(int feedbackValue);
    void BeginHandHeldShake();
    void StopShake();

    void SetHandHeldIntensity(float intensity) { _handHeldIntensity = intensity; }
    void SetHandHeldFrequency(float frequency) { _handHeldFrequency = frequency; }

protected:
    void DrawRail();
    void RunRail();

    void Shake();
    void GetShakeOffset();
    void HandHeldShakeLoop();

    float EaseTimeStep(float step);
    void ApplyTransform();

    std::vector<Vector3>    _posTethers;
    std::vector<Quaternion> _rotTethers;
    float                   _moveTimer         = 0.f;
    bool                    _railFlag          = false;
    bool                    _pauseFlag         = false;
    float                   _currentStep       = 0.f;
    float                   _shakeIntensity    = 0.f;
    float                   _shakeFrequency    = 0.35f;
    float                   _shakeElapsedTimer = 0.f;
    float                   _shakeDuration     = 0.f;
    float                   _shakeAmount       = 0.f;
    bool                    _shakeFlag         = false;
    Vector3                 _shakeDirection    = Vector3::Zero;
    Vector3                 _shakeOffset       = Vector3::Zero;
    Vector3                 _railTargetPos     = Vector3::Zero;
    Quaternion              _railTargetAngle   = Quaternion::Identity;
    Vector3                 _shakeTargetPos    = Vector3::Zero;

    UINT                    _selectedTether    = -1;
    bool                    _reverseFlag       = false;
    bool                    _showEasingFlag    = false;
    std::vector<float>      _easeLog;
    Matrix                  _oldWorldMat    = Matrix::Identity;
    Transform*              _oldParent      = nullptr;
    bool                    _manipulateFlag = false;

    inline constexpr static float _railSpeedScale = 1.5f;

    bool  _handheldShakeFlag     = false;
    float _handHeldIntensity     = 0.f;
    float _handHeldFrequency     = 0.f;


#ifdef _UMEDITOR
    void                                                                 UpdateTetherFromGuizmo();
    void                                                                 PushGuizmo(const Matrix& world);
    void                                                                 PopGuizmo();
    void                                                                 ClearGuizmo();
    void                                                                 RefreshGuizmo();
    void                                                                 DrawGuizmo();
    void                                                                 DrawGuizmoIcon();
    std::vector<std::tuple<SceneGizmo, Matrix, SceneGizmo::DefaultIcon>> _guizmoes;
#endif
};

