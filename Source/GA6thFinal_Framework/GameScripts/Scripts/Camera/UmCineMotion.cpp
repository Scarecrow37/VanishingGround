#include "pchScripts.h"
#include "UmCineMotion.h"
UmCineMotion::UmCineMotion()  = default;
UmCineMotion::~UmCineMotion() = default;

void UmCineMotion::OnDrawDebug()
{
    CameraComponent::OnDrawDebug();
    if (false == Global::IsPlay())
    {
        RunRail();
        Shake();
        ApplyTransform();
    }
}

void UmCineMotion::OnDrawDebugSelected()
{
    CameraComponent::OnDrawDebugSelected();
    DrawRail();
    if (false == Global::IsPlay())
    {
        RunRail();
        Shake();
        ApplyTransform();
    }
}

void UmCineMotion::Update()
{
    RunRail();
    Shake();
    ApplyTransform();
}

void UmCineMotion::ImGuiDrawPropertysEvent()
{
    CameraComponent::ImGuiDrawPropertysEvent();
    {
        bool isAddTetherButtonPressed = ImGui::Button("Add Tether Here", {150, 50});
        if (true == isAddTetherButtonPressed)
        {
            AddTetherAuto();
        }
        ImGui::SameLine();
        bool isUndoPressed = ImGui::Button("Undo Tether ", {150, 50});
        if (true == isUndoPressed)
        {
            UndoTether();
        }
        ImGui::SameLine();
        bool isClearPressed = ImGui::Button("Clear Tethers", {150, 50});
        if (true == isClearPressed)
        {
            ClearTethers();
        }
        bool isPlayPressed = ImGui::Button("Start Rail", {150, 50});
        if (true == isPlayPressed)
        {
            StartRail();
        }
        ImGui::SameLine();
        bool isPausePressed = ImGui::Button("Pause Rail", {150, 50});
        if (true == isPausePressed)
        {
            PauseRail();
        }
        ImGui::SameLine();
        bool isStopPressed = ImGui::Button("Stop Rail", {150, 50});
        if (true == isStopPressed)
        {
            StopRail();
        }
    }
    {
        bool isShakePressed = ImGui::Button("Shake", {150, 50});
        if (true == isShakePressed)
        {
            BeginShake(_shakeDuration,_shakeIntensity);
        }
    }
}

void UmCineMotion::AddTether(float timestep)
{
    ReflectFields->TimestepTethers.push_back(timestep);
    _posTethers.push_back(transform->GetWorldPosition());
    _rotTethers.push_back(transform->EulerAngle);
}

void UmCineMotion::AddTetherAuto() 
{
    if (_posTethers.size() >= 1)
    {
        Vector3 curPosition = transform->GetWorldPosition();
        float   dx     = curPosition.x - _posTethers[_posTethers.size() - 1].x;
        float   dy     = curPosition.y - _posTethers[_posTethers.size() - 1].y;
        float   dz     = curPosition.z - _posTethers[_posTethers.size() - 1].z;
        float length = Vector3(dx, dy, dz).Length();
        _totalRailLength += length;
        ReflectFields->TimestepTethers.push_back(_totalRailLength);
        _posTethers.push_back(transform->GetWorldPosition());
        _rotTethers.push_back(transform->EulerAngle);

    }
    else
    {
        ReflectFields->TimestepTethers.push_back(0);
        _posTethers.push_back(transform->GetWorldPosition());
        _rotTethers.push_back(transform->EulerAngle);
    }
}

void UmCineMotion::UndoTether()
{
    if (ReflectFields->TimestepTethers.empty())
    {
        return;
    }
    _totalRailLength -= ReflectFields->TimestepTethers.back();
    ReflectFields->TimestepTethers.pop_back();
    _posTethers.pop_back();
    _rotTethers.pop_back();
}

void UmCineMotion::ClearTethers() 
{
    ReflectFields->TimestepTethers.clear();
    _posTethers.clear();
    _rotTethers.clear();
}

void UmCineMotion::StartRail() 
{
    _railfFlag = true;
    _pauseFlag = false;
}

void UmCineMotion::PauseRail() 
{
    _pauseFlag = true;
}

void UmCineMotion::StopRail() 
{
    _moveTimer = 0;
    _pauseFlag = false;
    _railfFlag = false;
    if (false == _posTethers.empty())
    {
        transform->Position = _posTethers[0];
        transform->EulerAngle = _rotTethers[0];
    }
}

void UmCineMotion::Shake() 
{
    if (true == _shakeFlag)
    {
        _shakeElapsedTimer += UmTime.DeltaTime();
        _targetPos += GetShakeOffset(_shakeIntensity, _shakeElapsedTimer);
        if (_shakeElapsedTimer >= _shakeDuration)
        {
            _shakeFlag         = false;
            _shakeElapsedTimer = 0;
        }
    }
}

void UmCineMotion::DrawRail() 
{
    if (false == _posTethers.empty())
    {
        // tether points
        {
            for (auto& pos : _posTethers)
            {
                UmGraphics.DebugDraw3D("Editor", BoundingSphere(pos, 0.2f), Colors::Red);
            }
        }
        // interpolated points
        {
            Vector3 step = {0, 0, 0};
            if (_posTethers.size() > 2)
            {
                float max      = ReflectFields->TimestepTethers[_posTethers.size() - 1];
                float timestep = max / 100;
                for (float i = 0; i < max; i += timestep)
                {
                    step = Mathf::CatmullRomSpline(ReflectFields->TimestepTethers, _posTethers, i);
                    UmGraphics.DebugDraw3D("Editor", BoundingSphere(step, 0.05f), DEBUG_COLOR);
                }
            }
            else if (_posTethers.size() == 2)
            {
                for (float i = 0; i <= 1; i += 0.01f)
                {
                    step = Vector3::Lerp(_posTethers[0], _posTethers[1], i);
                    UmGraphics.DebugDraw3D("Editor", BoundingSphere(step, 0.05f), DEBUG_COLOR);
                }
            }
        }
    }
}

void UmCineMotion::BeginShake(float duration, float intensity) 
{
    _shakeFlag = true;
    _shakeDuration = duration;
    _shakeIntensity = intensity;
    _shakeElapsedTimer = 0.f;
}

DirectX::SimpleMath::Vector3 UmCineMotion::GetShakeOffset(float intensity, float time) 
{
    if (intensity <= 0.0f)
        return Vector3(0, 0, 0);
    float shakeX = sin(time * SHAKE_FREQUENCY + rand()) * intensity;
    float shakeY = cos(time * SHAKE_FREQUENCY + rand()) * intensity;
    float shakeZ = sin((time + 0.5f) * SHAKE_FREQUENCY + +rand()) * intensity; // z는 보통 진폭 작게

    return Vector3(shakeX, shakeY, shakeZ);
}

void UmCineMotion::ApplyTransform() 
{
    if (true == _railFlag)
    {
        transform->Position = _targetPos;
    }
}

void UmCineMotion::RunRail() 
{
    if (true == _railfFlag)
    {
        if (false == _pauseFlag)
        {
            _moveTimer += UmTime.DeltaTime() * ReflectFields->RailSpeed;
        }
        Vector3 angle = Mathf::CatmullRomSpline(ReflectFields->TimestepTethers, _rotTethers, _moveTimer);
        Vector3 position = Mathf::CatmullRomSpline(ReflectFields->TimestepTethers, _posTethers, _moveTimer);
        _targetPos            = position;
        transform->EulerAngle = angle;
    }
}

void UmCineMotion::DeserializedReflectEvent()
{
    CameraComponent::DeserializedReflectEvent();
    _posTethers.clear();
    _rotTethers.clear();
    for (int i = 0; i < ReflectFields->PositionXTethers.size();i++)
    {
        _posTethers.push_back({ReflectFields->PositionXTethers[i], ReflectFields->PositionYTethers[i],
                               ReflectFields->PositionZTethers[i]});
        _rotTethers.push_back({ReflectFields->RotationXTethers[i], ReflectFields->RotationYTethers[i],
                               ReflectFields->RotationZTethers[i]});
     }
}

void UmCineMotion::SerializedReflectEvent()
{
    for (auto& pos : _posTethers)
    {
        ReflectFields->PositionXTethers.push_back(pos.x);
        ReflectFields->PositionYTethers.push_back(pos.y);
        ReflectFields->PositionZTethers.push_back(pos.z);
    }
    for (auto& rot : _rotTethers)
    {
        ReflectFields->RotationXTethers.push_back(rot.x);
        ReflectFields->RotationYTethers.push_back(rot.y);
        ReflectFields->RotationZTethers.push_back(rot.z);
    }
}

