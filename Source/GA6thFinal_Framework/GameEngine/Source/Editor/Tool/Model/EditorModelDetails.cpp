#include "pch.h"
#include "EditorModelDetails.h"
#include "Engine/GraphicsCore/FBXConverter.h"
#include "Engine/GraphicsCore/MeshRenderer.h"
#include "Engine/GraphicsCore/Model.h"

EditorModelDetails::EditorModelDetails()
    : _meshRenderer(std::make_unique<MeshRenderer>(MeshRenderer::RENDER_TYPE::STATIC, _worldMatrix)),
      _selectedMeshIndex(0)
{
    SetLabel("Details##model");
    SetDockLayout(ImGuiDir_Right);
}

void EditorModelDetails::OnTickGui() {}

void EditorModelDetails::OnStartGui()
{
    UmRenderer.RegisterRenderQueue("ModelViewer", _meshRenderer.get());
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

    const auto& model = _meshRenderer->GetModel();
    if (model)
    {
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

    if (File::ShowOpenFileBrowser(UmApplication.GetHwnd(), L"Import Model", L"",
                                  {{L"Model Files (*.fbx;*.UmModel)", L"*.fbx; *.UmModel\0\0"}}, false, path))
    {
        std::shared_ptr<Model> model = std::make_shared<Model>();

        FBXConverter& fbxConverter = GetFBXConverter();
        fbxConverter.ImportModel(path.front(), model);
        _meshRenderer->SetModel(model);

        _filePath = path.front();
        _filePath.replace_extension("UmModel");
    }
}

void EditorModelDetails::ExportModel()
{
    File::Path path;

    if (File::ShowSaveFileBrowser(UmApplication.GetHwnd(), L"Export Model", L"", L"model.UmModel", path))
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