#include "pchScripts.h"
#include "UmCineMotion.h"
UMREAL_COMPONENT(UmCineMotion)

UmCineMotion::UmCineMotion()  = default;
UmCineMotion::~UmCineMotion() = default;

void UmCineMotion::Start() 
{
    transform->Position = _posTethers[0];
    transform->Rotation = _rotTethers[0];
}

void UmCineMotion::OnDrawDebug()
{
    CameraComponent::OnDrawDebug();
    if (false == UmCore->IsPlay())
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

#ifdef _UMEDITOR
    UpdateTetherFromGizmo();
    DrawGizmoIcon();
    DrawGuizmo();
#endif

    if (false == UmCore->IsPlay())
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
        if (false == _posTethers.empty())
        {
            Vector3 comboLabelPos = _posTethers[_selectedTether == -1 ? 0 : _selectedTether];
            Vector3 comboLabelRot = _rotTethers[_selectedTether == -1 ? 0 : _selectedTether].ToEuler() * Mathf::Rad2Deg;
            std::string comboLabel = std::to_string(comboLabelPos.x) + ", " + std::to_string(comboLabelPos.y) + ", " +
                                     std::to_string(comboLabelPos.z) + " / " + std::to_string(comboLabelRot.x) + ", " +
                                     std::to_string(comboLabelRot.y) + ", " + std::to_string(comboLabelRot.z);
            if (ImGui::BeginCombo("##Tethers", comboLabel.c_str()))
            {
                for (int i = 0; i < _posTethers.size(); ++i)
                {
                    bool    isSelected  = _selectedTether == i;
                    Vector3 selectedRot = _rotTethers[i].ToEuler() * Mathf::Rad2Deg;

                    std::string selected = std::to_string(_posTethers[i].x) + ", " + std::to_string(_posTethers[i].y) +
                                           ", " + std::to_string(_posTethers[i].z) + " / " +
                                           std::to_string(selectedRot.x) + ", " + std::to_string(selectedRot.y) + ", " +
                                           std::to_string(selectedRot.z);
                    if (ImGui::Selectable(selected.c_str(), isSelected))
                    {
                        _selectedTether = i;
                    }
                }
                ImGui::EndCombo();
            }
        }

        bool isAddTetherButtonPressed = ImGui::Button("Add Tether Here", {150, 50});
        if (true == isAddTetherButtonPressed)
        {
            if (false == _railFlag)
            {
                AddTetherAuto();
            }
        }
        ImGui::SameLine();

        bool isUndoPressed = ImGui::Button("Undo Tether ", {150, 50});
        if (true == isUndoPressed)
        {
            if (false == _railFlag)
            {
                UndoTether();
            }
        }
        ImGui::SameLine();

        bool isClearPressed = ImGui::Button("Clear Tethers", {150, 50});
        if (true == isClearPressed)
        {
            if (false == _railFlag)
            {
                ClearTethers();
            }
        }

        bool isPlayPressed = ImGui::Button("Start Rail", {150, 50});
        if (true == isPlayPressed)
        {
            StartRail(false);
        }
        ImGui::SameLine();

        bool isReversePressed = ImGui::Button("reverse Rail", {150, 50});
        if (true == isReversePressed)
        {
            StartRail(true);
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
            BeginShake(_shakeDuration, _shakeIntensity, _shakeFrequency);
        }
    }
}

void UmCineMotion::AddTether(float timestep)
{
    ReflectFields->TimestepTethers.push_back(timestep);
    _posTethers.push_back(transform->GetWorldPosition());
    _rotTethers.push_back(transform->Rotation);
    _railFlag = false;
}

void UmCineMotion::AddTetherAuto()
{
    if (_posTethers.size() >= 1)
    {
        Vector3 curPosition = transform->GetWorldPosition();
        float   dx          = curPosition.x - _posTethers[_posTethers.size() - 1].x;
        float   dy          = curPosition.y - _posTethers[_posTethers.size() - 1].y;
        float   dz          = curPosition.z - _posTethers[_posTethers.size() - 1].z;
        float   length      = std::max(Vector3(dx, dy, dz).Length(), 0.1f);
        ReflectFields->RailLength += length;
    }
    ReflectFields->TimestepTethers.push_back(ReflectFields->RailLength);
    _posTethers.push_back(transform->GetWorldPosition());
    _rotTethers.push_back(transform->Rotation);
    _railFlag = false;
#ifdef _UMEDITOR
    PushGizmo(transform->GetWorldMatrix());
#endif
}

void UmCineMotion::UndoTether()
{
    if (ReflectFields->TimestepTethers.empty())
    {
        return;
    }
    ReflectFields->RailLength -= ReflectFields->TimestepTethers.back();
    ReflectFields->TimestepTethers.pop_back();
    _posTethers.pop_back();
    _rotTethers.pop_back();
    _railFlag = false;
    if (_selectedTether >= ReflectFields->TimestepTethers.size())
    {
        _selectedTether = (UINT)(ReflectFields->TimestepTethers.size() - 1);
    }
#ifdef _UMEDITOR
    PopGizmo();
#endif
}

void UmCineMotion::ClearTethers()
{
    ReflectFields->RailLength = 0;
    ReflectFields->TimestepTethers.clear();
    _posTethers.clear();
    _rotTethers.clear();
    _railFlag       = false;
    _selectedTether = -1;
#ifdef _UMEDITOR
    ClearGizmo();
#endif
}

void UmCineMotion::StartRail(bool isReverse)
{
    _railFlag  = true;
    _pauseFlag = false;
    _reverseFlag = isReverse;
    //if (false == _posTethers.empty())
    //{
    //    transform->Position = _posTethers[0];
    //    transform->Rotation = _rotTethers[0];
    //}
}

void UmCineMotion::PauseRail()
{
    _railFlag  = true;
    _pauseFlag = true;
}

void UmCineMotion::StopRail()
{
    _moveTimer = 0;
    _pauseFlag = true;
    _railFlag  = false;
}

void UmCineMotion::DrawRail()
{
    if (false == _posTethers.empty())
    {
        // tether points
        {
            for (int i = 0; i < _posTethers.size(); i++)
            {
                if (i == _selectedTether)
                    UmGraphics.DebugDraw3D("Editor", BoundingSphere(_posTethers[i], 0.2f), Colors::Yellow);
                else
                    UmGraphics.DebugDraw3D("Editor", BoundingSphere(_posTethers[i], 0.2f), Colors::Red);
            }
            for (int i = 0; i < _rotTethers.size(); i++)
            {
                Quaternion rotation = _rotTethers[i];
                Vector3    forward  = Vector3::Transform(Vector3::Forward, rotation);
                Vector3    up       = Vector3::Transform(Vector3::Up, rotation);
                Vector3    right    = Vector3::Transform(Vector3::Right, rotation);


                BoundingOrientedBox shaft;
                shaft.Center      = _posTethers[i] - forward * 0.2f;
                shaft.Extents     = {0.01f, 0.01f, 0.2f};
                shaft.Orientation = rotation;
                UmGraphics.DebugDraw3D("Editor", shaft, Colors::Cyan);

                shaft.Extents     = {0.005f, 0.005f, 0.05f};
                shaft.Center      = _posTethers[i] - forward * 0.38f - right * 0.03f;
                shaft.Orientation = rotation * Quaternion::CreateFromAxisAngle(up, 45 * Mathf::Deg2Rad);
                UmGraphics.DebugDraw3D("Editor", shaft, Colors::Cyan);

                shaft.Extents     = {0.005f, 0.005f, 0.05f};
                shaft.Center      = _posTethers[i] - forward * 0.38f + right * 0.03f;
                shaft.Orientation = rotation * Quaternion::CreateFromAxisAngle(up, -45 * Mathf::Deg2Rad);
                UmGraphics.DebugDraw3D("Editor", shaft, Colors::Cyan);


            }
        }
        // interpolated points
        {
            Vector3    posStep = {0, 0, 0};
            Quaternion rotStep = Quaternion::Identity;
            if (_posTethers.size() >= 2)
            {
                float max      = ReflectFields->TimestepTethers[_posTethers.size() - 1];
                float timestep = max / 100;
                for (float i = 0; i < max; i += timestep)
                {
                    posStep = Mathf::CatmullRomSpline(ReflectFields->TimestepTethers, _posTethers, i);
                    UmGraphics.DebugDraw3D("Editor", BoundingSphere(posStep, 0.05f), DEBUG_COLOR);

                    rotStep = Mathf::CatmullRomSpline(ReflectFields->TimestepTethers, _rotTethers, i);
                    Vector3             forward = Vector3::Transform(Vector3::Forward, rotStep);
                    Vector3             up      = Vector3::Transform(Vector3::Up, rotStep);
                    Vector3             right   = Vector3::Transform(Vector3::Right, rotStep);
                    BoundingOrientedBox shaft;
                    shaft.Center      = posStep - forward * 0.1f;
                    shaft.Extents     = {0.01f, 0.01f, 0.1f};
                    shaft.Orientation = rotStep;
                    UmGraphics.DebugDraw3D("Editor", shaft, Colors::Coral);

                    shaft.Extents     = {0.005f, 0.005f, 0.03f};
                    shaft.Center      = posStep - forward * 0.18f - right * 0.02f;
                    shaft.Orientation = rotStep * Quaternion::CreateFromAxisAngle(up, 45 * Mathf::Deg2Rad);
                    UmGraphics.DebugDraw3D("Editor", shaft, Colors::Coral);

                    shaft.Extents     = {0.005f, 0.005f, 0.03f};
                    shaft.Center      = posStep - forward * 0.18f + right * 0.02f;
                    shaft.Orientation = rotStep * Quaternion::CreateFromAxisAngle(up, -45 * Mathf::Deg2Rad);
                    UmGraphics.DebugDraw3D("Editor", shaft, Colors::Coral);
                }
            }
        }
    }
}

void UmCineMotion::RunRail()
{
    if (true == _railFlag)
    {
        if (false == _pauseFlag)
        {
            _moveTimer += UmTime.DeltaTime() * ReflectFields->RailSpeed * (_reverseFlag ? -1 : 1);
            _moveTimer   = std::clamp(_moveTimer, 0.f, ReflectFields->RailLength);
            if (ReflectFields->RailLength > 0.f)
            {
                _currentStep = _moveTimer / ReflectFields->RailLength * 100.f;
            }
            else
            {
                _currentStep = 0.f;
            }
        }
    }

    Quaternion angle    = Quaternion::Identity;
    Vector3    position = {0, 0, 0};

    if (ReflectFields->TimestepTethers.size() > 1)
    {
        angle    = Mathf::CatmullRomSpline(ReflectFields->TimestepTethers, _rotTethers,
                                           _currentStep * ReflectFields->RailLength * 0.01f);
        position = Mathf::CatmullRomSpline(ReflectFields->TimestepTethers, _posTethers,
                                           _currentStep * ReflectFields->RailLength * 0.01f);
    }
    _targetPos   = position;
    _targetAngle = angle;
}

void UmCineMotion::Shake()
{
    if (true == _shakeFlag)
    {
        _shakeElapsedTimer += UmTime.DeltaTime();
        _targetPos += GetShakeOffset(_shakeIntensity, _shakeFrequency, _shakeElapsedTimer);
        if (_shakeElapsedTimer >= _shakeDuration)
        {
            _shakeFlag         = false;
            _shakeElapsedTimer = 0;
        }
    }
}

void UmCineMotion::BeginShake(float duration, float intensity, float frequency)
{
    _shakeFlag         = true;
    _shakeDuration     = duration;
    _shakeIntensity    = intensity;
    _shakeFrequency    = frequency;
    _shakeElapsedTimer = 0.f;
}

DirectX::SimpleMath::Vector3 UmCineMotion::GetShakeOffset(float intensity, float frequency, float time)
{
    if (intensity <= 0.0f || frequency <= 0.0f)
        return Vector3(0, 0, 0);

    const float freq = std::clamp(frequency, 0.01f, 100.0f);

    const float t = time * freq;

    constexpr int   kOctaves    = 4;   
    constexpr float kLacunarity = 2.0f;
    constexpr float kGain       = 0.5f;

    const float nx = Mathf::FBM1D(t + 37.173f, kOctaves, kLacunarity, kGain);
    const float ny = Mathf::FBM1D(t + 101.719f, kOctaves, kLacunarity, kGain);
    const float nz = Mathf::FBM1D(t + 223.357f, kOctaves, kLacunarity, kGain); 

    const float amp = intensity;

    return Vector3(nx * amp, ny * amp, nz * amp);
}

void UmCineMotion::ApplyTransform()
{
    if (true == _railFlag)
    {
        transform->Rotation = _targetAngle;
        transform->Position   = _targetPos;
    }
}

void UmCineMotion::DeserializedReflectEvent()
{
    CameraComponent::DeserializedReflectEvent();
    _posTethers.clear();
    _rotTethers.clear();
    for (int i = 0; i < ReflectFields->TimestepTethers.size(); i++)
    {
        _posTethers.push_back({ReflectFields->PositionXTethers[i], ReflectFields->PositionYTethers[i],
                               ReflectFields->PositionZTethers[i]});

        _rotTethers.push_back({ReflectFields->RotationXTethers[i], ReflectFields->RotationYTethers[i],
                               ReflectFields->RotationZTethers[i], ReflectFields->RotationWTethers[i]});
        Matrix world = Matrix::CreateFromQuaternion(_rotTethers[i]) * Matrix::CreateTranslation(_posTethers[i]);
#ifdef _UMEDITOR
        PushGizmo(world);
#endif
    }
}

void UmCineMotion::SerializedReflectEvent()
{
    ReflectFields->PositionXTethers.clear();
    ReflectFields->PositionYTethers.clear();
    ReflectFields->PositionZTethers.clear();
    ReflectFields->RotationXTethers.clear();
    ReflectFields->RotationYTethers.clear();
    ReflectFields->RotationZTethers.clear();

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
        ReflectFields->RotationWTethers.push_back(rot.w);
    }
}

#ifdef _UMEDITOR
void UmCineMotion::UpdateTetherFromGizmo()
{
    for (int i = 0; i < _gizmoes.size(); i++)
    {
        auto& [gizmo, mat, icon] = _gizmoes[i];
        Vector3    scale;
        Quaternion rot;
        mat.Decompose(scale, _rotTethers[i], _posTethers[i]);
    }
    ReflectFields->RailLength = 0;
    for (int i = 1; i < ReflectFields->TimestepTethers.size(); i++)
    {
        float dx     = _posTethers[i].x - _posTethers[i - 1].x;
        float dy     = _posTethers[i].y - _posTethers[i - 1].y;
        float dz     = _posTethers[i].z - _posTethers[i - 1].z;
        float length = std::max(Vector3(dx, dy, dz).Length(), 0.1f);
        ReflectFields->RailLength += length;
        ReflectFields->TimestepTethers[i] = ReflectFields->RailLength;
    }
}

void UmCineMotion::PushGizmo(const Matrix& world)
{
    int size                    = (int)_gizmoes.size();
    auto& [gizmo, matrix, icon] = _gizmoes.emplace_back(this, world, SceneGizmo::DefaultIcon::TETHER);
    gizmo.SetIconTexture(icon);
    gizmo.EventListener.AddListener([this, index = size]() { _selectedTether = index; });

    for (auto& [gizmo, matrix, icon] : _gizmoes)
    {
        gizmo.SetOwnerMatrix(matrix);
    }
}

void UmCineMotion::PopGizmo()
{
    _gizmoes.pop_back();
}

void UmCineMotion::ClearGizmo()
{
    _gizmoes.clear();
}

void UmCineMotion::DrawGuizmo()
{
    if (0 <= _selectedTether && _selectedTether < _gizmoes.size())
    {
        auto& [gizmo, matrix, icon] = _gizmoes[_selectedTether];
        gizmo.DrawImGuizmo();
    }
}

void UmCineMotion::DrawGizmoIcon()
{
    for (auto& [gizmo, matrix, icon] : _gizmoes)
    {
        gizmo.DrawIcon();
    }
}
#endif
