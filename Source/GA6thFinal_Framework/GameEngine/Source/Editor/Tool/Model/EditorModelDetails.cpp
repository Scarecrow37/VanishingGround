#include "pch.h"
#include "EditorModelDetails.h"
#include "EditorModelTool.h"
#include "Editor/DynamicCamera/EditorDynamicCamera.h"
#include "GraphicsEngine/FBXConverter.h"
#include "GraphicsEngine/Interface/ILight.h"
#include "GraphicsEngine/Interface/IMeshRenderer.h"
#include "GraphicsEngine/Interface/IAnimator.h"

EditorModelDetails::EditorModelDetails()
    : _mainLight(nullptr)
    , _selectedMeshIndex(0)
{    
    //_meshRenderer = std::make_unique<MeshRenderer>(STATIC_MESH, _position, _scale, _quaternion, _worldMatrix, _isDirtyFlag);
    SetLabel("Details##model");
    SetDockLayout(ImGuiDir_Right);
}

std::shared_ptr<Model> EditorModelDetails::GetModel() const
{
    return _meshRenderer ? _meshRenderer->GetModel() : std::shared_ptr<Model>();
}

IAnimator* EditorModelDetails::GetAnimator() const
{
    return _animator;
}

std::shared_ptr<Animation> EditorModelDetails::GetAnimation() const
{
    const auto& model = GetModel();

    if (nullptr != model)
    {
        return model->GetAnimation();
    }

    return nullptr;
}

const std::string& EditorModelDetails::GetCurrentAnimationName() const
{
    return _currentAnimationName;
}

void EditorModelDetails::ChangeAnimation(std::string_view anim) 
{
    if (nullptr != _animator)
    {
        _currentAnimationName = anim.data();
        _currentAnimationIndex = _animationIndexMap[anim.data()];
        _animator->ChangeAnimation(anim.data());
        _animationTime = 0.0f;
    }
}

void EditorModelDetails::SetCurrentAnimationSpeed(float speed) 
{
    _animationSpeed = speed;
}

void EditorModelDetails::SetCurrentAnimationTime(float time) 
{
    _animationTime = time;
}

void EditorModelDetails::PlayCurrentAnimation()
{
    _isAnimationPlaying = false;
    if (nullptr != _animator)
    {
        _animator->SetAnimationTime(0.0f);
    }
}

void EditorModelDetails::ResumeCurrentAnimation()
{
    _isAnimationPlaying = true;
}

void EditorModelDetails::PauseCurrentAnimation() 
{
    _isAnimationPlaying = false;
}

void EditorModelDetails::StopCurrentAnimation() 
{
    _isAnimationPlaying = false;
    if (nullptr != _animator)
    {
        _animator->SetAnimationTime(0.0f);
    }
}

void EditorModelDetails::UpdateModelTransform() 
{
    Matrix matScale     = Matrix::CreateScale(_scale);
    Matrix matRotation  = Matrix::CreateFromYawPitchRoll(_rotation.y, _rotation.x, _rotation.z);
    Matrix matTranslate = Matrix::CreateTranslation(_position);
    _quaternion         = Quaternion::CreateFromYawPitchRoll(_rotation.y, _rotation.x, _rotation.z);

    // 변환 순서: S  R  T
    _worldMatrix = matScale * matRotation * matTranslate;
}

void EditorModelDetails::OnTickGui()
{
    if (nullptr != GetModel() && nullptr != GetAnimator() && nullptr != GetAnimation())
    {
        _animator->SetPause(!_isAnimationPlaying);
        _animator->SetLoop(_isAnimationLooping);
        _animator->SetAnimationSpeed(_animationSpeed);
        _animationTime = _animator->GetCurrentAnimationPlayTime();
        _animator->Update(UmTime.DeltaTime());
    }
}

void EditorModelDetails::OnStartGui()
{
    UmGraphics.CreateMeshRenderer(&_meshRenderer, &_worldMatrix);
    UmGraphics.CreateLight(&_mainLight);

    UmGraphics.RegisterComponent("ModelViewer", _meshRenderer.Get());
    UmGraphics.RegisterComponent("ModelViewer", _mainLight.Get());

    _color = Vector3(1.f);
    _ambient = Vector3(1.f);
    _direction = Vector3(0.f, -1.f, 1.f);
    _intensity = 1.f;
    _mainLight->SetDirectionalLight(_color, _ambient, _direction, _intensity);

    _mainLight->SetActive(&_isLightActive);

    UpdateModelTransform();

    auto& system = Global::editorModule->GetDockWindowSystem();
    auto* modelDock = system.GetDockWindow("Model##dock");
    _modelTool      = modelDock->GetGui<EditorModelTool>();

    UmFileSystem.RegisterFileEventSubscriber(this, {".fbx", ".UmModel"});
}

void EditorModelDetails::OnFrameRender()
{
    ImGui::BeginHorizontal("Model##dock");
    if (ImGui::Button("Import", ImVec2(100, 50)))
    {
        ImportModelWithDialog();
    }

    if (ImGui::Button("Export", ImVec2(100, 50)))
    {
        ExportModel();
    }
    ImGui::EndHorizontal();

    if (ImGui::TreeNodeEx("Camera Property##details"))
    {
        if (_modelTool && _modelTool->GetCamera())
        {
            auto& camera = _modelTool->GetCamera();
            ImGui::Text("Camera Move Speed: ");
            float moveSpeed = camera->GetMoveSpeed();
            if (ImGui::SliderFloat("##camera move speed", &moveSpeed, 0.1f, 100.f))
            {
                camera->SetMoveSpeed(moveSpeed);
            }
            ImGui::Text("Camera Rotation Speed: ");
            float rotationSpeed = camera->GetRotationSpeed();
            if (ImGui::SliderFloat("##camera rotation speed", &rotationSpeed, 0.1f, 50.f))
            {
                camera->SetRotationSpeed(rotationSpeed);
            }
        }
        ImGui::TreePop();
    }

    ImGui::Separator();

    if (ImGui::TreeNodeEx("Light Property##details"))
    {
        ImGui::ColorEdit3("Color##Light", (float*)&_color);
        ImGui::ColorEdit3("Ambient##Light", (float*)&_ambient);
        ImGui::SliderFloat3("Direction##Light", (float*)&_direction, -1.f, 1.f);
        ImGui::SliderFloat("Intensity##Light", &_intensity, 0.f, 1000.f);

        ImGui::TreePop();
    }

    ImGui::Separator();

    const auto& model = _meshRenderer->GetModel();
    if (model && model->IsValid())
    {
        const auto type = _meshRenderer->GetType();
        if (ImGui::TreeNodeEx("Model##details", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // ReadOnly inputText for file path
            int inputFlags = ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_AutoSelectAll;
            std::string filePath  = _filePath.string();
            ImGui::InputText("File Path##details", &filePath, inputFlags);
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip(filePath.c_str());
            }
            ImGui::Separator();

            ImGui::Text("Type: %s", type == STATIC_MESH ? "Static" : "Skeletal");
            ImGui::Text("Mesh Count: %d", model->GetMeshes().size());

            if (ImGui::TreeNodeEx("Transform##details", ImGuiTreeNodeFlags_DefaultOpen))
            {
                bool isDirty = false;
                {
                    ImGui::Text("Position: ");
                    ImGui::DragFloat3("##position", &_position.x, 0.05f) ? isDirty = true : isDirty;
                    ImGui::SameLine();
                    if (ImGui::Button("Reset##position"))
                    {
                        _position = Vector3::Zero;
                        isDirty   = true;
                    }
                }
                {
                    ImGui::Text("Rotation: ");
                    ImGui::DragFloat3("##rotation", &_rotation.x, 0.05f) ? isDirty = true : isDirty;
                    ImGui::SameLine();
                    if (ImGui::Button("Reset##rotation"))
                    {
                        _rotation = Vector3::Zero;
                        isDirty   = true;
                    }
                }
                {
                    ImGui::Text("Scale: ");
                    ImGui::DragFloat3("##scale", &_scale.x, 0.05f) ? isDirty = true : isDirty;
                    ImGui::SameLine();
                    if (ImGui::Button("Reset##scale"))
                    {
                        _scale = Vector3::One;
                        isDirty   = true;
                    }
                }
                if (isDirty)
                {
                    UpdateModelTransform();
                }
                ImGui::TreePop();
            }

            ImGui::Separator();

            if (ImGui::TreeNodeEx("Animation##details", ImGuiTreeNodeFlags_DefaultOpen))
            {
                const auto& animation = model->GetAnimation();
                if (nullptr == animation)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
                    ImGui::Text("this model has no animation data.");
                    ImGui::PopStyleColor();
                }
                else
                {
                    const auto& animationNames = animation->GetAnimations();
                    const char* comboLabel =
                        _currentAnimationIndex == -1 ? "-" : animationNames[_currentAnimationIndex];
                    if (ImGuiHelper::BeginComboInput("##Animation", comboLabel, inputFlags))
                    {
                        for (int i = 0; i < animationNames.size(); ++i)
                        {
                            bool isSelected = (_currentAnimationIndex == i);
                            if (ImGui::Selectable(animationNames[i], isSelected))
                            {
                                _currentAnimationIndex = i;
                                _currentAnimationName  = animationNames[i];
                                ChangeAnimation(animationNames[i]);
                            }
                        }
                        ImGui::EndCombo();
                    }

                    if (true == _currentAnimationName.empty())
                    {
                        ImGui::BeginDisabled();
                    }
                    {
                        bool usePushStyleColor = _isAnimationPlaying;
                        if (true == usePushStyleColor)
                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 0.8f, 0.2f, 1.0f));
                        if (ImGui::Button(EditorIcon::ICON_PLAY))
                            _isAnimationPlaying = !_isAnimationPlaying;
                        if (true == usePushStyleColor)
                            ImGui::PopStyleColor();
                        if (ImGui::IsItemHovered())
                            ImGui::SetTooltip("Play");

                        ImGui::SameLine();

                        if (ImGui::Button(EditorIcon::ICON_PAUSE))
                            PauseCurrentAnimation();
                        if (ImGui::IsItemHovered())
                            ImGui::SetTooltip("Pause");

                        ImGui::SameLine();

                        if (ImGui::Button(EditorIcon::ICON_STOP))
                            StopCurrentAnimation();
                        if (ImGui::IsItemHovered())
                            ImGui::SetTooltip("Stop");
                    }
                    if (true == _currentAnimationName.empty())
                    {
                        ImGui::EndDisabled();
                    }

                    ImGui::Checkbox("Loop", &_isAnimationLooping);

                    float min = 0.0f;
                    float max = _animator ? _animator->GetCurrentAnimationLastTime() : 0.0f;
                    if (ImGui::SliderFloat("Current Animation Frame", &_animationTime, min, max))
                    {
                        if (_animator)
                        {
                            _animator->SetAnimationTime(_animationTime);
                        }
                    }
                    ImGui::DragFloat("Animation Speed", &_animationSpeed, 0.01f);
                }
                ImGui::TreePop();
            }
            ImGui::Separator();

            auto& materials = model->GetMaterials();
            auto& material  = materials[_selectedMeshIndex];

            if (ImGui::TreeNodeEx("Material##details", ImGuiTreeNodeFlags_DefaultOpen))
            {
                /*if (ImGui::BeginTable("##material", 2, ImGuiTableFlags_Borders))
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    const char* blendModeNames[] = {"Opaque", "Masked", "Translucent"};
                    ImGui::Text("Blend Mode");

                    ImGui::TableNextColumn();
                    ImGui::Combo("##blendMode", (int*)&material.BlendMode, blendModeNames,
                (int)Material::BlendModeType::BMT_END);

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    const char* shadingModelNames[] = {"Unlit", "Default Lit"};
                    ImGui::Text("Shading Model");

                    ImGui::TableNextColumn();
                    ImGui::Combo("##shadingModel", (int*)&material.ShadingModel, shadingModelNames,
                (int)Material::ShadingModelType::SMT_END);

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Two Sided");

                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##Two Sided", &material.IsTwoSided);

                    ImGui::EndTable();
                }
                ImGui::TreePop();*/
            }
            ImGui::Separator();

            ImGui::TreePop();
        }
    }
}

void EditorModelDetails::OnRequestedDragDrop(const File::Path& path) 
{
    auto dock = GetOwnerDockWindow();
    if (dock && dock->IsFocusFrame())
    {
        ImportModel(path);
    }
}

FBXConverter& EditorModelDetails::GetFBXConverter()
{
    static FBXConverter fbxConverter;
    return fbxConverter;
}

bool EditorModelDetails::ImportModelWithDialog()
{
    std::vector<std::pair<LPCWSTR, LPCWSTR>> filters = {{L"Model Files (*.fbx;*.UmModel)", L"*.fbx; *.UmModel\0\0"}};
    File::Path importPath;
    bool result = File::ShowOpenFileDialog(NULL, L"Import Model", L"", filters, importPath);
    if (result)
    {
        ImportModel(importPath);
    }
    return result;
}

void EditorModelDetails::ImportModel(const File::Path& path)
{
    std::shared_ptr<Model> model = std::make_shared<Model>();

    FBXConverter& fbxConverter = GetFBXConverter();
    fbxConverter.ImportModel(path, model);

    UmGraphics.SetResource(model, _meshRenderer.Get());
    //UmGraphics.LoadResource(path.wstring(), _meshRenderer.Get());

    _meshRenderer->SetActive(&_isModelActive);
    _filePath = path;
    _filePath.replace_extension("UmModel");

    _currentAnimationIndex = -1;
    _currentAnimationName  = "";
    _animationIndexMap.clear();
    
    if (_animator = _meshRenderer->GetAnimator())
    {
        auto        animation  = model->GetAnimation();
        const auto& animations = animation->GetAnimations();

        for (int i = 0; i < animations.size(); ++i)
        {
            _animationIndexMap[animations[i]] = i;
        }

        StopCurrentAnimation();
    }
    else
    {
        _animator = nullptr;
    }
}

void EditorModelDetails::ExportModel()
{
    File::Path path;

    if (File::ShowSaveFileDialog(UmApplication.GetHwnd(), L"Export Model", L"", L"model.UmModel", {}, path))
    {
        FBXConverter& fbxConverter = GetFBXConverter();
        fbxConverter.ExportModel(path);
    }
    // wchar_t path[MAX_PATH] = L"";
    //
    // OPENFILENAMEW ofn = {};
    // ofn.lStructSize   = sizeof(ofn);
    // ofn.hwndOwner     = UmApplication.GetHwnd();
    // ofn.lpstrFilter   = L"UmModel Files (*.UmModel)\0*.UmModel\0";
    // ofn.lpstrFile     = path;
    // ofn.nMaxFile      = MAX_PATH;
    // ofn.Flags         = OFN_OVERWRITEPROMPT;
    //
    // {L"Model Files (*.UmModel)", L"*.UmModel\0\0"}}
    // if (GetSaveFileNameW(&ofn))
    //{
    //    FBXConverter& fbxConverter = GetFBXConverter();
    //    fbxConverter.ExportModel(path);
    //}
}

void EditorModelDetails::SaveModel()
{
    FBXConverter& fbxConverter = GetFBXConverter();
    fbxConverter.ExportModel(_filePath);
}