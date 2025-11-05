#include "pchScripts.h"
#include "UmCineMotion.h"

UMREAL_COMPONENT(UmCineMotion)

UmCineMotion::UmCineMotion()  = default;
UmCineMotion::~UmCineMotion() = default;
void UmCineMotion::Start()
{
    ResetRail(true);
}

void UmCineMotion::OnDrawDebug()
{
#ifdef _UMEDITOR
    RefreshGuizmo();
#endif
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
#ifdef _UMEDITOR
    RefreshGuizmo();
#endif
    CameraComponent::OnDrawDebugSelected();
    DrawRail();

#ifdef _UMEDITOR
    UpdateTetherFromGuizmo();
    DrawGuizmoIcon();
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
#ifdef _UMEDITOR
    RefreshGuizmo();
#endif
    RunRail();
    Shake();
    ApplyTransform();
}

void UmCineMotion::ImGuiDrawPropertysEvent()
{
#ifdef _UMEDITOR
    CameraComponent::ImGuiDrawPropertysEvent();
    if (false == _posTethers.empty())
    {
        const size_t idx = (_selectedTether == (UINT)-1 ? 0u : static_cast<size_t>(_selectedTether));
        if (idx < _posTethers.size() && idx < _rotTethers.size())
        {
            Vector3     comboLabelPos = _posTethers[idx];
            Vector3     comboLabelRot = _rotTethers[idx].ToEuler() * Mathf::Rad2Deg;
            std::string comboLabel = std::to_string(comboLabelPos.x) + ", " + std::to_string(comboLabelPos.y) + ", " +
                                     std::to_string(comboLabelPos.z) + " / " + std::to_string(comboLabelRot.x) + ", " +
                                     std::to_string(comboLabelRot.y) + ", " + std::to_string(comboLabelRot.z);

            if (ImGui::BeginCombo("##Tethers", comboLabel.c_str()))
            {
                for (size_t i = 0; i < _posTethers.size(); ++i)
                {
                    bool        isSelected  = _selectedTether == i; // [NOTE] implicit cast; see sentinel note above
                    Vector3     selectedRot = _rotTethers[i].ToEuler() * Mathf::Rad2Deg;
                    std::string selected = std::to_string(_posTethers[i].x) + ", " + std::to_string(_posTethers[i].y) +
                                           ", " + std::to_string(_posTethers[i].z) + " / " +
                                           std::to_string(selectedRot.x) + ", " + std::to_string(selectedRot.y) + ", " +
                                           std::to_string(selectedRot.z);
                    if (ImGui::Selectable(selected.c_str(), isSelected))
                    {
                        _selectedTether = static_cast<UINT>(i);
                    }
                }
                ImGui::EndCombo();
            }

            if (_selectedTether != -1)
            {
                _manipulateFlag                    = false;
                std::array<float, 3> selectedPos   = {_posTethers[_selectedTether].x, _posTethers[_selectedTether].y,
                                                      _posTethers[_selectedTether].z};
                Vector3              selectedEuler = _rotTethers[_selectedTether].ToEuler() * Mathf::Rad2Deg;
                std::array<float, 3> selectedRot   = {selectedEuler.x, selectedEuler.y, selectedEuler.z};
                bool isPosChanged = ImGui::DragFloat3("Position##selected pos", selectedPos.data(), 1.f, -100.f, 100.f);
                bool isRotChanged = ImGui::DragFloat3("Rotation##selected rot", selectedRot.data(), 1.f, -360.f, 360.f);

                if (isPosChanged)
                {
                    _manipulateFlag = true;
                    memcpy(&_posTethers[_selectedTether], selectedPos.data(), sizeof(float) * 3);
                    ReflectFields->RailLength = 0.0f;
                    for (int i = 1; i < static_cast<int>(ReflectFields->TimestepTethers.size()); ++i)
                    {
                        const Vector3 distance = _posTethers[i] - _posTethers[i - 1];
                        const float   len      = std::max(distance.Length(), 0.1f);
                        ReflectFields->RailLength += len;
                        ReflectFields->TimestepTethers[i] = ReflectFields->RailLength;
                    }
                    RefreshGuizmo();
                }
                if (isRotChanged)
                {
                    _manipulateFlag = true;
                    memcpy(&selectedEuler, selectedRot.data(), sizeof(float) * 3);
                    selectedEuler *= Mathf::Deg2Rad;
                    _rotTethers[_selectedTether] = Quaternion::CreateFromYawPitchRoll(selectedEuler);
                    RefreshGuizmo();
                }
            }
        }
    }

    {
        ImGui::BeginDisabled(_railFlag);

        bool isAddTetherButtonPressed = ImGui::Button("Add Tether", {150, 50});
        if (true == isAddTetherButtonPressed)
        {
            AddTether();
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

        ImGui::EndDisabled();
    }

    {
        bool isPlayPressed = ImGui::Button("Start Rail", {100, 50});
        if (true == isPlayPressed)
        {
            StartRail(false);
        }
        ImGui::SameLine();

        bool isReversePressed = ImGui::Button("Reverse Rail", {100, 50});
        if (true == isReversePressed)
        {
            StartRail(true);
        }
        ImGui::SameLine();

        bool isPausePressed = ImGui::Button("Pause Rail", {100, 50});
        if (true == isPausePressed)
        {
            PauseRail();
        }
        ImGui::SameLine();

        bool isStopPressed = ImGui::Button("Stop Rail", {100, 50});
        if (true == isStopPressed)
        {
            StopRail();
        }
        ImGui::SameLine();

        bool isResetBPressed = ImGui::Button("Reset to Begin", {100, 50});
        if (true == isResetBPressed)
        {
            ResetRail(true);
        }
        ImGui::SameLine();

        bool isResetEPressed = ImGui::Button("Reset to End", {100, 50});
        if (true == isResetEPressed)
        {
            ResetRail(false);
        }

    }
    {
        bool isShakePressed = ImGui::Button("Shake", {100, 50});
        if (true == isShakePressed)
        {
            BeginShake(_shakeDuration, _shakeIntensity, _shakeFrequency);
        }
    }
    {
        ImGui::Checkbox("Easing Panel", &_showEasingFlag);
        if (_showEasingFlag)
        {
            const char* easetype = Mathf::EaseNameTable[ReflectFields->EaseType].c_str();
            if (ImGui::BeginCombo("##ease type", easetype))
            {
                for (int i = 0; i < Mathf::EaseNameTable.size(); ++i)
                {
                    bool isSelected = ReflectFields->EaseType == i;
                    if (ImGui::Selectable(Mathf::EaseNameTable[i].c_str(), isSelected))
                    {
                        ReflectFields->EaseType = i;
                    }
                }
                ImGui::EndCombo();
            }
            const char* easefunc = Mathf::EaseFuncNameTable[ReflectFields->EaseFuncType].c_str();
            if (ImGui::BeginCombo("##ease func type", easefunc))
            {
                for (int i = 0; i < Mathf::EaseFuncNameTable.size(); ++i)
                {
                    bool isSelected = ReflectFields->EaseFuncType == i;
                    if (ImGui::Selectable(Mathf::EaseFuncNameTable[i].c_str(), isSelected))
                    {
                        ReflectFields->EaseFuncType = i;
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SliderFloat("Shift Threshold", &ReflectFields->EaseThreshold, 0.f, 1.f);
            ImGui::PlotLines("Ease Graph", _easeLog.data(), (int)_easeLog.size(), 0, NULL, -0.5f, 1.5f,
                             ImVec2(400, 150));
        }
    }
#endif
}

void UmCineMotion::AddTether()
{
    if (_posTethers.size() >= 1)
    {
        Vector3 curPosition = transform->Position;
        float   dx          = curPosition.x - _posTethers[_posTethers.size() - 1].x;
        float   dy          = curPosition.y - _posTethers[_posTethers.size() - 1].y;
        float   dz          = curPosition.z - _posTethers[_posTethers.size() - 1].z;
        float   length      = std::max(Vector3(dx, dy, dz).Length(), 0.1f);
        ReflectFields->RailLength += length;
    }
    ReflectFields->TimestepTethers.push_back(ReflectFields->RailLength);
    _posTethers.push_back(transform->Position);
    _rotTethers.push_back(transform->Rotation);
    _railFlag = false;
#ifdef _UMEDITOR
    PushGuizmo(transform->GetWorldMatrix());
#endif
}

void UmCineMotion::UndoTether()
{
    if (ReflectFields->TimestepTethers.empty())
    {
        return;
    }
    if (!_posTethers.empty())
        _posTethers.pop_back();
    if (!_rotTethers.empty())
        _rotTethers.pop_back();

    ReflectFields->RailLength -= ReflectFields->TimestepTethers.back();
    ReflectFields->TimestepTethers.pop_back();
    _railFlag = false;

    if (!_posTethers.empty())
    {
        if (_selectedTether >= ReflectFields->TimestepTethers.size())
        {
            _selectedTether = (UINT)(ReflectFields->TimestepTethers.size() - 1);
        }
    }
    else
    {
        _selectedTether = (UINT)-1; // [RISK] see type note
    }
#ifdef _UMEDITOR
    PopGuizmo();
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
    ClearGuizmo();
#endif
}

void UmCineMotion::StartRail(bool isReverse)
{
    if (ReflectFields->TimestepTethers.empty())
    {
        return;
    }
    _railFlag    = true;
    _pauseFlag   = false;
    _reverseFlag = isReverse;
    _easeLog.clear();
}

void UmCineMotion::PauseRail()
{
    if (ReflectFields->TimestepTethers.empty())
    {
        return;
    }
    _railFlag  = true;
    _pauseFlag = true;
}

void UmCineMotion::StopRail()
{
    if (ReflectFields->TimestepTethers.empty())
    {
        return;
    }
    _moveTimer = 0;
    _pauseFlag = true;
    _railFlag  = false;
}

void UmCineMotion::DrawRail()
{
    if (false == _posTethers.empty())
    {
        Matrix     parentMat = Matrix::Identity;
        Vector3    parentPos = Vector3::Zero;
        Quaternion parentRot = Quaternion::Identity;
        if (transform->Parent)
        {
            parentMat = transform->Parent->GetWorldMatrix();
            Vector3 s;
            parentMat.Decompose(s, parentRot, parentPos);
            parentRot.Normalize();
        }
        // tether points
        {
            for (int i = 0; i < _posTethers.size(); i++)
            {
                Vector3 tetherPos = Vector3::Transform(_posTethers[i], parentMat);
                if (i == _selectedTether)
                    UmGraphics.DebugDraw3D("Editor", BoundingSphere(tetherPos, 0.2f), Colors::Yellow);
                else
                    UmGraphics.DebugDraw3D("Editor", BoundingSphere(tetherPos, 0.2f), Colors::Red);

                Quaternion rotation = _rotTethers[i] * parentRot;
                Vector3    forward  = Vector3::Transform(Vector3::Forward, rotation);
                Vector3    up       = Vector3::Transform(Vector3::Up, rotation);
                Vector3    right    = Vector3::Transform(Vector3::Right, rotation);

                BoundingOrientedBox shaft;
                shaft.Center      = tetherPos - forward * 0.2f;
                shaft.Extents     = {0.01f, 0.01f, 0.2f};
                shaft.Orientation = rotation;
                UmGraphics.DebugDraw3D("Editor", shaft, Colors::Cyan);

                shaft.Extents     = {0.005f, 0.005f, 0.05f};
                shaft.Center      = tetherPos - forward * 0.38f - right * 0.03f;
                shaft.Orientation = rotation * Quaternion::CreateFromAxisAngle(up, 45 * Mathf::Deg2Rad);
                UmGraphics.DebugDraw3D("Editor", shaft, Colors::Cyan);

                shaft.Extents     = {0.005f, 0.005f, 0.05f};
                shaft.Center      = tetherPos - forward * 0.38f + right * 0.03f;
                shaft.Orientation = rotation * Quaternion::CreateFromAxisAngle(up, -45 * Mathf::Deg2Rad);
                UmGraphics.DebugDraw3D("Editor", shaft, Colors::Cyan);
            }
        }
        // interpolated points
        {
            Vector3    posStep = {0, 0, 0};
            Quaternion rotStep = Quaternion::Identity;
            if (_posTethers.size() >= 2 && !ReflectFields->TimestepTethers.empty())
            {
                float max      = ReflectFields->TimestepTethers[_posTethers.size() - 1];
                float timestep = (max > 0.0f) ? (max / 100.0f) : 0.0f;
                if (timestep > 0.0f)
                {
                    for (float t = 0; t < max; t += timestep)
                    {
                        posStep = Mathf::CatmullRomSpline(ReflectFields->TimestepTethers, _posTethers, t);
                        posStep = Vector3::Transform(posStep, parentMat);

                        UmGraphics.DebugDraw3D("Editor", BoundingSphere(posStep, 0.05f), DEBUG_COLOR);

                        rotStep = Mathf::CatmullRomSpline(ReflectFields->TimestepTethers, _rotTethers, t);
                        rotStep *= parentRot;
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
}

void UmCineMotion::RunRail()
{
    if (_posTethers.empty() || _rotTethers.empty())
        return;
    if (true == _railFlag)
    {
        if (false == _pauseFlag)
        {
            if (ReflectFields->RailLength > 0.f && ReflectFields->RailSpeed > 0.f)
            {
                float duration = (ReflectFields->RailLength / (ReflectFields->RailSpeed * _railSpeedScale));
                _moveTimer += (_reverseFlag ? -1 : 1) * UmTime.DeltaTime() * ReflectFields->RailSpeed * _railSpeedScale;
                _moveTimer   = std::clamp(_moveTimer, 0.f, duration);
                float xAxis  = (duration > 0.f) ? (_moveTimer / duration) : 0.f;
                _currentStep = EaseTimeStep(xAxis);
            }
            else
            {
                _currentStep = 0.f;
            }
        }

        Quaternion angle    = Quaternion::Identity;
        Vector3    position = {0, 0, 0};

        if (ReflectFields->TimestepTethers.size() > 1)
        {
            angle    = Mathf::CatmullRomSpline(ReflectFields->TimestepTethers, _rotTethers,
                                               _currentStep * ReflectFields->RailLength);
            position = Mathf::CatmullRomSpline(ReflectFields->TimestepTethers, _posTethers,
                                               _currentStep * ReflectFields->RailLength);
        }
        _targetPos   = position;
        _targetAngle = angle;
    }
    else
    {
        _targetPos   = transform->Position;
        _targetAngle = transform->Rotation;
    }
}

void UmCineMotion::Shake()
{
    if (true == _shakeFlag)
    {
        _shakeElapsedTimer += UmTime.DeltaTime();
        GetShakeOffset();
        _targetPos += _shakeOffset;
        
        if (_shakeElapsedTimer >= _shakeDuration)
        {
            _shakeFlag         = false;
            _shakeElapsedTimer = 0.f;
        }
    }
}

void UmCineMotion::ResetRail(bool toBegin)
{
    _targetPos   = transform->Position;
    _targetAngle = transform->Rotation;
    _easeLog.clear();
    if (toBegin)
    {
        _moveTimer   = 0.f;
        _currentStep = 0.f;
    }
    else
    {
        if (ReflectFields->RailLength > 0.f && ReflectFields->RailSpeed > 0.f)
        {
            _moveTimer   = (ReflectFields->RailLength / (ReflectFields->RailSpeed * _railSpeedScale));
            _currentStep = 100.f;
        }
    }
    if (ReflectFields->TimestepTethers.size() > 1)
    {
        int idx             = toBegin ? 0 : static_cast<int>(ReflectFields->TimestepTethers.size()) - 1;
        _targetPos          = _posTethers[idx];
        _targetAngle        = _rotTethers[idx];
        transform->Position = _targetPos;
        transform->Rotation = _targetAngle;
    }
}

void UmCineMotion::BeginShake(float duration, float intensity, float frequency)
{
    _shakeFlag         = true;
    Vector3 camUp    = Vector3::Transform(Vector3::Up, _targetAngle);
    Vector3 camRight = Vector3::Transform(Vector3::Right, _targetAngle);
    _shakeDirection  = Random::Range(-1.f, 1.f) * camUp + Random::Range(-1.f, 1.f) * camRight;
    _shakeDirection.Normalize();
    _shakeDuration     = duration;
    _shakeIntensity    = intensity;
    _shakeFrequency    = frequency;
    _shakeElapsedTimer = 0.f;
}

void UmCineMotion::GetShakeOffset()
{
    if (_shakeIntensity <= 0.0f || _shakeFrequency <= 0.0f || _shakeDuration <= 0.0f)
    {
        _shakeOffset = Vector3::Zero;
        return;
    }

    float normalizedTime = std::clamp(_shakeElapsedTimer / _shakeDuration, 0.f, 1.f);
    float envelope       = 1.0f - (normalizedTime * normalizedTime);
    _shakeAmount         = std::clamp(std::sinf(_shakeElapsedTimer * _shakeFrequency * XM_2PI), -1.f, 1.f);
    _shakeAmount *= _shakeIntensity * envelope;
    _shakeOffset = _shakeDirection * _shakeAmount;
}

void UmCineMotion::ApplyTransform()
{
    if (true == _railFlag)
    {
        transform->Rotation = _targetAngle;
        transform->Position = _targetPos;
    }
}

void UmCineMotion::BeginFeedBackShake(int feedbackValue)
{
    constexpr int weakThreshold   = 10;
    constexpr int strongThreshold = 20;

    constexpr float weakIntensity = 0.02f;
    constexpr float weakDuration  = 0.5f;
    constexpr float weakFrequency = 5.f;

    constexpr float midIntensity = 0.035f;
    constexpr float midDuration  = 0.5f;
    constexpr float midFrequency = 5.f;

    constexpr float strongIntensity = 0.06f;
    constexpr float strongDuration  = 0.5f;
    constexpr float strongFrequency = 5.f;

    if (feedbackValue <= 0)
        return;
    if (feedbackValue <= weakThreshold)
    {
        BeginShake(weakDuration, weakIntensity, weakFrequency);
    }
    else if (feedbackValue <= strongThreshold)
    {
        BeginShake(midDuration, midIntensity, midFrequency);
    }
    else
    {
        BeginShake(strongDuration, strongIntensity, strongFrequency);
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
        PushGuizmo(world);
#endif
    }
    ResetRail(true);
}

void UmCineMotion::SerializedReflectEvent()
{
    ReflectFields->PositionXTethers.clear();
    ReflectFields->PositionYTethers.clear();
    ReflectFields->PositionZTethers.clear();
    ReflectFields->RotationXTethers.clear();
    ReflectFields->RotationYTethers.clear();
    ReflectFields->RotationZTethers.clear();
    ReflectFields->RotationWTethers.clear();

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
float UmCineMotion::EaseTimeStep(float step)
{
    float curStep = Mathf::Ease((Mathf::EaseType)ReflectFields->EaseType,
                                (Mathf::EaseFuncType)ReflectFields->EaseFuncType, ReflectFields->EaseThreshold, step);
    if (curStep <= 1.f)
        _easeLog.push_back(curStep);
    return curStep;
}

#ifdef _UMEDITOR
void UmCineMotion::UpdateTetherFromGuizmo()
{
    if (_guizmoes.empty() || _manipulateFlag == true)
        return;

    const Matrix parentInv = (transform->Parent) ? transform->Parent->GetInversWorldMatrix() : Matrix::Identity;

    for (int i = 0; i < static_cast<int>(_guizmoes.size()); ++i)
    {
        auto& [gizmo, worldM, icon] = _guizmoes[i];

        Matrix     localM = worldM * parentInv;
        Vector3    guizmoScale;
        Quaternion guizmoRotation;
        Vector3    guizmoPosition;
        localM.Decompose(guizmoScale, guizmoRotation, guizmoPosition);
        guizmoRotation.Normalize();

        if (i < static_cast<int>(_rotTethers.size()))
            _rotTethers[i] = guizmoRotation;
        if (i < static_cast<int>(_posTethers.size()))
            _posTethers[i] = guizmoPosition;
    }

    ReflectFields->RailLength = 0.0f;
    for (int i = 1; i < static_cast<int>(ReflectFields->TimestepTethers.size()); ++i)
    {
        const Vector3 distance   = _posTethers[i] - _posTethers[i - 1];
        const float   len      = std::max(distance.Length(), 0.1f);
        ReflectFields->RailLength += len;
        ReflectFields->TimestepTethers[i] = ReflectFields->RailLength;
    }
}

void UmCineMotion::PushGuizmo(const Matrix& world)
{
    int size                     = (int)_guizmoes.size();
    auto& [guizmo, matrix, icon] = _guizmoes.emplace_back(this, world, SceneGizmo::DefaultIcon::TETHER);
    guizmo.SetIconTexture(icon);
    guizmo.EventListener.AddListener([this, index = size]() { _selectedTether = index; });

    for (auto& [guizmo, matrix, icon] : _guizmoes)
    {
        guizmo.SetOwnerMatrix(matrix);
    }
}
void UmCineMotion::PopGuizmo()
{
    if (_guizmoes.empty())
        return;

    _guizmoes.pop_back();
}

void UmCineMotion::ClearGuizmo()
{
    _guizmoes.clear();
}

void UmCineMotion::RefreshGuizmo()
{
    if (_oldWorldMat == transform->GetWorldMatrix() && transform->Parent == _oldParent)
    {
        if (_manipulateFlag == false)
            return;
    }
    _oldWorldMat = transform->GetWorldMatrix();
    _oldParent   = transform->Parent;

    for (int i = 0; i < ReflectFields->TimestepTethers.size(); i++)
    {
        Matrix world = Matrix::CreateFromQuaternion(_rotTethers[i]) * Matrix::CreateTranslation(_posTethers[i]);
        if (transform->Parent)
        {
            Matrix pWorld = transform->Parent->GetWorldMatrix();
            world *= pWorld;
        }
        auto& [guizmo, matrix, icon] = _guizmoes[i];
        matrix                       = world;
    }
}

void UmCineMotion::DrawGuizmo()
{
    if (_guizmoes.empty())
        return;

    if (0 <= _selectedTether && _selectedTether < _guizmoes.size())
    {
        auto& [guizmo, matrix, icon] = _guizmoes[_selectedTether];
        guizmo.DrawImGuizmo();
    }
}

void UmCineMotion::DrawGuizmoIcon()
{
    for (auto& [guizmo, matrix, icon] : _guizmoes)
    {
        guizmo.DrawIcon();
    }
}
#endif
