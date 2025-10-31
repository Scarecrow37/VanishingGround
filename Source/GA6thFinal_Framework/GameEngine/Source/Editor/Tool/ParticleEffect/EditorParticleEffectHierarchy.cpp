#include "pch.h"
#include "EditorParticleEffectHierarchy.h"
#include "GraphicsEngine/FBXConverter.h"
#include "GraphicsEngine/Interface/ILight.h"
#include "GraphicsEngine/Interface/IMeshRenderer.h"

 EditorParticleEffectHierarchy::EditorParticleEffectHierarchy()
{
    SetLabel("Hierarchy##particleeffect");
    SetDockLayout(ImGuiDir_Left);
}

EditorParticleEffectHierarchy::~EditorParticleEffectHierarchy() = default;

void EditorParticleEffectHierarchy::OnStartGui()
{
    auto&             system     = Global::editorModule->GetDockWindowSystem();
    EditorDockWindow* effectDock = system.GetDockWindow("Effect##dock");
    if (effectDock)
    {
        _editorParticleEffectDetails = effectDock->GetGui<EditorParticleEffectDetails>();
    }
    // light settting
    {
        UmGraphics.CreateLight(&_directionalLight);
        _color         = Vector3(1.f);
        _ambient       = Vector3(1.f);
        _direction     = Vector3(0.f, -1.f, 0.f);
        _intensity     = 1.f;
        _lightActivity = true;
        _directionalLight->SetDirectionalLight(_color, _ambient, _direction, _intensity);
        _directionalLight->SetActive(&_lightActivity);
        UmGraphics.RegisterComponent("ParticleEditor", _directionalLight.Get());
    }
}

void EditorParticleEffectHierarchy::OnFrameRender()
{
    ParticleEffect* effect = UmParticleManager->GetCurrentEditorEffect();

    // --- New ---
    bool isNewButtonPressed = ImGui::Button("New", {180, 50});
    if (isNewButtonPressed)
    {
        auto* newEffect = UmParticleManager->RegisterEffectOnEditor();
        _editorParticleEffectDetails->SetCurrentEffect(newEffect);
        effect = newEffect; // Update local variable for this frame

        // 새 효과 선택 시 현재 이미터 선택 해제
        _curEmitter = nullptr;

        if (effect)
        {
            effect->SetLifetime(10.f);
            effect->SetEffectName("newEffect");
            effect->SetPosition(&_effectPosition);
            effect->SetRotation(&_effectRotation);
            effect->SetScale(&_effectScale);
            effect->SetParentMatrix(&_effectWorldMatrix);
            effect->SetBoneFollowFlag(&_boneFlag);
        }
    }
    ImGui::SameLine();

    // --- Load ---
    bool isLoadButtonPressed = ImGui::Button("Load", {180, 50});
    bool isControlOPressed =
        ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_O, false); // (필요시 ImGuiMod_Ctrl로 교체)
    if (isLoadButtonPressed || isControlOPressed)
    {
        LoadEffect();
        effect      = UmParticleManager->GetCurrentEditorEffect();
        _curEmitter = nullptr; // 로드 후 선택 초기화
    }

    // --- Save ---
    if (effect != nullptr)
    {
        bool isSaveButtonPressed = ImGui::Button("Save", {180, 50});
        bool isControlSPressed   = ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_S, false);
        if (isSaveButtonPressed || isControlSPressed)
        {
            File::Path   path;
            std::wstring filename;
            if (File::ShowSaveFileDialog(UmApplication.GetHwnd(), L"Save as vfx file", L"", L"Effect.vfx", {}, path))
            {
                UmParticleSerializer.Serialize(effect, path);
            }
            _currentEffectFilePath = path.string();
        }
    }

    // --- Refresh ---
    {
        bool isRefressButtonPressed = ImGui::Button("refresh", {100, 30});
        if (isRefressButtonPressed && effect != nullptr)
        {
            UmParticleManager->RefreshEditor();
        }
        ImGui::SameLine();

        bool isAutoRefresh = UmParticleManager->GetAutoRefresh();
        ImGui::Checkbox("Auto Refresh", &isAutoRefresh);
        UmParticleManager->SetAutoRefresh(isAutoRefresh);
    }

    // --- Time scale ---
    {
        float deltaScale = UmParticleManager->GetDeltaScale();
        ImGui::SliderFloat("Time Speed", &deltaScale, 0.f, 2.f);
        UmParticleManager->SetDeltaScale(deltaScale);
    }

    // --- Directional light ---
    {
        ImGui::Checkbox("Directional Light", &_lightActivity);
        if (_lightActivity)
        {
            ImGui::Text("Light Direction: ");
            ImGui::DragFloat3("##Light Direction", &_direction.x, 0.05f);

            ImGui::Text("Light Color: ");
            ImGui::ColorEdit3("##Light Color", &_color.x);

            ImGui::Text("Light Ambient: ");
            ImGui::DragFloat3("##Light Ambient", &_ambient.x, 0.05f);

            ImGui::Text("Light Intensity: ");
            ImGui::DragFloat("##Light Intensity", &_intensity, 0.05f);
        }
    }

    // --- Environment model load ---
    {
        _envModelPath = std::filesystem::absolute(_envModelPath);
        ImGui::Text(_envModelPath.string().c_str());
        bool isLoadModelButtonPressed = ImGui::Button("load environment model", {250, 30});
        if (isLoadModelButtonPressed)
        {
            HWND                    owner = UmApplication.GetHwnd();
            constexpr LPCWSTR       title = L"Load fbx file";
            std::vector<File::Path> out;
            if (File::ShowOpenFileDialog(UmApplication.GetHwnd(), title, L"",
                                         {{L"Model Files (*.fbx;*.UmModel)", L"*.fbx; *.UmModel\0\0"}}, false, out))
            {
                _envModelPath = out.front();
            }
            _envModelPath = std::filesystem::absolute(_envModelPath);
            LoadEnvironmentModel(_envModelPath);
            ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
        }
        if (_envModelPath != File::NULL_PATH)
        {
            ImGui::Checkbox("Environment Transform", &_hideModelTransform);
            if (_hideModelTransform)
            {
                bool isDirty = false;
                {
                    ImGui::Text("Position: ");
                    ImGui::DragFloat3("##env position", &_position.x, 0.05f) ? isDirty = true : isDirty;
                    ImGui::SameLine();
                    if (ImGui::Button("Reset##env position"))
                    {
                        _position = Vector3::Zero;
                        isDirty   = true;
                    }
                }
                {
                    ImGui::Text("Rotation: ");
                    ImGui::DragFloat3("##env rotation", &_rotation.x, 0.05f) ? isDirty = true : isDirty;
                    ImGui::SameLine();
                    if (ImGui::Button("Reset##env rotation"))
                    {
                        _rotation = Vector3::Zero;
                        isDirty   = true;
                    }
                }
                {
                    ImGui::Text("Scale: ");
                    ImGui::DragFloat3("##env scale", &_scale.x, 0.05f) ? isDirty = true : isDirty;
                    ImGui::SameLine();
                    if (ImGui::Button("Reset##env scale"))
                    {
                        _scale  = Vector3::One;
                        isDirty = true;
                    }
                }
                if (isDirty)
                {
                    Matrix matScale     = Matrix::CreateScale(_scale);
                    Matrix matRotation  = Matrix::CreateFromYawPitchRoll(_rotation.y, _rotation.x, _rotation.z);
                    Matrix matTranslate = Matrix::CreateTranslation(_position);
                    _quaternion         = Quaternion::CreateFromYawPitchRoll(_rotation.y, _rotation.x, _rotation.z);
                    _worldMatrix        = matScale * matRotation * matTranslate;
                }
            }
        }
    }

    // 효과 없음 → 조기 종료
    if (effect == nullptr)
        return;

    // 매 프레임: 현재 선택된 emitter 생존 검증 (컨테이너 변경/삭제 대비)
    if (_curEmitter)
    {
        bool stillExists = false;
        for (auto const& up : effect->GetEmitterList())
        {
            if (up.get() == _curEmitter)
            {
                stillExists = true;
                break;
            }
        }
        if (!stillExists)
        {
            _curEmitter = nullptr;
            _editorParticleEffectDetails->SetCurrentEmitter(nullptr);
        }
    }

    ImGui::Text("current particle count : %d", UmParticleManager->GetTotalCount("ParticleEditor") +
                                                   UmParticleManager->GetRibbonCount("ParticleEditor"));
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.f);

    ImGui::Checkbox("Transform", &_effectTransformHide);
    if (_effectTransformHide)
    {
        ImGui::Text("Position: ");
        ImGui::DragFloat3("##effect position", &_effectPosition.x, 0.05f);

        ImGui::Text("Rotation: ");
        ImGui::DragFloat3("##effect rotation", &_effectRotation.x, 0.05f);

        ImGui::Text("Scale: ");
        ImGui::DragFloat3("##effect scale", &_effectScale.x, 0.05f);
    }
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.f);

    ImGui::Checkbox("Move Ribbon", &_effectRotateFlag);
    if (_effectRotateFlag)
    {
        ImGui::Text("Vector: ");
        ImGui::DragFloat3("##rotate vector", &_rotationVelocity.x, 0.05f);

        ImGui::Text("Speed: ");
        ImGui::DragFloat("##rotate speed", &_rotationSpeed);

        _elapsedTimer += UmTime.DeltaTime();
        Vector3 _finalRotation = _elapsedTimer * _rotationSpeed * _rotationVelocity;
        _effectWorldMatrix     = Matrix::CreateFromYawPitchRoll(_finalRotation);
    }
    else
    {
        _effectWorldMatrix = Matrix::Identity;
    }
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.f);

    ImGui::Text("FilePath : ");
    ImGui::SameLine();
    ImGui::Text(_currentEffectFilePath.c_str());

    LocationShape locationType;
    ParticleType  particleType;
    {
        ImGui::BeginGroup();
        // particleType combobox
        {
            static int  renderIdx      = 0;
            const char* renderItems[3] = {"Sprite", "Mesh  ", "Ribbon"};
            ImGui::Text("Render Type    ");
            ImGui::SetNextItemWidth(130);
            ImGui::SameLine();
            if (ImGui::BeginCombo("##Render Type", renderItems[renderIdx]))
            {
                for (int n = 0; n < 3; n++)
                {
                    bool isSelected = (renderIdx == n);
                    if (ImGui::Selectable(renderItems[n], isSelected))
                        renderIdx = n;
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            particleType = static_cast<ParticleType>(renderIdx);
        }
        // location combobox
        {
            static int  shapeIdx      = 0;
            const char* shapeItems[6] = {"Sphere      ", "Cube        ", "Cylinder    ",
                                         "Cone        ", "Torus       ", "Mesh Surface"};
            ImGui::Text("Emission Shape");
            ImGui::SetNextItemWidth(130);
            ImGui::SameLine();
            if (ImGui::BeginCombo("##Emission Shape", shapeItems[shapeIdx]))
            {
                for (int n = 0; n < 6; n++)
                {
                    bool isSelected = (shapeIdx == n);
                    if (ImGui::Selectable(shapeItems[n], isSelected))
                        shapeIdx = n;
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            locationType = static_cast<LocationShape>(shapeIdx);
        }
        ImGui::EndGroup();

        if (LocationShape::MESH_SURFACE == locationType)
        {
            _currentMeshSurfaceModelPath = std::filesystem::absolute(_currentMeshSurfaceModelPath);
            ImGui::Text(_currentMeshSurfaceModelPath.string().c_str());
            bool isLoadModelButtonPressed = ImGui::Button("load target model", {250, 30});
            if (isLoadModelButtonPressed)
            {
                HWND                    owner = UmApplication.GetHwnd();
                constexpr LPCWSTR       title = L"Load fbx file";
                std::vector<File::Path> out;
                if (File::ShowOpenFileDialog(UmApplication.GetHwnd(), title, L"",
                                             {{L"Model Files (*.fbx;*.UmModel)", L"*.fbx; *.UmModel\0\0"}}, false, out))
                {
                    _currentMeshSurfaceModelPath = out.front();
                }
            }
            _currentMeshSurfaceModelPath = std::filesystem::absolute(_currentMeshSurfaceModelPath);
        }
    }
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.f);

    // --- Add new Emitter ---
    {
        bool isAddButtonPressed = ImGui::Button("Add new Emitter", {250, 30});
        if (isAddButtonPressed)
        {
            auto* emitter =
                UmParticleManager->RegisterEmitter(effect, 100000, 1000, 20, locationType, {0, 0, 0}, particleType);
            UmGraphics.LoadTextureResource(emitter->_particleRenderModule->GetModelAndTexturePath(), emitter);
            if (auto* locator = emitter->_emitLocator->AsMeshSurfaceLocator())
            {
                UmGraphics.LoadModelResource(std::wstring_view(_currentMeshSurfaceModelPath.wstring()), emitter);
                locator->SetModelPath(_currentMeshSurfaceModelPath.wstring());
            }
            emitter->InitializeEditorLight();
            // 선택 이동(선택 유지 원하면 제거)
            _curEmitter = emitter;
            _editorParticleEffectDetails->SetCurrentEmitter(_curEmitter);
        }
        bool isSomeoneChanged = false; // (원래 변수 유지)
    }
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.f);

    // --- Effect / Emitters tree ---
    {
        ImGuiTreeNodeFlags parentFlags  = ImGuiTreeNodeFlags_OpenOnArrow;
        bool               isParentOpen = ImGui::TreeNodeEx(effect->GetEffectName().c_str(), parentFlags);

        bool isHovered      = ImGui::IsItemHovered();
        bool isMouseClicked = ImGui::IsMouseClicked(0);
        if (isHovered && isMouseClicked)
        {
            _editorParticleEffectDetails->SetCurrentEffect(effect);
        }

        if (isParentOpen)
        {
            ImGui::GetStyle().ItemSpacing.y = 3.f;

            // ❗ 삭제 가능성이 있으므로 인덱스 기반으로 순회
            auto& list = effect->GetEmitterList();
            for (int i = 0; i < static_cast<int>(list.size()); ++i)
            {
                ParticleEmitter* emitter = list[i].get();

                ImGuiTreeNodeFlags leafFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
                if (ImGui::TreeNodeEx(emitter->GetEmitterName().c_str(), leafFlags))
                {
                    bool hovered = ImGui::IsItemHovered();
                    bool clicked = ImGui::IsMouseClicked(0);
                    if (hovered && clicked)
                    {
                        _editorParticleEffectDetails->SetCurrentEmitter(emitter);
                        _curEmitter = emitter;
                    }

                    if (_curEmitter == emitter)
                    {
                        ImGui::SameLine();
                        ImVec2 buttonSize(120.0f, 25.0f);
                        float  avail = ImGui::GetContentRegionAvail().x;
                        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + avail - buttonSize.x);
                        bool isRemoveButtonPressed = ImGui::Button("Remove Emitter", buttonSize);
                        if (isRemoveButtonPressed)
                        {
                            // 인덱스로 가져온 포인터를 즉시 사용한 뒤, 루프를 중단(break)해 iterator 무효화 방지
                            effect->RemoveEmitter(emitter);
                            _editorParticleEffectDetails->SetCurrentEmitter(nullptr);
                            _curEmitter = nullptr;

                            UmParticleManager->RefreshEditor();
                            break; // 중요: 삭제 후 루프 탈출
                        }
                    }
                }
            }
            ImGui::TreePop();
        }
    }
}


void EditorParticleEffectHierarchy::LoadEnvironmentModel(const File::Path& path)
{
    if (!_meshRenderer)
    {
        UmGraphics.CreateMeshRenderer(&_meshRenderer, &_worldMatrix);
        UmGraphics.RegisterComponent("ParticleEditor", _meshRenderer.Get());
    }
    std::shared_ptr<Model> model        = std::make_shared<Model>();
    FBXConverter&          fbxConverter = GetFBXConverter();
    fbxConverter.ImportModel(path, model);

    UmGraphics.SetResource(model, _meshRenderer.Get());
    _meshRenderer->SetActive(&_isModelActive);
}

void EditorParticleEffectHierarchy::LoadEffect()
{
    HWND                    owner = UmApplication.GetHwnd();
    constexpr LPCWSTR                 title = L"Load vfx file";
    std::vector<File::Path> out;
    if (File::ShowOpenFileDialog(owner, title, L"", {{L"\0", L"*.vfx*\0"}}, false, out))
    {
        if (auto loadedEffect = UmParticleSerializer.Deserialize(this, "", out.front(), true, "ParticleEditor"))
        {
            _editorParticleEffectDetails->SetCurrentEffect(loadedEffect);
            if(loadedEffect)
            {
                loadedEffect->SetPosition(&_effectPosition);
                loadedEffect->SetRotation(&_effectRotation);
                loadedEffect->SetScale(&_effectScale);
                loadedEffect->SetParentMatrix(&_effectWorldMatrix);
                loadedEffect->SetBoneFollowFlag(&_boneFlag);
            }
            _currentEffectFilePath = out.front().string();

        }
    }
}

FBXConverter& EditorParticleEffectHierarchy::GetFBXConverter()
{
    static FBXConverter fbxConverter;
    return fbxConverter;
}