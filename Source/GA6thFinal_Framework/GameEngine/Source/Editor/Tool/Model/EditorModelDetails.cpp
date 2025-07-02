#include "pch.h"
#include "EditorModelDetails.h"
#include "Engine/GraphicsCore/FBXConverter.h"
#include "Engine/GraphicsCore/MeshRenderer.h"
#include "Engine/GraphicsCore/Model.h"
#include "Engine/GraphicsCore/Animation.h"
#include "Engine/GraphicsCore/Animator.h"
#include "Engine/GraphicsCore/Light.h"

EditorModelDetails::EditorModelDetails()
    : _meshRenderer(std::make_unique<MeshRenderer>(MeshRenderType::STATIC, _worldMatrix))
    , _animator(std::make_shared<Animator>())
    , _mainLight(std::make_unique<Light>())
    , _selectedMeshIndex(0)
{
    SetLabel("Details##model");
    SetDockLayout(ImGuiDir_Right);
}

std::shared_ptr<Model> EditorModelDetails::GetModel() const
{
    return _meshRenderer ? _meshRenderer->GetModel() : std::shared_ptr<Model>();
}

std::shared_ptr<Animator> EditorModelDetails::GetAnimator() const
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
    return std::shared_ptr<Animation>();
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

void EditorModelDetails::OnTickGui()
{
    //if (nullptr != GetModel() && nullptr != GetAnimator() && nullptr != GetAnimation())
    //{
    //    _animator->SetPause(_isAnimationPlaying);
    //    if (true == _isAnimationPlaying)
    //    {
    //        _animationTime += UmTime.DeltaTime() * _animationSpeed;
    //    }
    //    _animator->SetAnimationTime(_animationTime);
    //    float min = 0.0f;
    //    float max = _animator->GetCurrentAnimationLastTime();
    //    if (_animationTime > max)
    //    {
    //        if (true == _isAnimationLooping)
    //        {
    //            _animationTime -= max; 
    //        }
    //        else
    //        {
    //            _animationTime      = max;   // Stop at the end of the animation
    //            _isAnimationPlaying = false; // Stop playing when reaching the end
    //        }
    //    }
    //}
    //else
    //{
    //    _animationTime = 0.0f;
    //}

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
    UmRenderer.RegisterRenderQueue("ModelViewer", _meshRenderer.get());
    UmLightCore.RegisterLight("ModelViewer", _mainLight.get());

    _color = Vector3(1.f);
    _ambient = Vector3(1.f);
    _direction = Vector3(0.f, -1.f, 1.f);
    _intensity = 1.f;
    _mainLight->SetDirectionalLight(_color, _ambient, _direction, _intensity);

    _mainLight->SetActive(&_isLightActive);
}

void EditorModelDetails::OnEndGui() {}

void EditorModelDetails::OnPreFrameBegin() {}

void EditorModelDetails::OnPostFrameBegin() {}

void EditorModelDetails::OnFrameRender()
{
    ImGui::BeginHorizontal("model");
    if (ImGui::Button("Import", ImVec2(100, 50)))
    {
        // FBX or binary Load
        ImportModel();
    }

    if (ImGui::Button("Export", ImVec2(100, 50)))
    {
        ExportModel();
    }
    ImGui::EndHorizontal();

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
    if (model)
    {
        const auto type = _meshRenderer->GetType();
        if (ImGui::TreeNodeEx("Model##details", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // ReadOnly inputText for file path
            int flags = ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_AutoSelectAll;
            std::string filePath  = _filePath.string();
            char* filePathBuffer  = (char*)filePath.c_str();
            size_t filePathLength = filePath.length() + 1;
            ImGui::InputText("File Path##details", filePathBuffer, filePathLength, flags);
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip(filePath.c_str());
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
                    if (ImGui::BeginCombo("##Animation", comboLabel))
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
                    float max = _animator->GetCurrentAnimationLastTime();
                    ImGui::SliderFloat("Current Animation Frame", &_animationTime, min, max);
                    ImGui::DragFloat("Animation Speed", &_animationSpeed, 0.01f);
                }
                ImGui::TreePop();
            }
            ImGui::Separator();

            auto& materials = model->GetMaterials();
            auto& material  = materials[_selectedMeshIndex];

            if (ImGui::TreeNodeEx("Material##details", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::BeginTable("##material", 2, ImGuiTableFlags_Borders))
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    const char* blendModeNames[] = {"Opaque", "Masked", "Translucent", "Additive", "Modulate"};
                    ImGui::Text("Blend Mode");

                    ImGui::TableNextColumn();
                    ImGui::Combo("##blendMode", (int*)&material.Mode, blendModeNames, (int)Material::BlendMode::END);

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    const char* shadingModelNames[] = {"Unlit", "Default Lit"};
                    ImGui::Text("Shading Model");

                    ImGui::TableNextColumn();
                    ImGui::Combo("##shadingModel", (int*)&material.Model, shadingModelNames,
                                 (int)Material::ShadingModel::END);

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Two Sided");

                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##Two Sided", &material.IsTwoSided);

                    ImGui::EndTable();
                }
                ImGui::TreePop();
            }
            ImGui::Separator();

            ImGui::TreePop();
        }
    }
}

void EditorModelDetails::OnFrameClipped() {}

void EditorModelDetails::OnFrameEnd() {}

void EditorModelDetails::OnFrameFocusEnter() {}

void EditorModelDetails::OnFrameFocusStay() {}

void EditorModelDetails::OnFrameFocusExit() {}

void EditorModelDetails::OnFramePopupOpened() {}

FBXConverter& EditorModelDetails::GetFBXConverter()
{
    static FBXConverter fbxConverter;
    return fbxConverter;
}

void EditorModelDetails::ImportModel()
{
    std::vector<File::Path> path;

    if (File::ShowOpenFileDialog(UmApplication.GetHwnd(), L"Import Model", L"",
                                 {{L"Model Files (*.fbx;*.UmModel)", L"*.fbx; *.UmModel\0\0"}}, false, path))
    {
        std::shared_ptr<Model> model = std::make_shared<Model>();

        FBXConverter& fbxConverter = GetFBXConverter();
        fbxConverter.ImportModel(path.front(), model);
        _meshRenderer->SetModel(model);
        _meshRenderer->SetActive(&_isModelActive);
        _animator->Initialize(model->GetAnimation(), model->GetSkeleton());
        _meshRenderer->SetAnimator(_animator);
        _filePath = path.front();
        _filePath.replace_extension("UmModel");

        _animationIndexMap.clear();
        _currentAnimationIndex = -1;
        _currentAnimationName  = "";
        auto& animatoion = model->GetAnimation();
        if (nullptr != animatoion)
        {
            auto& animations = animatoion->GetAnimations();
            for (int i = 0; i < animations.size(); ++i)
            {
                _animationIndexMap[animations[i]] = i;
            }
        }
        StopCurrentAnimation();
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