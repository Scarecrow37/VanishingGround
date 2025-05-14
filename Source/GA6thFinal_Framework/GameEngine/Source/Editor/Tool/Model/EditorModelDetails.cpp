#include "pch.h"
#include "EditorModelDetails.h"
#include "Engine/GraphicsCore/MeshRenderer.h"
#include "Engine/GraphicsCore/FBXConverter.h"
#include "Engine/GraphicsCore/Model.h"

EditorModelDetails::EditorModelDetails()
    : _meshRenderer(std::make_unique<MeshRenderer>(MeshRenderer::RENDER_TYPE::STATIC, _worldMatrix))
{
    SetLabel("Details##model");
    SetDockLayout(ImGuiDir_Right);
}

void EditorModelDetails::OnTickGui()
{
}

void EditorModelDetails::OnStartGui()
{
    UmRenderer.RegisterRenderQueue("ModelViewer", _meshRenderer.get());
}

void EditorModelDetails::OnEndGui() 
{    
}

void EditorModelDetails::OnPreFrameBegin()
{    
}

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
}

void EditorModelDetails::OnFrameClipped() {}

void EditorModelDetails::OnFrameEnd() {
}

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
    TCHAR      filter[] = L"Model Files (*.fbx;*.UmModel)\0*.fbx;*.UmModel\0\0";
    File::Path path;

    if (File::OpenFileNameBrowser(filter, path))
    {
        std::shared_ptr<Model> model = std::make_shared<Model>();

        FBXConverter& fbxConverter = GetFBXConverter();
        fbxConverter.ImportModel(path, model.get());
        _meshRenderer->SetModel(model);

        _filePath = path;
        _filePath.replace_extension("UmModel");
    }    
}

void EditorModelDetails::ExportModel()
{
    wchar_t path[MAX_PATH] = L"";

    OPENFILENAMEW ofn = {};
    ofn.lStructSize   = sizeof(ofn);
    ofn.hwndOwner     = UmApplication.GetHwnd();
    ofn.lpstrFilter   = L"UmModel Files (*.UmModel)\0*.UmModel\0";
    ofn.lpstrFile     = path;
    ofn.nMaxFile      = MAX_PATH;
    ofn.Flags         = OFN_OVERWRITEPROMPT;

    if (GetSaveFileNameW(&ofn))
    {
        FBXConverter& fbxConverter = GetFBXConverter();
        fbxConverter.ExportModel(path);
    }
}

void EditorModelDetails::SaveModel()
{
    FBXConverter& fbxConverter = GetFBXConverter();
    fbxConverter.ExportModel(_filePath);
}