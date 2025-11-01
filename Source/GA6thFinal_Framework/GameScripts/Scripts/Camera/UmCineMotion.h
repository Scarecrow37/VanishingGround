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

    GETTER_ONLY(float, Duration) { return ReflectFields->RailLength / ReflectFields->RailSpeed; }
    PROPERTY(Duration)

public:
    UmCineMotion();
    ~UmCineMotion() override;

    void Start() override;

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
    std::array<float, 3> OriginPosition;
    std::array<float, 4> OriginRotation;
    bool                 OriginFlag;
    UINT                 EaseType      = 0;
    UINT                 EaseFuncType  = 0;
    float                EaseThreshold = 0.5f;
    REFLECT_FIELDS_END(UmCineMotion)

    void OnDrawDebug() override;
    void OnDrawDebugSelected() override;
    void Update() override;

    void ImGuiDrawPropertysEvent() override;
    void DeserializedReflectEvent() override;
    void SerializedReflectEvent() override;

    void UndoTether();
    void ClearTethers();
    void DrawRail();
    void RunRail();

    void    BeginShake(float duration, float intensity, float frequency);
    Vector3 GetShakeOffset(float intensity, float frequency, float time);

    void ApplyTransform();

public:
    void AddTether();
    void StartRail(bool isReverse);
    void PauseRail();
    void StopRail();
    void Shake();
    void ResetRail(bool toBegin );

protected:
    std::vector<Vector3>    _posTethers;
    std::vector<Quaternion> _rotTethers;
    Vector3                 _originPosition    = Vector3::Zero;
    Quaternion              _originRotation    = Quaternion::Identity;
    float                   _moveTimer         = 0.f;
    bool                    _railFlag          = false;
    bool                    _pauseFlag         = false;
    float                   _currentStep       = 0.f;
    float                   _shakeIntensity    = 0.f;
    float                   _shakeFrequency    = 0.35f;
    float                   _shakeElapsedTimer = 0.f;
    float                   _shakeDuration     = 0.f;
    bool                    _shakeFlag         = false;
    Vector3                 _targetPos         = Vector3::Zero;
    Quaternion              _targetAngle       = Quaternion::Identity;
    UINT                    _selectedTether    = -1;
    bool                    _reverseFlag       = false;
    bool                    _showEasingFlag    = false;
    std::vector<float>      _easeLog;
    Matrix                  _oldWorldMat = Matrix::Identity;
    Transform*              _oldParent   = nullptr;
    bool                    _manipulateFlag = false;

    float EaseTimeStep(float step);

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

