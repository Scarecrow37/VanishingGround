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

void EditorModelDetails::OnTickGui()
{
    _animator->Update(UmTime.DeltaTime());
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

    if (ImGui::CollapsingHeader("Light Property"))
    {
        ImGui::ColorEdit3("Color##Light", (float*)&_color);
        ImGui::ColorEdit3("Ambient##Light", (float*)&_ambient);
        ImGui::SliderFloat3("Direction##Light", (float*)&_direction, -1.f, 1.f);
        ImGui::SliderFloat("Intensity##Light", &_intensity, 0.f, 1000.f);
    }

    const auto& model = _meshRenderer->GetModel();
    if (model)
    {
        const auto type = _meshRenderer->GetType();

        if (MeshRenderType::SKELETAL == type)
        {
            const auto& animation      = model->GetAnimation();
            const auto& animationNames = animation->GetAnimations();
            ImGui::Text("Animation");
            ImGui::SameLine();

            if (ImGui::BeginCombo("##Animation", animationNames[_currentAnimationIndex]))
            {
                for (int i = 0; i < animationNames.size(); ++i)
                {
                    bool isSelected = (_currentAnimationIndex == i);
                    if (ImGui::Selectable(animationNames[i], isSelected))
                    {
                        if (_currentAnimationIndex != i)
                            _currentAnimationIndex = i;
                        _animator->ChangeAnimation(animationNames[_currentAnimationIndex]);
                    }
                    // 선택된 항목은 포커스를 줌
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        }

        auto& materials = model->GetMaterials();
        auto& material  = materials[_selectedMeshIndex];

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
            ImGui::Combo("##shadingModel", (int*)&material.Model, shadingModelNames, (int)Material::ShadingModel::END);
            
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Two Sided");
            
            ImGui::TableNextColumn();
            ImGui::Checkbox("##Two Sided", &material.IsTwoSided);

            ImGui::EndTable();
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
        _animator->Initialize(model->GetAnimation(), model->GetSkeleton());
        _meshRenderer->SetAnimator(_animator);

        _filePath = path.front();
        _filePath.replace_extension("UmModel");
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