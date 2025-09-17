#pragma once
#include "DLLExportDefine.h"
#include "UmFramework.h"
#include "CameraComponent.h"
class UmCineMotion : public CameraComponent
{
    inline static constexpr float SHAKE_FREQUENCY = 0.35f;

    USING_PROPERTY(UmCineMotion)
public:
    REFLECT_PROPERTY(RailSpeed, ShakeIntensity, ShakeDuration,Current)
    GETTER(float, RailSpeed) { return ReflectFields->RailSpeed; }
    SETTER(float, RailSpeed) { ReflectFields->RailSpeed = value; }
    PROPERTY(RailSpeed)
    GETTER(float, ShakeIntensity) { return _shakeIntensity; }
    SETTER(float, ShakeIntensity) { _shakeIntensity = value; }
    PROPERTY(ShakeIntensity)
    GETTER(float, ShakeDuration) { return _shakeDuration; }
    SETTER(float, ShakeDuration) { _shakeDuration = value; }
    PROPERTY(ShakeDuration)
    GETTER(float, Current) { return _currentStep; }
    SETTER(float, Current) 
    { 
        _currentStep = std::clamp(value, 0.f, 100.f); 
        _railFlag    = true;
        _pauseFlag   = true;
    }
    PROPERTY(Current)

public:
    UmCineMotion();
    ~UmCineMotion() override;


protected:
    REFLECT_FIELDS_BEGIN(CameraComponent)
    float              RailSpeed  = 1.f;
    float              RailLength = 0.f;
    std::vector<float> PositionXTethers;
    std::vector<float> PositionYTethers;
    std::vector<float> PositionZTethers;
    std::vector<float> RotationXTethers;
    std::vector<float> RotationYTethers;
    std::vector<float> RotationZTethers;
    std::vector<float> TimestepTethers;
    REFLECT_FIELDS_END(UmCineMotion)


    void OnDrawDebug() override;
    void OnDrawDebugSelected() override;
    void Update() override;

    void ImGuiDrawPropertysEvent() override;
    void DeserializedReflectEvent() override;
    void SerializedReflectEvent() override;

#ifdef _UMEDITOR
    void UpdateTetherFromGizmo();
    void PushGizmo();
    void PopGizmo();
    void ClearGizmo();
    void DrawGuizmo();
    void DrawGizmoIcon();

    std::vector<std::tuple<SceneGizmo, Matrix, SceneGizmo::DefaultIcon>> _gizmoes;
#endif

    void RunRail();

    void    UndoTether();
    void    ClearTethers();
    void    DrawRail();
    void    BeginShake(float duration, float intensity);
    Vector3 GetShakeOffset(float intensity, float time);

    void ApplyTransform();


 public:
    void AddTether(float timestep);
    void AddTetherAuto();
    void StartRail();
    void PauseRail();
    void StopRail();
    void Shake();


protected:
    std::vector<Vector3> _posTethers;
    std::vector<Vector3> _rotTethers;
    float                _moveTimer       = 0.f;
    float                _speed           = 1.f;
    bool                 _railFlag       = false;
    bool                 _pauseFlag       = false;
    float                _currentStep       = 0.f;

    float _shakeIntensity    = 0.f;
    float _shakeElapsedTimer = 0.f;
    float _shakeDuration     = 0.f;
    bool  _shakeFlag         = false;

    Vector3 _targetPos = {0, 0, 0};
    Vector3 _targetAngle = {0, 0, 0};

    UINT _selectedTether = -1;

};

