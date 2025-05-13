#include "pch.h"
#include "EditorModelTool.h"
#include "Editor/DynamicCamera/EditorDynamicCamera.h"
#include "Engine/GraphicsCore/MeshRenderer.h"
#include "Engine/GraphicsCore/FBXConverter.h"

EditorModelTool::EditorModelTool()
    : _camera(std::make_unique<EditorDynamicCamera>())
    , _meshRenderer(std::make_unique<MeshRenderer>(MeshRenderer::RENDER_TYPE::STATIC, _worldMatrix))
{
    SetLabel("ModelViewer");    
}

void EditorModelTool::OnTickGui()
{
}

void EditorModelTool::OnStartGui()
{
    std::shared_ptr<Camera> camera = UmRenderer.GetCamera("ModelViewer");
    ASSERT((nullptr != camera), L"Camera is nullptr");
    _camera->SetTarget(camera);

    SIZE size = UmCore->App.GetClientSize();
    camera->SetupPerspective(45.f, (float)size.cx / (float)size.cy, 0.1f, 1000.f);
    _meshRenderer->RegisterRenderQueue("ModelViewer");
}

void EditorModelTool::OnEndGui()
{
}

void EditorModelTool::OnPreFrameBegin()
{
    if (ImGui::Button("Import", ImVec2(100, 50)))
    {
        // FBX or binary Load
        TCHAR      filter[] = L"Model File (.fbx, .UmModel)\0*.fbx;*.UmModel\0";
        File::Path out;
        if (File::OpenFileNameBrowser(filter, out))
        {
            ImportFBX(out);
        }
    }
}

void EditorModelTool::OnPostFrameBegin() {}

void EditorModelTool::OnFrameRender()
{
    auto handle = UmRenderer.GetRenderSceneImage("ModelViewer");

    ImVec2 size = ImGui::GetContentRegionAvail();

    ImGui::Image((ImTextureID)handle.ptr, size);

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImRect       rect   = window->Rect(); // 윈도우 전체 영역
    if (ImGui::BeginDragDropTargetCustom(rect, window->ID))
    {
        if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
        {
            DragDropAsset::Data* data    = (DragDropAsset::Data*)payLoad->Data;
            auto                 context = data->pContext->lock();

            if (nullptr != context)
            {
                const auto& path = context->GetPath();
                if (path.extension() == L".fbx" || path.extension() == L".UmModel")
                {
                    ImportFBX(path);
                }
            }
        }

        ImGui::EndDragDropTarget();
    }
}

void EditorModelTool::OnFrameClipped() {}

void EditorModelTool::OnFrameEnd()
{
}

void EditorModelTool::OnFrameFocusEnter() {}

void EditorModelTool::OnFrameFocusStay() {}

void EditorModelTool::OnFrameFocusExit() {}

void EditorModelTool::OnFramePopupOpened() {}

void EditorModelTool::ImportFBX(const std::filesystem::path& path)
{
    FBXConverter fbxConverter;
    fbxConverter.ImportFBX(path);
    fbxConverter.ExportFBX(path);

    //_meshRenderer->LoadModel(path.c_str());
}

void EditorModelTool::ExportFBX() {}