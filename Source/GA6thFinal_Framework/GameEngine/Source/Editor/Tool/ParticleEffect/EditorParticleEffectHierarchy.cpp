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
     EditorDockWindow* effectdock  = system.GetDockWindow("Effect##dock");;
     if (effectdock)
     {
         _editorParticleEffectDetails = effectdock->GetGui<EditorParticleEffectDetails>();
     }
     //light settting
     {
         UmGraphics.CreateLight(&_directionalLight);
         _color            = Vector3(1.f);
         _ambient          = Vector3(1.f);
         _direction        = Vector3(0.f, -1.f, 0.f);
         _intensity        = 1.f;
         _lightActivity    = true;
         _directionalLight->SetDirectionalLight(_color, _ambient, _direction, _intensity);
         _directionalLight->SetActive(&_lightActivity);
         UmGraphics.RegisterComponent("ParticleEditor", _directionalLight.Get());
     }
}

void EditorParticleEffectHierarchy::OnPostFrameBegin()
{
    bool isnewbuttonpressed = ImGui::Button("New", {180, 50});
    if (true == isnewbuttonpressed)
    {
        auto newEffect = UmParticleManager->RegisterEffectOnEditor();
        newEffect->SetLifetime(10.f);
        newEffect->SetEffectName("newEffect");
        _editorParticleEffectDetails->SetCurrentEffect(newEffect);
        _curEffect = newEffect;
        _curEffect->SetPosition(&_effectPosition);
        _curEffect->SetRotation(&_effectRotation);
        _curEffect->SetScale(&_effectScale);
        _curEffect->SetParentMatrix(&_effectWorldMatrix);
        _curEffect->SetBoneFollowFlag(&_boneFlag);
    }

    ImGui::SameLine();

    bool isloadbuttonpressed = ImGui::Button("Load", {180, 50});
    bool isControlOPressed   = ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_O,false);
    if (true == isloadbuttonpressed || true == isControlOPressed)
    {
        LoadEffect();
    }
    ParticleEffect* effect = UmParticleManager->GetCurrentEditorEffect();
    if (nullptr != effect)
    {
        bool isSaveButtonPressed = ImGui::Button("Save", {180, 50});
        bool isControlSPressed =
            ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_S, false);

        if (true == isSaveButtonPressed || true == isControlSPressed)
        {
            File::Path   path;
            std::wstring filename;
            if (File::ShowSaveFileDialog(UmApplication.GetHwnd(), L"Save as vfx file", L"", L"Effect.vfx", {}, path))
            {
                // TODO:: 모듈에 있는 시리얼라이저 가져와야 함
                UmParticleSerializer.Serialize(_curEffect, path);
            }
        }
    }

    // refresh button
    {
        bool isrefreshbutton = ImGui::Button("refresh", {100, 30});
        if (true == isrefreshbutton && nullptr != _curEffect)
        {
            UmParticleManager->RefreshEditor();
        }

        ImGui::SameLine();

        bool isAutorefresh = UmParticleManager->GetAutoRefresh();
        ImGui::Checkbox("Auto Refresh", &isAutorefresh);
        UmParticleManager->SetAutoRefresh(isAutorefresh);
    }
    
    //time scale
    {
        float deltaScale = UmParticleManager->GetDeltaScale();
        ImGui::SliderFloat("Time Speed", &deltaScale, 0.f, 2.f);
        UmParticleManager->SetDeltaScale(deltaScale);
    }

    // directional light setting
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

    //env model load
    {
        _envmodelpath = std::filesystem::absolute(_envmodelpath);
        ImGui::Text(_envmodelpath.string().c_str());
        bool isLoadModelButtonPressed = ImGui::Button("load environment model", {250, 30});
        if (true == isLoadModelButtonPressed)
        {

            HWND                    owner = UmApplication.GetHwnd();
            LPCWSTR                 title = L"Load fbx file";
            std::vector<File::Path> out;
            if (File::ShowOpenFileDialog(UmApplication.GetHwnd(), title, L"",
                                         {{L"Model Files (*.fbx;*.UmModel)", L"*.fbx; *.UmModel\0\0"}}, false, out))
            {
                _envmodelpath = out.front();
            }
            _envmodelpath = std::filesystem::absolute(_envmodelpath);
            LoadEnvironmentModel(_envmodelpath);
            ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
        }
        if (_envmodelpath != File::NULL_PATH)
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

                    // 변환 순서: S  R  T
                    _worldMatrix = matScale * matRotation * matTranslate;
                }
            }
        }
    }

    if (nullptr == effect)
    {
        _curEffect = nullptr;
        return;
    }

    ImGui::Text("current particle count : %d", UmParticleManager->GetTotalCount("ParticleEditor") +
                                                   UmParticleManager->GetRibbonCount("ParticleEditor"));
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.f);


    ImGui::Checkbox("Transform", &_effectTransformHide);
    if (_effectTransformHide)
    {
        {
            ImGui::Text("Position: ");
            ImGui::DragFloat3("##effect position", &_effectPosition.x, 0.05f);

            ImGui::Text("Rotation: ");
            ImGui::DragFloat3("##effect rotation", &_effectRotation.x, 0.05f);
    
            ImGui::Text("Scale: ");
            ImGui::DragFloat3("##effect scale", &_effectScale.x, 0.05f);
        }
    }
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.f);

    ImGui::Checkbox("Move Ribbon", &_effectRotateFlag);
    if (_effectRotateFlag)
    {
        {
            ImGui::Text("Vector: ");
            ImGui::DragFloat3("##rotate vector", &_rotationVelocity.x, 0.05f);
 
            ImGui::Text("Speed: ");
            ImGui::DragFloat("##rotate speed", &_rotationSpeed);
        }
        float delta = UmTime.DeltaTime();
        _elapsedTimer += delta;
        Vector3 _finalRotation = _elapsedTimer * _rotationSpeed * _rotationVelocity;
        _effectWorldMatrix = Matrix::CreateFromYawPitchRoll(_finalRotation);
    }
    else
    {
        _effectWorldMatrix = Matrix::Identity;
    }
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.f);

    LocationShape locationType;
    ParticleType  particleType;
    {
        ImGui::BeginGroup();
        // particleType combobox
        {

            static int  renderrow      = -1;
            const char* renderitems[3] = {"Sprite", "Mesh  ", "Ribbon"};
            static int  renderIdx      = 0;
            ImGui::Text("Render Type    ");
            ImGui::SetNextItemWidth(130);
            ImGui::SameLine();
            if (ImGui::BeginCombo("##Render Type", renderitems[renderIdx]))
            {
                for (int n = 0; n < 3; n++)
                {
                    bool is_selected = (renderIdx == n);
                    if (ImGui::Selectable(renderitems[n], is_selected))
                    {
                        renderIdx = n;
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            particleType = (ParticleType)renderIdx;
        }
        // location combobox
        {
            static int  shapeRow      = -1;
            const char* shapeitems[6] = {"Sphere      ", "Cube        ", "Cylinder    ",
                                         "Cone        ", "Torus       ", "Mesh Surface"};
            static int  shapeIdx      = 0;
            ImGui::Text("Emission Shape");
            ImGui::SetNextItemWidth(130);
            ImGui::SameLine();
            if (ImGui::BeginCombo("##Emission Shape", shapeitems[shapeIdx]))
            {
                for (int n = 0; n < 6; n++)
                {
                    bool is_selected = (shapeIdx == n);
                    if (ImGui::Selectable(shapeitems[n], is_selected))
                    {
                        shapeIdx = n;
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            locationType = (LocationShape)shapeIdx;
        }
        ImGui::EndGroup();
        if (LocationShape::MESH_SURFACE == locationType)
        {
            currentmeshsurfacepath = std::filesystem::absolute(currentmeshsurfacepath);
            ImGui::Text(currentmeshsurfacepath.string().c_str());
            bool isLoadModelButtonPressed = ImGui::Button("load target model", {250, 30});
            if (true == isLoadModelButtonPressed)
            {
                HWND                    owner = UmApplication.GetHwnd();
                LPCWSTR                 title = L"Load fbx file";
                std::vector<File::Path> out;
                if (File::ShowOpenFileDialog(UmApplication.GetHwnd(), title, L"",
                                             {{L"Model Files (*.fbx;*.UmModel)", L"*.fbx; *.UmModel\0\0"}}, false, out))
                {
                    currentmeshsurfacepath = out.front();
                }
            }
            currentmeshsurfacepath = std::filesystem::absolute(currentmeshsurfacepath);
        }
    }
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.f);

    {
        bool isAddButtonPressed = ImGui::Button("Add new Emitter", {250, 30});
        if (true == isAddButtonPressed)
        {
            auto emitter =
                UmParticleManager->RegisterEmitter(_curEffect, 100000, 1000, 20, locationType, {0, 0, 0}, particleType);
            UmGraphics.LoadTextureResource(emitter->_particleRenderModule->GetModelAndTexturePath(), emitter);
            

            // File::Path absolutePath = emitter->_particleRenderModule->GetModelAndTexturePath();
            // absolutePath            = std::filesystem::absolute(absolutePath).generic_string();
            // UmGraphics.LoadTextureResource(std::wstring_view(absolutePath.wstring()), emitter);

            if (auto locator = emitter->_emitLocator->AsMeshSurfaceLocator())
            {
                UmGraphics.LoadModelResource(std::wstring_view(currentmeshsurfacepath.wstring()), emitter);
            }




            emitter->InitializeEditorLight();
        }
        bool isSomeoneChanged = false;
    }
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal,2.f);

    {
        // 부모 노드: 기본 플래그 사용
        ImGuiTreeNodeFlags parent_flags = ImGuiTreeNodeFlags_OpenOnArrow;
        bool               parent_open  = ImGui::TreeNodeEx(_curEffect->GetEffectName().c_str(), parent_flags);

        bool isHovered      = ImGui::IsItemHovered();
        bool isMouseClicked = ImGui::IsMouseClicked(0);
        if (true == isHovered && true == isMouseClicked)
        {
            _editorParticleEffectDetails->SetCurrentEffect(_curEffect);

        }
        if (parent_open)
        {
            ImGui::GetStyle().ItemSpacing.y = 3.f; // 모든 위젯 사이의 기본 세로 간격을 10으로

            // 자식 노드: Leaf 플래그 사용
            ImGuiTreeNodeFlags leaf_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            for (const auto& emitter : _curEffect->GetEmitterList())
            {

                if (ImGui::TreeNodeEx(emitter->GetEmitterName().c_str(), leaf_flags))
                {

                    bool isHovered      = ImGui::IsItemHovered();
                    bool isMouseClicked = ImGui::IsMouseClicked(0);
                    if (true == isHovered && true == isMouseClicked)
                    {
                        _editorParticleEffectDetails->SetCurrentEmitter(emitter);
                        _curEmitter = emitter;
                    }
                    if (emitter == _curEmitter)
                    {
                        ImGui::SameLine();
                        ImVec2 buttonSize(120.0f, 25.0f);                // 버튼 크기 지정
                        float  avail = ImGui::GetContentRegionAvail().x; // 현재 남은 가로 공간

                        // 커서를 오른쪽 끝으로 이동 (버튼 너비만큼 빼고)
                        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + avail - buttonSize.x);

                        bool isRemoveButtonPressed = ImGui::Button("Remove Emitter", buttonSize);
                        if (true == isRemoveButtonPressed)
                        {
                            effect->RemoveEmitter(emitter);
                            _editorParticleEffectDetails->SetCurrentEmitter(nullptr);
                            UmParticleManager->RefreshEditor();
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
    UmGraphics.CreateMeshRenderer(&_meshRenderer, &_worldMatrix);
    UmGraphics.LoadResource(path.wstring(), _meshRenderer.Get());
    _meshRenderer->SetActive(&_isModelActive);
    UmGraphics.RegisterComponent("ParticleEditor", _meshRenderer.Get());
}

void EditorParticleEffectHierarchy::LoadEffect() 
{
    HWND                    owner = UmApplication.GetHwnd();
    LPCWSTR                 title = L"Load vfx file";
    std::vector<File::Path> out;
    if (File::ShowOpenFileDialog(owner, title, L"", {{L"\0", L"*.vfx*\0"}}, false, out))
    {
        auto effect = UmParticleSerializer.Deserialize(this, "", out.front(), true, "ParticleEditor");
        if (effect)
        {
            for (auto& emitter : effect->GetEmitterList())
            {
                File::Path absolutePath = emitter->_particleRenderModule->GetModelAndTexturePath();
                absolutePath            = std::filesystem::absolute(absolutePath).generic_string();
                UmGraphics.LoadTextureResource(std::wstring_view(absolutePath.wstring()), emitter);

                if (LocationShape::MESH_SURFACE == emitter->_locationType)
                {
                    if (auto locator = emitter->_emitLocator->AsMeshSurfaceLocator())
                    {
                        File::Path absolutePath = locator->GetModelPath();
                        absolutePath            = std::filesystem::absolute(absolutePath).generic_string();
                        UmGraphics.LoadModelResource(std::wstring_view(absolutePath.wstring()), emitter);
                    }
                }
            }
        }
        _editorParticleEffectDetails->SetCurrentEffect(effect);
        _curEffect = effect;
        _curEffect->SetPosition(&_effectPosition);
        _curEffect->SetRotation(&_effectRotation);
        _curEffect->SetScale(&_effectScale);
        _curEffect->SetParentMatrix(&_effectWorldMatrix);
        _curEffect->SetBoneFollowFlag(&_boneFlag);
    }
}
