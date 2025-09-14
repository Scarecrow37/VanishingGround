#pragma once
#include "UmFramework.h"
#include "CameraComponent.h"
class UmCineMotion : public CameraComponent
{
    USING_PROPERTY(UmCineMotion)

public:
    UmCineMotion();
    ~UmCineMotion() override;

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(CameraComponent)
    float                RailSpeed = 1.f;
    std::vector<float> PositionXTethers;
    std::vector<float> PositionYTethers;
    std::vector<float> PositionZTethers;
    std::vector<float> RotationXTethers;
    std::vector<float> RotationYTethers;
    std::vector<float> RotationZTethers;
    std::vector<float>   TimestepTethers;
    REFLECT_FIELDS_END(UmCineMotion)


    void OnDrawDebug() override;
    void OnDrawDebugSelected() override;
    void Update() override;

    void ImGuiDrawPropertysEvent() override;
    void DeserializedReflectEvent() override;
    void SerializedReflectEvent() override;
    void RunRail();

 public:
    void AddTether(float timestep);
    void AddTetherAuto();
    void UndoTether();
    void ClearTethers();
    void StartRail();
    void PauseRail();
    void StopRail();


protected:
    std::vector<Vector3> _posTethers;
    std::vector<Vector3> _rotTethers;
    float                _totalRailLength = 0.f;
    float                _moveTimer       = 0.f;
    float                _speed           = 1.f;
    bool                 _railfFlag       = false;
    bool                 _pauseFlag       = false;
};
